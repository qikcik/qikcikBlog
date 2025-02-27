// webserver.c

#include <stdlib.h>
#include <string.h>

#include "tcp_server.h"
#include "ownedStr.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#include "md4c/src/md4c-html.h"
#include "staticFile.h"

#define NO_CACHE "Cache-Control: no-cache, no-store, must-revalidate\r\n" "Pragma: no-cache\r\n" "Expires: 0\r\n"
#define FILE_PREFIX "../content/"

int Lua_GetFileContent(lua_State* L)
{
    const char* arg = lua_tostring(L, 1);

    OwnedStr path = OwnedStr_Alloc(FILE_PREFIX);
    OwnedStr_Concate(&path,arg);

    FILE* file = fopen(path.str, "r");
    OwnedStr_Free(&path);

    if(file)
    {
        OwnedStr buffer = OwnedStr_AllocFromFile(file);

        lua_pushstring(L, buffer.str);

        OwnedStr_Free(&buffer);
        fclose(file);
        return 1;
    }


    lua_pushstring(L, "");
    return 1;
}

typedef struct LuaUserContext
{
    TCPServer_RequestState* s;
} LuaUserContext;

LuaUserContext* GetUserContext(lua_State* L)
{
    lua_getglobal(L,"_CONTEXT_PTR");
    LUA_INTEGER ptr = lua_tointeger(L, -1);
    lua_pop(L,1);

    LuaUserContext* result = (LuaUserContext*)ptr;


    return result;
}

int Lua_Print(lua_State* L)
{
    LuaUserContext* ctx = GetUserContext(L);

    const char* arg = lua_tostring(L, 1);

    TCPServer_sendString(ctx->s,arg);

    return 0;
}

char resp_header[] = "HTTP/1.0 404 Not Found\r\n";


void handleTcpRequest(TCPServer_RequestState* s) {
    char method[128], uri[2048], version[128];
    sscanf(TCPServer_GetRequestString(s), "%s %s %s", method, uri, version);

    OwnedStr path = OwnedStr_Alloc(FILE_PREFIX);
    OwnedStr_Concate(&path,uri);

    serveStaticFile(s,path.str);

    OwnedStr_Free(&path);


    /*if(extFileInfo.type == File_Text) {
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
                else if(extFileInfo.scriptType == File_Text_ScriptType_Lua) {
                    fseek(file, 0L, SEEK_END);
                    long int count = ftell(file);


                    fseek (file, 0, SEEK_END);
                    size_t length = ftell (file);
                    fseek (file, 0, SEEK_SET);
                    char* buffer = malloc (length);
                    fread (buffer, 1, length, file);


                    lua_State *L = luaL_newstate();
                    luaL_openlibs(L);

                    LuaUserContext context;
                    context.s = s;

                    LUA_INTEGER ptr = (LUA_INTEGER)(&context);

                    lua_pushinteger(L,ptr);
                    lua_setglobal(L,"_CONTEXT_PTR");

                    lua_register(L, "GetFileContent", Lua_GetFileContent);
                    lua_register(L, "Print", Lua_Print);


                    int status = luaL_loadstring(L,buffer);
                    if(status){
                        sendDefault500Header(s);
                        if(status == LUA_ERRSYNTAX){
                            TCPServer_sendString(s,"<h1> SYNTAX ERROR: </h1>\n");
                            TCPServer_sendString(s,lua_tostring(L, -1));
                        }
                        else{
                            TCPServer_sendString(s,"<h1> UNKNOWN LOAD ERROR: </h1>\n");
                        }

                        free(buffer);
                        fclose(file);
                        lua_close(L);
                        return;
                    }

                    if(lua_pcall(L, 0, 0, 0)) {
                        sendDefault500Header(s);
                        TCPServer_sendString(s,"<h1> INIT ERROR: </h1>\n");
                        TCPServer_sendString(s,lua_tostring(L, -1));

                        free(buffer);
                        fclose(file);
                        lua_close(L);
                        return;
                    }


                    lua_getglobal(L, "OnNewRequest");
                    lua_pushstring(L,TCPServer_GetRequestString(s));

                    if (lua_pcall(L, 1, 1, 0)) {
                        sendDefault500Header(s);
                        TCPServer_sendString(s,"<h1> RUNTIME ERROR: </h1>\n");
                        TCPServer_sendString(s,lua_tostring(L, -1));

                        free(buffer);
                        fclose(file);
                        lua_close(L);
                        return;
                    }

                    sendDefaultOkHeader(s,extFileInfo.default_contentType);
                    TCPServer_sendString(s,lua_tostring(L,-1));
                    lua_pop(L,1);

                    free(buffer);
                    fclose(file);
                    lua_close(L);
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
     */
}

void handleTcpError(void* s,const char* c_str,LogType_t type) {
    if(type == LogType_Error)
        printf("error: %s \n",c_str);
    else
        printf("info: %s \n",c_str);
}

int main() {
    return TCPServer_run(8081,handleTcpRequest,handleTcpError,0);
}