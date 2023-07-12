#include "kernel/types.h"
#include "user/user.h"

// 0. 定义pipe的read端和write端
#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

/**
 * @brief 读取左邻居的第一个数据
 * @param lpipe 左邻居的管道符
 * @param pfirst 用于存储第一个数据的地址
 * @return 如果没有数据返回-1,有数据返回0
 */
int lpipe_first_data(int lpipe[2], int *dst) {
    // 从左管道读取数据，如果读取成功，打印出来即为素数
    // 如果读取失败，说明左管道没有数据，返回-1
    if (read(lpipe[RD], dst, sizeof(int)) == sizeof(int)) {
        printf("prime %d\n", *dst);
        return 0;
    }
    return -1;
}

/**
 * @brief 读取左邻居的数据，将不能被first整除的写入右邻居
 * @param lpipe 左邻居的管道符
 * @param rpipe 右邻居的管道符
 * @param first 左邻居的第一个数据
 */
void transmit_data(int lpipe[2], int rpipe[2], int first) {
    int data;
    // 从左管道读取数据
    // 将无法整除的数据传递入右管道
    while (read(lpipe[RD], &data, sizeof(int)) == sizeof(int)) {
        if (data % first) write(rpipe[WR], &data, sizeof(int));
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}

/**
 * @brief 寻找素数
 * @param lpipe 左邻居管道
 */
__attribute__((noreturn)) 
void primes(int lpipe[2]) {
    close(lpipe[WR]);
    int first;  // 左邻居的第一个数据
    // 递归的终止条件：左管道没有数据
    if (lpipe_first_data(lpipe, &first) == 0) {
        int p[2];
        pipe(p);  // 当前的管道
        transmit_data(lpipe, p, first);

        if (fork() == 0) {
            primes(p);  // 递归的思想，但这将在一个新的进程中调用
        } else {
            close(p[RD]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char const *argv[]) {
    // 1. 创建管道
    int p[2];
    pipe(p);

    // 2. 写入初始数据
    for (int i = 2; i <= 35; ++i) write(p[WR], &i, INT_LEN);

    // 3. 创建子进程
    if (fork() == 0) {
        // 3.1 子进程调用primes
        primes(p);
    } else {
        // 3.2 父进程关闭管道，等待子进程结束
        close(p[WR]);
        close(p[RD]);
        wait(0);
    }

    exit(0);
}
