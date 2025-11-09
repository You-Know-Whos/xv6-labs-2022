#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void filter(int wr);

int main()
{
    int p[2];
    pipe(p);

    if (fork() == 0)
    {
        close(p[1]);
        filter(p[0]);
    }
    else
    {
        close(p[0]);
        for (int i = 2; i  < 36; i++)
            write(p[1],  &i, sizeof(int));
        close(p[1]);
    }
    wait(0);
    exit(0);
}

void filter(int wr)
{
    int val;
    if (read(wr, &val, sizeof(int)) != sizeof(int))
    {   
        close(wr);
        return;
    }
    printf("prime %d\n", val);

    int p[2]; pipe(p);
    int buf, flag = 0;
    while (read(wr, &buf, sizeof(int)) > 0)
    {
        // printf("%d ", buf);
        if (buf % val != 0)
        {
            if (!flag)
            {
                if (fork() == 0)
                {
                    close(p[1]);
                    filter(p[0]);
                    exit(0);
                }
                else
                {
                    flag = 1;
                    close(p[0]);
                }
            }
            write(p[1], &buf, sizeof(int));
        }
    }
    close(p[1]);
    wait(0);
}