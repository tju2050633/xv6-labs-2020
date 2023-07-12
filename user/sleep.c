#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char const *argv[]) {
    // 1. 检查参数个数
    if (argc != 2) {
        fprintf(2, "usage: sleep <time>\n");
        exit(1);
    }
    // 2. 调用sleep函数，atoi函数将字符串转换为整数
    sleep(atoi(argv[1]));
    exit(0);
}
