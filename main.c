// webserver.c

#include "http_server.h"
#include "ownedStr.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"


char resp_header[] = "HTTP/1.0 200 OK\r\n"
"Server: webserver-c\r\n"
"Content-type: text/html\r\n\r\n";



OwnedStr handleHttpRequest(void* s,const char* requestStr) {
    OwnedStr resp = OwnedStr_Alloc(resp_header);



    char method[1024], uri[1024], version[1024]; //TODO: Refactor
    sscanf(requestStr, "%s %s %s", method, uri, version);

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);


    int status = luaL_loadfile(L, "../data/main.lua");
    if(status) {
        if(status == LUA_ERRSYNTAX) {
            OwnedStr_Concate(&resp,"<h1> SYNTAX ERROR: </h1>");
            OwnedStr_Concate(&resp,lua_tostring(L, -1));
        }
        else {
            OwnedStr_Concate(&resp,"<h1> UNKNOWN LOAD ERROR: </h1>");
        }

        lua_close(L);
        return resp;
    }
    if(lua_pcall(L, 0, 0, 0)) {
        OwnedStr_Concate(&resp,"<h1> INIT ERROR: </h1>");
        OwnedStr_Concate(&resp,lua_tostring(L, -1));
        printf("%s",resp.str);
        lua_close(L);
        return resp;
    }


    lua_getglobal(L, "OnNewRequest");
    lua_pushstring(L,requestStr);

    if (lua_pcall(L, 1, 1, 0)) {
        OwnedStr_Concate(&resp,"<h1> RUNTIME ERROR: </h1>");
        OwnedStr_Concate(&resp,lua_tostring(L, -1));
        lua_close(L);
        return resp;
    }

    OwnedStr_Concate(&resp,lua_tostring(L,-1));

    lua_pop(L,1);
    lua_close(L);
    return resp;
}

void handleHttpError(void* s,const char* c_str) {
    fprintf(stderr, "HTTP_ERROR: %s",c_str);
}

int main() {
    return HTTP_run(8088,handleHttpRequest,handleHttpError,0);
}