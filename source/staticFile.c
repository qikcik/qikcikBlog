#include "staticFile.h"
#include <string.h>
#include "tcp_server.h"
#include "ownedStr.h"

typedef struct ExtFileInfo {
    enum {File_Binary, File_Text} type;
    const char* default_contentType;
} ExtFileInfo;

ExtFileInfo getExtFileInfo(const char* filename)
{
    char *ext = strrchr(filename, '.');
    ExtFileInfo def = {File_Text,"Content-Type: text/plain\n"};
    if(!ext) return def;

    if (strcmp(ext, ".html") == 0) return (ExtFileInfo){File_Text,"Content-Type: text/html;charset=utf-8\n"};
    if (strcmp(ext, ".css" ) == 0) return (ExtFileInfo){File_Text,"Content-Type: text/css;charset=utf-8\n"};
    if (strcmp(ext, ".js"  ) == 0) return (ExtFileInfo){File_Text,"Content-Type: application/javascript;charset=utf-8\n"};
    if (strcmp(ext, ".json") == 0) return (ExtFileInfo){File_Text,"Content-Type: application/json;charset=utf-8\n"};
    if (strcmp(ext, ".txt" ) == 0) return (ExtFileInfo){File_Text,"Content-Type: text/plain;charset=utf-8\n"};

    if (strcmp(ext, ".jpg" ) == 0) return (ExtFileInfo){File_Binary,"Content-Type: image/jpg\n"};
    if (strcmp(ext, ".png" ) == 0) return (ExtFileInfo){File_Binary,"Content-Type: image/png\n"};
    if (strcmp(ext, ".gif" ) == 0) return (ExtFileInfo){File_Binary,"Content-Type: image/gif\n"};
    if (strcmp(ext, ".ttf" ) == 0) return (ExtFileInfo){File_Binary,"Content-Type: font/ttf\n"};

    return def;
}

void sendDefaultOkHeader(TCPServer_RequestState* s,const char* contentType) {
    TCPServer_sendString(s,"HTTP/1.1 200 OK\n");
    TCPServer_sendString(s,"Server: qws\n");
    TCPServer_sendString(s,contentType);
    TCPServer_sendString(s,"\n");
}

void sendDefault404Header(TCPServer_RequestState* s) {
    TCPServer_sendString(s,"HTTP/1.1 404 Not Found\n");
    TCPServer_sendString(s,"Server: qws\n");
    TCPServer_sendString(s,"Content-Type: text/plain\n");
    TCPServer_sendString(s,"\n");
    TCPServer_sendString(s,"Not Found");
}

void sendDefault500Header(TCPServer_RequestState* s) {
    TCPServer_sendString(s,"HTTP/1.1 500 Internal Error\n");
    TCPServer_sendString(s,"Server: qws\n");
    TCPServer_sendString(s,"Content-Type: text/plain\n");
    TCPServer_sendString(s,"\n");
    TCPServer_sendString(s,"Not Found");
}

void serveStaticFile(TCPServer_RequestState* s, const char* filename)
{
    const ExtFileInfo extFileInfo = getExtFileInfo(filename);
    if(extFileInfo.type == File_Text) {
        FILE* file = fopen(filename, "r");
        if (file) {
            sendDefaultOkHeader(s,extFileInfo.default_contentType);

            char buff[1024*10];
            while (fgets(buff, 1024*10, file) != NULL) {
                TCPServer_sendString(s,buff);
            }

            fclose(file);
            //printf("sent: %s \n",filename);
            return;
        }
    }

    if(extFileInfo.type == File_Binary) {
        FILE* file = fopen(filename, "rb");
        if (file) {
            sendDefaultOkHeader(s,extFileInfo.default_contentType);

            TCPServer_sendFile(s,file);
            fclose(file);
            //printf("sent: %s \n",filename);
            return;
        }
    }

    sendDefault404Header(s);
}