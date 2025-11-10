#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
// #include "kernel/fs.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: xargs cmd [args]\n");
        exit(1);
    }

    // 先把标准输入读取
    char buf[512];
    int nread = 0, r;
    while ((r = read(0, buf + nread, sizeof(buf)-1-nread)) > 0) // 反复读取直到read 返回 0（EOF）或缓冲区满
    {
        nread += r;
        if (nread > sizeof(buf) - 1) break;
    }
    buf[nread] = 0;

    // 把固定参数加入
    char *args[32];
    int ai = 0;
    for (int i = 1; i < argc; i++, ai++)
        args[ai] = argv[i];
    
    // 解析标准输出里的参数
    char *p = buf;
    while(*p != 0)
    {
        while (*p == ' ' || *p == '\n' || *p == '\t') p++;
        if (*p == 0) break;
        // 预留一个 NULL
        if (ai >= (int)(sizeof(args)/sizeof(args[0])) - 1) break; 
        // 先假定后面的都是真正的一个string
        args[ai++] = p;
        // 跳到 token 末尾并置零
        while (*p && *p != ' ' && *p != '\n' && *p != '\t') p++;
        if (*p) { *p = 0; p++; }
    }
    args[ai] = 0; // argv 末尾必须为 NULL


    if (fork() == 0)
    {
        exec(argv[1], args);
        printf("exec failed\n");
        exit(1);
    }
    else 
        wait(0);
}