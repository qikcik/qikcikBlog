// webserver.c
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 2048

char resp[] = "HTTP/1.0 200 OK\r\n"
"Server: webserver-c\r\n"
"Content-type: text/html\r\n\r\n"
"<html>hello, world</html>\r\n";

int main() {
    const int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_descriptor == -1) {
        perror("unable to open socket!");
        return 1;
    }

    struct sockaddr_in host_addr;
    int addrlen = sizeof(host_addr);
    host_addr.sin_family= AF_INET;
    host_addr.sin_port= htons(8080);
    host_addr.sin_addr.s_addr= htonl(INADDR_ANY);

    if(bind(socket_descriptor,(struct sockaddr*)&host_addr, addrlen) != 0) {
        perror("unable to bind address!");
        return 1;
    }

    if(listen(socket_descriptor,SOMAXCONN) != 0) {
        perror("unable to listen!");
        return 1;
    }

    const int running = 1;
    char buffer[BUFF_SIZE];
    while(running) {
        int conn = accept(socket_descriptor, (struct sockaddr*)&host_addr, (socklen_t *)&addrlen);

        struct sockaddr_in client_addr;
        int client_addrlen = sizeof(client_addr);
        int conn_name = getsockname(conn,(struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);
        if(conn_name < 0) {
            perror("unable to get connection name");
            continue;
        }

        if(conn < 0) {
            perror("unable to accept connection");
            continue;
        }
        int valread = read(conn,buffer,BUFF_SIZE);
        if(valread < 0) {
            perror("unable to read from connection");
            continue;
        }

        char method[BUFF_SIZE], uri[BUFF_SIZE], version[BUFF_SIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), method, version, uri);

        int valwrite = write(conn,resp,strlen(resp));
        if(valwrite < 0) {
            perror("unable to write to connection");
            continue;
        }

        close(conn);
    }

    close(socket_descriptor);
    return 0;
}