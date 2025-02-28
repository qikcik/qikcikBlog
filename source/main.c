// webserver.c

#include <string.h>

#include "tcp_server.h"
#include "ownedStr.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "staticFile.h"
#include "md4c.h"
#include "md4c-html.h"

#define FILE_PREFIX "../content/"

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

void md_process_output(const MD_CHAR* str , MD_SIZE length, void* inState) {
    OwnedStr* buffer = (OwnedStr*)inState;
    OwnedStr_ConcateWithSize(buffer,str,length);
}

int Lua_MdToHTML( lua_State* L)
{
    const char* arg = lua_tostring(L, 1);
    OwnedStr buffer = OwnedStr_Alloc("");
        md_html(arg, strlen(arg), md_process_output, &buffer, MD_FLAG_TABLES|MD_FLAG_TASKLISTS|MD_FLAG_WIKILINKS,0);
    lua_pushstring(L, buffer.str);
    OwnedStr_Free(&buffer);
    return 1;
}

int Lua_ServeStaticFile(lua_State* L)
{
    LuaUserContext* ctx = GetUserContext(L);
    const char* arg = lua_tostring(L, 1);

    OwnedStr path = OwnedStr_Alloc(FILE_PREFIX);
    OwnedStr_Concate(&path,arg);
    serveStaticFile(ctx->s,path.str);

    OwnedStr_Free(&path);
    return 0;
}

int Lua_Print(lua_State* L)
{
    LuaUserContext* ctx = GetUserContext(L);
    const char* arg = lua_tostring(L, 1);
    TCPServer_sendString(ctx->s,arg);
    return 0;
}

int Lua_sendDefaultHtmlOkHeader(lua_State* L)
{
    LuaUserContext* ctx = GetUserContext(L);
    sendDefaultOkHeader(ctx->s,"Content-Type: text/html;charset=utf-8\n");
    return 0;
}

int Lua_sendDefault404Header(lua_State* L)
{
    LuaUserContext* ctx = GetUserContext(L);
    sendDefault404Header(ctx->s);
    return 0;
}


int Lua_SetPackagePath( lua_State* L, const char* path )
{
    lua_getglobal( L, "package" );

    lua_pushstring( L, path );
    lua_setfield( L, -2, "path" );
    lua_pop( L, 1 );
    return 0;
}


void handleScriptError(void* s,const char* c_str,LogType_t type) {
    if(type == LogType_Error)
        printf("error: %s \n",c_str);
    else
        printf("info: %s \n",c_str);
}

void handleTcpRequest(TCPServer_RequestState* s) {
    char method[128], uri[2048], version[128];
    sscanf(TCPServer_GetRequestString(s), "%s %s %s", method, uri, version);

    FILE* file = fopen("../content/entry.lua", "r");
    OwnedStr buffer = OwnedStr_AllocFromFile(file);

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    // set LUA Context
    LuaUserContext context;
    context.s = s;
    LUA_INTEGER ptr = (LUA_INTEGER) (&context);
    lua_pushinteger(L,ptr);
    lua_setglobal(L,"_CONTEXT_PTR");
    Lua_SetPackagePath(L, "../content/?.lua");

    // register functions
    lua_register(L, "GetFileContent", Lua_GetFileContent);
    lua_register(L, "MdToHTML", Lua_MdToHTML);
    lua_register(L, "Print", Lua_Print);
    lua_register(L, "PrintDefaultHtmlOkHeader", Lua_sendDefaultHtmlOkHeader);
    lua_register(L, "Print", Lua_Print);
    lua_register(L, "ServeStaticFile", Lua_ServeStaticFile);


    // LoadScript
    int status = luaL_loadstring(L,buffer.str);
    if(status){
        sendDefault500Header(s);
        if(status == LUA_ERRSYNTAX){
            TCPServer_sendString(s,"<h1> SYNTAX ERROR: </h1>\n");
            TCPServer_sendString(s,lua_tostring(L, -1));
            handleScriptError(s,lua_tostring(L, -1),LogType_Error);
        }
        else{
            TCPServer_sendString(s,"<h1> UNKNOWN LOAD ERROR: </h1>\n");
        }

        goto cleanupAndReturn;
    }

    if(lua_pcall(L, 0, 0, 0)) {
        sendDefault500Header(s);
        TCPServer_sendString(s,"<h1> INIT ERROR: </h1>\n");
        TCPServer_sendString(s,lua_tostring(L, -1));
        handleScriptError(s,lua_tostring(L, -1),LogType_Error);

        goto cleanupAndReturn;
    }


    lua_getglobal(L, "HandleRequest");
    lua_pushstring(L,TCPServer_GetRequestString(s));

    if (lua_pcall(L, 1, 0, 0)) {
        sendDefault500Header(s);
        TCPServer_sendString(s,"<h1> RUNTIME ERROR: </h1>\n");
        TCPServer_sendString(s,lua_tostring(L, -1));
        handleScriptError(s,lua_tostring(L, -1),LogType_Error);

        goto cleanupAndReturn;
    }

    cleanupAndReturn:
        OwnedStr_Free(&buffer);
        fclose(file);
        lua_close(L);
        return;
}

void handleTcpError(void* s,const char* c_str,LogType_t type) {
    if(type == LogType_Error)
        printf("error: %s \n",c_str);
    else
        printf("info: %s \n",c_str);
}

int main() {
    return TCPServer_run(51800,handleTcpRequest,handleTcpError,0);
}