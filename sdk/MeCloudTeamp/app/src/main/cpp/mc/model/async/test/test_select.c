/**
 * file :	test_select.c
 * author :	bushaofeng
 * create :	2015-03-22 09:54
 * func : 
 * history:
 */

#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>
#include<fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

int     fd[2];

void* sthread(void* para){
    fd_set  set;
    int     rv;
    int     optval = 0;
    int     len = sizeof(unsigned int);
    char    buf[1024] = {0};

    FD_ZERO(&set);
    if(pipe(fd)!=0){
        fprintf(stderr, "pipe error\n");
        return NULL;
    }
    setsockopt(fd[0], IPPROTO_TCP, TCP_NODELAY, &optval, len);//禁用NAGLE算法
    setsockopt(fd[1], IPPROTO_TCP, TCP_NODELAY, &optval, len);//禁用NAGLE算法
    FD_SET(fd[0], &set);
    while(1){
        rv = select(fd[0]+1, &set, NULL, NULL, NULL);
        read(fd[0], buf, 1024);
        printf("rv[%d] buf[%s]\n", rv, buf);
    }
}

void* cthread(void* para){
    char*   a = "a";
    sleep(2);
    write(fd[1], a, strlen(a));
    return NULL;
}
int main(){
    pthread_t t;
    t = pthread_create(&t, NULL, sthread, NULL);
    t = pthread_create(&t, NULL, cthread, NULL);
    while(1){
    }
}
