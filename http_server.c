#include "http_server.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE (1024*10)

int HTTP_run(int port,HTTP_OnRequest_t onRequest, HTTP_OnError_t onError, void* forwardedState) {
    const int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_descriptor == -1) { onError(forwardedState,"unable to open socket!"); return -1; }

    struct sockaddr_in host_addr;
    int addrlen = sizeof(host_addr);
    host_addr.sin_family= AF_INET;
    host_addr.sin_port= htons(port);
    host_addr.sin_addr.s_addr= htonl(INADDR_ANY);

    if(bind(socket_descriptor,(struct sockaddr*)&host_addr, addrlen) != 0)
        if(socket_descriptor == -1) { onError(forwardedState,"unable to bind address!"); return -1; }

    if(listen(socket_descriptor,SOMAXCONN) != 0)
        { onError(forwardedState,"unable to listen!"); return -1; }

    const int running = 1;
    char* buffer = (char*)malloc( BUFF_SIZE*sizeof(char));
    while(running) {
        int conn = accept(socket_descriptor, (struct sockaddr*)&host_addr, (socklen_t *)&addrlen);

        struct sockaddr_in client_addr;
        int client_addrlen = sizeof(client_addr);
        int conn_name = getsockname(conn,(struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);

        if(conn_name < 0)
            { onError(forwardedState,"unable to get connection name"); close(conn); continue; }

        if(conn < 0)
            { onError(forwardedState,"unable to accept connection"); close(conn); continue; }

        const int read_status = read(conn,buffer,BUFF_SIZE);
        if(read_status < 0)
            { onError(forwardedState,"unable to read from connection"); close(conn); continue; }

        OwnedStr request_msg = onRequest(forwardedState,buffer);
        int write_status = write(conn,request_msg.str,strlen(request_msg.str));
        OwnedStr_Free(&request_msg);

        if(write_status < 0)
            { onError(forwardedState,"unable to write to connection"); close(conn); continue; }

        close(conn);
    }

    free(buffer);
    close(socket_descriptor);

    return 0;
}
