#include "tcp_server.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <winsock2.h>
#include <ws2tcpip.h>

struct TCPServer_RequestState {
    SOCKET connection;
    const char* request;

    void* forwardedState;
    TCPServer_OnLogPrint_t onLogPrintCallback;
};

const char* TCPServer_GetRequestString(TCPServer_RequestState* s) {
    return s->request;
}

void TCPServer_sendString(TCPServer_RequestState* s, const char* c) {
    int write_status = send(s->connection, c, (int)strlen(c), 0);
    if(write_status == SOCKET_ERROR)
        s->onLogPrintCallback(s->forwardedState, "unable to write to connection", LogType_Error);
}

void TCPServer_sendStringWithLength(TCPServer_RequestState* s, const char* c, size_t l) {
    int write_status = send(s->connection, c, (int)l, 0);
    if(write_status == SOCKET_ERROR)
        s->onLogPrintCallback(s->forwardedState, "unable to write to connection", LogType_Error);
}

void TCPServer_sendFile(TCPServer_RequestState* s, FILE* f) {
    OwnedStr buffor = OwnedStr_AllocFromFile(f);

    int write_status = send(s->connection, buffor.str, (int)buffor.capacity, 0);
    if(write_status == SOCKET_ERROR)
        s->onLogPrintCallback(s->forwardedState, "unable to write to connection", LogType_Error);

    OwnedStr_Free(&buffor);
}

void* TCPServer_GetForwardedState(TCPServer_RequestState* s) {
    return s->forwardedState;
}

#define BUFF_SIZE (1024*10)

int TCPServer_run(int port, TCPServer_OnRequest_t onRequest, TCPServer_OnLogPrint_t OnLogPrint, void* forwardedState) {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        OnLogPrint(forwardedState, "WSAStartup failed!", LogType_Error);
        return -1;
    }

    const SOCKET socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socket_descriptor == INVALID_SOCKET) {
        OnLogPrint(forwardedState, "unable to open socket!", LogType_Error);
        WSACleanup();
        return -1;
    }

    struct sockaddr_in host_addr;
    int addrlen = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(port);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socket_descriptor, (struct sockaddr*)&host_addr, addrlen) == SOCKET_ERROR) {
        OnLogPrint(forwardedState, "unable to bind address!", LogType_Error);
        closesocket(socket_descriptor);
        WSACleanup();
        return -1;
    }

    if(listen(socket_descriptor, SOMAXCONN) == SOCKET_ERROR) {
        OnLogPrint(forwardedState, "unable to listen!", LogType_Error);
        closesocket(socket_descriptor);
        WSACleanup();
        return -1;
    }

    const int running = 1;
    char* buffer = (char*)malloc(BUFF_SIZE * sizeof(char));

    OnLogPrint(forwardedState, "start listening!", LogType_Info);

    while(running) {
        TCPServer_RequestState req;
        req.forwardedState = forwardedState;
        req.onLogPrintCallback = OnLogPrint;

        struct sockaddr_in client_addr;
        int client_addrlen = sizeof(client_addr);
        req.connection = accept(socket_descriptor, (struct sockaddr *)&client_addr, &client_addrlen);

        if (req.connection == INVALID_SOCKET) {
            OnLogPrint(forwardedState, "unable to accept connection", LogType_Error);
            continue;
        }

        int conn_name = getsockname(req.connection, (struct sockaddr *)&client_addr, &client_addrlen);
        if (conn_name == SOCKET_ERROR) {
            OnLogPrint(forwardedState, "unable to get connection name", LogType_Error);
            closesocket(req.connection);
            continue;
        }

        const int read_status = recv(req.connection, buffer, BUFF_SIZE - 1, 0);
        if (read_status <= 0) {
            OnLogPrint(forwardedState, "unable to read from connection (closed or error)", LogType_Error);
            closesocket(req.connection);
            continue;
        }

        buffer[read_status] = '\0';
        req.request = buffer;

        onRequest(&req);

        {
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            OnLogPrint(forwardedState, asctime(tm), LogType_Info);
            OnLogPrint(forwardedState, req.request, LogType_Info);
        }

        closesocket(req.connection);
    }

    free(buffer);
    closesocket(socket_descriptor);
    WSACleanup();

    return 0;
}