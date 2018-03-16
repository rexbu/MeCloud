#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <net/if.h>
//#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <math.h>
#include <zlib.h>

#define BUFFER_SIZE 10240000
extern "C"{ 
int crypto(char* input, int in_size, char* output, int out_size);
int decrypt(char* input, int in_size, char* output, int out_size);
int cryptoData(unsigned char* input, int in_size, unsigned char* output, int out_size);
int decryptData(unsigned char* input, int in_size, unsigned char* output, int out_size);
}

int crypto(char* input, int in_size, char* output, int out_size){
    if (out_size<2*in_size+1)
    {
        fprintf(stderr, "Output buffer is short!");
        return -1;
    }

    memset(output, 0, out_size);
    unsigned char bit = 0;
    for(int i=0;i<in_size;i++){
        bit++;
        unsigned char c = (unsigned char)input[i] + (bit/5+bit%3);
        sprintf(output+strlen(output), "%02x", c);
    }

    return in_size;
}

int decrypt(char* input, int in_size, char* output, int out_size){
    if (out_size < in_size/2+1)
    {
        fprintf(stderr, "Output buffer is short!");
        return -1;
    }

    unsigned char cmap[256] = {0};
    for (char i = '0'; i <= '9'; ++i)
    {
        cmap[i] = i-'0';
    }
    for (char i = 'a'; i <= 'f'; ++i)
    {
        cmap[i] = i-'a'+10;
    }

    memset(output, 0, out_size);
    unsigned char bit = 0;
    for(int i=0; i<in_size; i+=2){
        bit++;
        unsigned char c = cmap[input[i]]*16 + cmap[input[i+1]];
        c -= (unsigned char)(bit/5 + bit%3);
        output[i/2] = (char)c;
    }
    output[out_size-1] = '\0';
    return in_size/2;
}

int cryptoData(unsigned char* input, int in_size, unsigned char* output, int out_size) {
    if (out_size<in_size+1)
    {
        fprintf(stderr, "Output buffer is short!");
        return -1;
    }

    memset(output, 0, out_size);
    unsigned char bit = 0;
    for(int i=0;i<in_size;i++){
        bit++;
        output[i] = (unsigned char)input[i] + (bit/5+bit%3);
    }

    return in_size;
}

int decryptData(unsigned char* input, int in_size, unsigned char* output, int out_size) {
    if (out_size < in_size+1)
    {
        fprintf(stderr, "Output buffer is short!");
        return -1;
    }

    memset(output, 0, out_size);
    unsigned char bit = 0;
    for(int i=0; i<in_size; i++){
        bit++;
        output[i] = (unsigned char)input[i] - (bit/5+bit%3);
    }

    output[out_size-1] = '\0';
    return in_size;
}
