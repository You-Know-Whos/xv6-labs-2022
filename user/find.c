#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname();
void find(char *path, char *name);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        exit(-1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
void find(char *path, char *name)
{
    char buf[128], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 尝试打开路径
    if((fd = open(path, 0)) < 0)
    {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    // 尝试获取stat
    if(fstat(fd, &st) < 0)
    {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
        case T_DEVICE:
        case T_FILE:
            if (!strcmp(name, fmtname(path)))
                printf("%s/%s\n", path, name);
            break;
        case T_DIR:      
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) /// path长度+'/'+DIRSIZ+0的长度
            {
                printf("ls: path too long\n");
                break;
            }
            while (read(fd, &de, sizeof(de)) == sizeof(de))
            {
                // 未使用过的目录
                if (!de.inum) 
                    continue;
                
                // 给路径末尾加一个'/'和结束符，构造完整路径，p是用来更改buf的指针
                strcpy(buf, path);
                p = buf + strlen(buf);
                *p++ = '/';
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;

                // 跳过 "." 和 ".."
                if (p[0] == '.' && (p[1] == '\0' || (p[1] == '.' && p[2] == '\0')))
                    continue;
                if (stat(buf, &st) < 0) {
                    fprintf(2, "find: cannot stat %s\n", buf);
                    continue;
                }
                if (!strcmp(p, name)) {
                    printf("%s\n", buf);
                }
                if (st.type == T_DIR) {
                    find(buf, name);
                }
            }
            break;
    }
    close(fd);
}
char* fmtname(char *path)
{
    static char buf[DIRSIZ+1];
    char *p;

    for(p=path+strlen(path); p >= path && *p != '/'; p--); // 找到最后位置的文件名
    p++; // 将p指向文件名开头
    
    // 文件名长度大于DIRSIZ，直接输出
    if(strlen(p) >= DIRSIZ) 
        return p;
    // 否则填充空格到DIRSIZ    
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}