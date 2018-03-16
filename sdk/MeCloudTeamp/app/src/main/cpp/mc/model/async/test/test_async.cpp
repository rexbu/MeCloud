    //
//  main.cpp
//  WebSocket
//
//  Created by Rex on 15/1/22.
//  Copyright (c) 2015年 Rex. All rights reserved.
//

#include <iostream>
#include "AsyncSocket.h"
#include "AsyncQueue.h"
#include "ListenSocket.h"
#include "SocketFrame.h"
#include "SocketServer.h"

class ClientSocket:public AsyncSocket{
public:
    ClientSocket(int sock):AsyncSocket(sock){}
    virtual void onWrite(){};
    virtual void onError(int error){};
    virtual void onMessage(async::message_t* msg){
        msg->buf[msg->size] = '\0';
        printf("message:%s\n", msg->buf);
    }
};
class AcceptSocket:public ListenSocket{
public:
    AcceptSocket(int sock):ListenSocket(sock){}
    AsyncSocket* createSocket(int sock){
        return new ClientSocket(sock);
    }
};

int main(int argc, const char * argv[]) {
    bs_log_init("stdout");
    bs_log_set(g_log, LOG_DEBUG, 1);
    
    SocketFrame     frame;
    frame.start();
    
    AcceptSocket    socket(8808);
    SocketServer    server(&socket);
    server.start();
    
    fprintf(stdout, "--------");
    int sock = socket_tcp(true);
    bs_sock_optimize(sock);
    bs_sock_connect(sock, "127.0.0.1", 8808);
    
    ClientSocket client(sock);
    frame.append(&client);
    
    while (1) {
        sleep(3);
        write(sock, "aaaa", 4);
    }
    
    return 0;
}