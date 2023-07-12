#include "kernel/types.h"
#include "user/user.h"

// 0. 定义pipe的read端和write端
#define RD 0  // pipe的read端
#define WR 1  // pipe的write端

int main(int argc, char const *argv[]) {
    char buf = 'P';  // 用于传送的字节
    // 1. 创建两个管道
    int fd_c2p[2];  // 子进程->父进程
    int fd_p2c[2];  // 父进程->子进程
    pipe(fd_c2p);
    pipe(fd_p2c);

    // 2. 创建子进程
    int pid = fork();
    int exit_status = 0;

    // 3. 如果pid < 0，创建失败
    if (pid < 0) {
        fprintf(2, "fork() error!\n");

        // 3.1 关闭管道
        close(fd_c2p[RD]);
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);
        close(fd_p2c[WR]);

        // 3.2 退出
        exit(1);
    }
    // 4. 如果pid == 0，子进程
    else if (pid == 0) {  // 子进程
        // 4.1 关闭不需要的管道
        close(fd_p2c[WR]);
        close(fd_c2p[RD]);

        // 4.2 读取父进程传来的数据
        if (read(fd_p2c[RD], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child read() error!\n");
            exit_status = 1;  // 标记出错
        } else {
            fprintf(1, "%d: received ping\n", getpid());
        }

        // 4.3 将数据写入管道
        if (write(fd_c2p[WR], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child write() error!\n");
            exit_status = 1;  // 标记出错
        }

        // 4.4 关闭管道
        close(fd_p2c[RD]);
        close(fd_c2p[WR]);

        // 4.5 退出
        exit(exit_status);
    }
    // 5. 如果pid > 0，父进程
    else {  // 父进程
        // 5.1 关闭不需要的管道
        close(fd_p2c[RD]);
        close(fd_c2p[WR]);

        // 5.2 将数据写入管道
        if (write(fd_p2c[WR], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent write() error!\n");
            exit_status = 1;  // 标记出错
        }

        // 5.3 读取子进程传来的数据
        if (read(fd_c2p[RD], &buf, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent read() error!\n");
            exit_status = 1;  // 标记出错
        } else {
            fprintf(1, "%d: received pong\n", getpid());
        }

        // 5.4 关闭管道
        close(fd_p2c[WR]);
        close(fd_c2p[RD]);

        // 5.5 退出
        exit(exit_status);
    }
}
