// webserver.c

#include <stdlib.h>
#include <string.h>

#include "tcp_server.h"
#include "ownedStr.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#include "md4c/src/md4c-html.h"

#define NO_CACHE "Cache-Control: no-cache, no-store, must-revalidate\r\n" "Pragma: no-cache\r\n" "Expires: 0\r\n"


char resp_header[] = "HTTP/1.0 404 Not Found\r\n";

#define FILE_PREFIX "../content/"

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

void handleTcpRequest(TCPServer_RequestState* s) {
    char method[128], uri[2048], version[128];
    sscanf(TCPServer_GetRequestString(s), "%s %s %s", method, uri, version);

    const ExtFileInfo extFileInfo = getExtFileInfo(uri);
    char path[2048] = FILE_PREFIX; strcat(path,uri); // TODO: Possible unsafe

    if(extFileInfo.type == File_Text) {
        FILE* file = fopen(path, "r");
        if (file) {
            if(extFileInfo.scriptType == File_Text_ScriptType_None) {
                sendDefaultOkHeader(s,extFileInfo.default_contentType);

                char buff[1024*10];
                while (fgets(buff, 1024*10, file) != NULL) {
                    TCPServer_sendString(s,buff);
                }

                fclose(file);
                printf("sent: %s %s \n",method, path);
                return;
            }
            else {
                if(extFileInfo.scriptType == File_Text_ScriptType_Markdawn) {

                    sendDefaultOkHeader(s,extFileInfo.default_contentType);

                    fseek(file, 0L, SEEK_END);
                    long int count = ftell(file);


                    fseek (file, 0, SEEK_END);
                    size_t length = ftell (file);
                    fseek (file, 0, SEEK_SET);
                    char* buffer = malloc (length);
                    fread (buffer, 1, length, file);

                    md_html(buffer, count, md_process_output, s, MD_FLAG_TABLES|MD_FLAG_TASKLISTS|MD_FLAG_WIKILINKS,0);

                    free(buffer);
                    fclose(file);
                    printf("sent: %s %s \n",method, path);
                    return;
                }
            }
        }
    }

    if(extFileInfo.type == File_Binary) {
        FILE* file = fopen(path, "rb");
        if (file) {
            sendDefaultOkHeader(s,extFileInfo.default_contentType);

            TCPServer_sendFile(s,file);
            fclose(file);
            printf("sent: %s %s \n",method, path);
            return;
        }
    }

    //TODO
    sendDefault404Header(s);


    // OwnedStr filename = OwnedStr_Alloc(FILE_PREFIX);
    // OwnedStr_Concate(&filename,uri);
    // printf("%s", filename.str);
    // FILE *file;
    // if ((file = fopen(filename.str, "rb")))
    // {
    //     OwnedStr content = OwnedStr_AllocFromFile(file);
    //     fclose(file);
    //
    //     const char resp_header_1[] = "HTTP/1.0 200 OK\r\n" "Content-type: ";
    //     const char resp_header_2[] = "\r\n" NO_CACHE "\r\n";
    //
    //     OwnedStr response = OwnedStr_Alloc(resp_header_1);
    //     OwnedStr_Concate(&response,getContentType(filename.str));
    //     OwnedStr_Concate(&response,"\r\nContent-length: ");
    //
    //     const char* buff[256];
    //     sprintf(&buff,"%lu",(content.capacity*sizeof(char)));
    //     OwnedStr_Concate(&response,buff);
    //     OwnedStr_Concate(&response,resp_header_2);
    //
    //     OwnedStr_Concate(&response,content.str);
    //     OwnedStr_Free(&content);
    //     OwnedStr_Free(&filename);
    //
    //     return response;
    // }
    // OwnedStr_Free(&filename);


    //return OwnedStr_Alloc("HTTP/1.0 404 Not Found\r\n" "Content-type: text/html\r\n " NO_CACHE "\r\n" "Not Found");

    // lua_State *L = luaL_newstate();
    // luaL_openlibs(L);
    //
    //
    // int status = luaL_loadfile(L, "../content/main.lua");
    // if(status) {
    //     if(status == LUA_ERRSYNTAX) {
    //         OwnedStr_Concate(&resp,"<h1> SYNTAX ERROR: </h1>");
    //         OwnedStr_Concate(&resp,lua_tostring(L, -1));
    //     }
    //     else {
    //         OwnedStr_Concate(&resp,"<h1> UNKNOWN LOAD ERROR: </h1>");
    //     }
    //
    //     lua_close(L);
    //     return resp;
    // }
    // if(lua_pcall(L, 0, 0, 0)) {
    //     OwnedStr_Concate(&resp,"<h1> INIT ERROR: </h1>");
    //     OwnedStr_Concate(&resp,lua_tostring(L, -1));
    //     printf("%s",resp.str);
    //     lua_close(L);
    //     return resp;
    // }
    //
    //
    // lua_getglobal(L, "OnNewRequest");
    // lua_pushstring(L,requestStr);
    //
    // if (lua_pcall(L, 1, 1, 0)) {
    //     OwnedStr_Concate(&resp,"<h1> RUNTIME ERROR: </h1>");
    //     OwnedStr_Concate(&resp,lua_tostring(L, -1));
    //     lua_close(L);
    //     return resp;
    // }
    //
    // OwnedStr_Concate(&resp,lua_tostring(L,-1));
    //
    // lua_pop(L,1);
    // lua_close(L);
    // return resp;
}

void handleTcpError(void* s,const char* c_str,LogType_t type) {
    if(type == LogType_Error)
        printf("error: %s \n",c_str);
    else
        printf("info: %s \n",c_str);
}

int main() {
    return TCPServer_run(8088,handleTcpRequest,handleTcpError,0);
}