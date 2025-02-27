#include "staticFile.h"
#include <string.h>
#include "tcp_server.h"
#include "ownedStr.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#include "md4c/src/md4c-html.h"

typedef struct ExtFileInfo {
    enum {File_Binary, File_Text} type;
    enum {File_Text_ScriptType_None, File_Text_ScriptType_Markdawn, File_Text_ScriptType_Lua} scriptType;
    const char* default_contentType; // Only aplicable for File_Binary and File_Text
} ExtFileInfo;

ExtFileInfo getExtFileInfo(const char* filename)
{
    char *ext = strrchr(filename, '.');
    ExtFileInfo def = {File_Text,File_Text_ScriptType_None,"Content-Type: text/plain\n"};
    if(!ext) return def;

    if (strcmp(ext, ".html") == 0) return (ExtFileInfo){File_Text,File_Text_ScriptType_None,"Content-Type: text/html;charset=utf-8\n"};
    if (strcmp(ext, ".css" ) == 0) return (ExtFileInfo){File_Text,File_Text_ScriptType_None,"Content-Type: text/css;charset=utf-8\n"};
    if (strcmp(ext, ".js"  ) == 0) return (ExtFileInfo){File_Text,File_Text_ScriptType_None,"Content-Type: application/javascript;charset=utf-8\n"};
    if (strcmp(ext, ".json") == 0) return (ExtFileInfo){File_Text,File_Text_ScriptType_None,"Content-Type: application/json;charset=utf-8\n"};
    if (strcmp(ext, ".txt" ) == 0) return (ExtFileInfo){File_Text,File_Text_ScriptType_None,"Content-Type: text/plain;charset=utf-8\n"};

    if (strcmp(ext, ".jpg" ) == 0) return (ExtFileInfo){File_Binary,File_Text_ScriptType_None,"Content-Type: image/jpg\n"};
    if (strcmp(ext, ".png" ) == 0) return (ExtFileInfo){File_Binary,File_Text_ScriptType_None,"Content-Type: image/png\n"};
    if (strcmp(ext, ".gif" ) == 0) return (ExtFileInfo){File_Binary,File_Text_ScriptType_None,"Content-Type: image/gif\n"};

    if (strcmp(ext, ".lua" ) == 0) return (ExtFileInfo){File_Text,File_Text_ScriptType_Lua,"Content-Type: text/html;charset=utf-8\n"};
    if (strcmp(ext, ".md"  ) == 0) return (ExtFileInfo){File_Text,File_Text_ScriptType_Markdawn,"Content-Type: text/html;charset=utf-8\n"};


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

void md_process_output(const MD_CHAR* str , MD_SIZE length, void* inState) {
    TCPServer_RequestState* s = (TCPServer_RequestState*)inState;
    TCPServer_sendStringWithLength(s,str,length);
}
void serveStaticFile(TCPServer_RequestState* s, const char* filename)
{
    const ExtFileInfo extFileInfo = getExtFileInfo(filename);
    if(extFileInfo.type == File_Text) {
        FILE* file = fopen(filename, "r");
        if (file) {
            if(extFileInfo.scriptType == File_Text_ScriptType_None) {
                sendDefaultOkHeader(s,extFileInfo.default_contentType);

                char buff[1024*10];
                while (fgets(buff, 1024*10, file) != NULL) {
                    TCPServer_sendString(s,buff);
                }

                fclose(file);
                printf("sent: %s \n",filename);
                return;
            }
            else {
                if(extFileInfo.scriptType == File_Text_ScriptType_Markdawn)
                {
                    sendDefaultOkHeader(s,extFileInfo.default_contentType);

                    OwnedStr buffer = OwnedStr_AllocFromFile(file);

                    md_html(buffer.str, buffer.capacity, md_process_output, s, MD_FLAG_TABLES|MD_FLAG_TASKLISTS|MD_FLAG_WIKILINKS,0);

                    OwnedStr_Free(&buffer);
                    fclose(file);
                    printf("sent: %s \n",filename);
                    return;
                }
            }
        }
    }

    if(extFileInfo.type == File_Binary) {
        FILE* file = fopen(filename, "rb");
        if (file) {
            sendDefaultOkHeader(s,extFileInfo.default_contentType);

            TCPServer_sendFile(s,file);
            fclose(file);
            printf("sent: %s \n",filename);
            return;
        }
    }

    sendDefault404Header(s);
}