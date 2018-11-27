#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char ** argv) 
{
    int n, a, d, k;
    int f = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
    sscanf(argv[1], "%d", &n);
    sscanf(argv[3], "%d", &a);
    sscanf(argv[4], "%d", &d);
    sscanf(argv[5], "%d", &k);
    int  fd[1000][2];
    for(int i = 0; i < n; i++) {
        pipe(fd[i]);
    }
    int jj = 0;
    write(fd[0][1], &jj, sizeof(jj));
    for(int i = 0; i < n; i++) {
        pid_t p = fork();
        if(!p) {
            printf("%d  %d\n", getpid(), getppid());
            int por = 0;
            while(por <  n * k) {
                read(fd[i][0], &por, sizeof(por));
                if(por == -1) {
                    exit(0);
                }
                if(por >= k * n) {
                    for(int i = 0; i < n; i++) {
                        int kk = -1;
                        write(fd[i][1], &kk, sizeof(k));
                    }
                    exit(0);
                }
                int otv = a + d * por;
                printf("%d %d %d\n", otv, getpid(), getppid());
                write(f, &otv, sizeof(otv));
                int next = (i == n - 1) ? 0 : i+1;
                int next_por = por + 1; 
                write(fd[next][1], &next_por, sizeof(next_por));
            }
            exit(0);
        } 
        
    }
    while( wait(NULL) != -1);
    return 0;
}