#include "tcp_server.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <time.h>

struct TCPServer_RequestState {
    int connection;
    const char* request;

    void* forwardedState;
    TCPServer_OnLogPrint_t onLogPrintCallback;
};

const char* TCPServer_GetRequestString(TCPServer_RequestState* s) {
    return s->request;
}

void TCPServer_sendString(TCPServer_RequestState* s, const char* c) {
    int write_status = write(s->connection,c,strlen(c));
    if(write_status < 0)
        s->onLogPrintCallback(s->forwardedState,"unable to write to connection",LogType_Error);
}

void TCPServer_sendStringWithLength(TCPServer_RequestState* s, const char* c, size_t l) {
    int write_status = write(s->connection,c,l);
    if(write_status < 0)
        s->onLogPrintCallback(s->forwardedState,"unable to write to connection",LogType_Error);
}

#include <signal.h>


void TCPServer_sendFile(TCPServer_RequestState* s ,FILE* f) {
    OwnedStr buffor = OwnedStr_AllocFromFile(f);

    int write_status = write(s->connection,buffor.str,buffor.capacity);
    if(write_status < 0)
        s->onLogPrintCallback(s->forwardedState,"unable to write to connection",LogType_Error);

    OwnedStr_Free(&buffor);
}

void* TCPServer_GetForwardedState(TCPServer_RequestState* s) {
    return s->forwardedState;
}

#define BUFF_SIZE (1024*10)

int TCPServer_run(int port,TCPServer_OnRequest_t onRequest, TCPServer_OnLogPrint_t OnLogPrint, void* forwardedState) {
    signal(SIGPIPE, SIG_IGN);

    const int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_descriptor == -1) { OnLogPrint(forwardedState,"unable to open socket!",LogType_Error); return -1; }

    struct sockaddr_in host_addr;
    int addrlen = sizeof(host_addr);
    host_addr.sin_family= AF_INET;
    host_addr.sin_port= htons(port);
    host_addr.sin_addr.s_addr= htonl(INADDR_ANY);

    if(bind(socket_descriptor,(struct sockaddr*)&host_addr, addrlen) != 0)
        if(socket_descriptor == -1) { OnLogPrint(forwardedState,"unable to bind address!",LogType_Error); return -1; }

    if(listen(socket_descriptor,SOMAXCONN) != 0)
        { OnLogPrint(forwardedState,"unable to listen!",LogType_Error); return -1; }

    const int running = 1;
    char* buffer = (char*)malloc( BUFF_SIZE*sizeof(char));

    OnLogPrint(forwardedState,"start listening!",LogType_Info);
    while(running) {
        TCPServer_RequestState req;
        req.forwardedState = forwardedState;
        req.onLogPrintCallback = OnLogPrint;
        req.connection = accept(socket_descriptor, (struct sockaddr *) &host_addr, (socklen_t *) &addrlen);

        struct sockaddr_in client_addr;
        int client_addrlen = sizeof(client_addr);
        int conn_name = getsockname(req.connection, (struct sockaddr *) &client_addr, (socklen_t *) &client_addrlen);

        if (conn_name < 0) {
            OnLogPrint(forwardedState, "unable to get connection name", LogType_Error);
            close(req.connection);
            continue;
        }

        if (req.connection < 0) {
            OnLogPrint(forwardedState, "unable to accept connection", LogType_Error);
            close(req.connection);
            continue;
        }

        const int read_status = read(req.connection, buffer, BUFF_SIZE);
        if (read_status < 0) {
            OnLogPrint(forwardedState, "unable to read from connection", LogType_Error);
            close(req.connection);
            continue;
        }

        req.request = buffer;
        onRequest(&req);

        {
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            OnLogPrint(forwardedState, asctime(tm), LogType_Info);
            OnLogPrint(forwardedState, req.request, LogType_Info);
        }
        close(req.connection);
    }

    free(buffer);
    close(socket_descriptor);

    return 0;
}
