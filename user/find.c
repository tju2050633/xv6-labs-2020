#include "kernel/types.h"

#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char *path, const char *filename) {
    // 1. 准备变量
    char buf[512], *p;  // buf：路径，p：指向路径最后一个'/'之后
    int fd;             // 文件描述符
    struct dirent de;   // 目录项
    struct stat st;     // 文件状态

    // 2. 打开失败，报错
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 3. 获取文件状态
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot fstat %s\n", path);
        close(fd);
        return;
    }

    // 4. 参数错误，find的第一个参数必须是目录
    if (st.type != T_DIR) {
        fprintf(2, "usage: find <DIRECTORY> <filename>\n");
        return;
    }

    // 5. 拼接路径
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        fprintf(2, "find: path too long\n");
        return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';  // p指向最后一个'/'之后

    // 6. 读取目录项
    while (read(fd, &de, sizeof de) == sizeof de) {
        if (de.inum == 0) continue;
        memmove(p, de.name, DIRSIZ);  // 添加路径名称
        p[DIRSIZ] = 0;                // 字符串结束标志
        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }
        // 不要在“.”和“..”目录中递归
        if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
            find(buf, filename);
        } else if (strcmp(filename, p) == 0)
            printf("%s\n", buf);
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    // 检查参数个数
    if (argc != 3) {
        fprintf(2, "usage: find <directory> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
