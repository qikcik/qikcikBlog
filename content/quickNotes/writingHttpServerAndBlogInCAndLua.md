# Writing HTTPserver and blog in C and LUA

First I wanted to say Hi, as it is my first post in my blog!\
After countless hours spent with my friend Qiu. I decided that this is finally the time to start writing my own blog.
Of course, I could use WordPress or any other "blog engine", but then... where would the funniest part be?

Without further thinking I found my old small project of simple single threaded http server - assuming that I can
~200 lines of code call http server. It needed a little bit of tweaking. Let me briefly describe some part of it:

## Implementation
code below was must have for me to keep my mental sanity, when using string operation in C (later I will delegate as much as possible of string operation to LUA):
```c
//file: OwnedStr.h
struct OwnedStr {
    char* str;
    size_t capacity;
};
typedef struct OwnedStr OwnedStr;

OwnedStr OwnedStr_Alloc(const char*);
void OwnedStr_Concate(OwnedStr*,const char*);
void OwnedStr_ConcateWithSize(OwnedStr* result, const char* in_str,size_t in_size);
void OwnedStr_Free(OwnedStr*);

OwnedStr OwnedStr_AllocFromFile(FILE *f);

//file: OwnedStr.cpp
#define JUST_IN_CASE 16

OwnedStr OwnedStr_Alloc(const char* in_c_str) {
    OwnedStr result;
    result.capacity = strlen(in_c_str);
    result.str = (char*)malloc(sizeof(char)*result.capacity+JUST_IN_CASE);
    
    strcpy(result.str,in_c_str);
    
    result.str[result.capacity] = '\0';
    return result;
}

void OwnedStr_Concate(OwnedStr* result, const char* in_str) {
    result->capacity = result->capacity+strlen(in_str);
    char* old_str = result->str;
    result->str = (char*)malloc(sizeof(char)*result->capacity+JUST_IN_CASE);
    
    strcpy(result->str,old_str);
    strcat(result->str,in_str);
    result->str[result->capacity] = '\0';
    free(old_str);
}
//...
```

Below there is main loop for handling requests. It is far from perfect, probably it not handle half of error, but for now what is the most important: it works

```c
//file: tcp_server.c
typedef void (*TCPServer_OnRequest_t)(TCPServer_RequestState* requestState);
#define BUFF_SIZE (1024*10)

int TCPServer_run(int port,TCPServer_OnRequest_t onRequest, TCPServer_OnLogPrint_t OnLogPrint, void* forwardedState) {
    signal(SIGPIPE, SIG_IGN); //resolved wierd bug in my WSL

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
        req.connection = accept(socket_descriptor, (struct sockaddr*)&host_addr, (socklen_t *)&addrlen);

        struct sockaddr_in client_addr;
        int client_addrlen = sizeof(client_addr);
        int conn_name = getsockname(req.connection,(struct sockaddr*)&client_addr, (socklen_t*)&client_addrlen);

        if(conn_name < 0)
            { OnLogPrint(forwardedState,"unable to get connection name",LogType_Error); close(req.connection); continue; }

        if(req.connection < 0)
            { OnLogPrint(forwardedState,"unable to accept connection",LogType_Error); close(req.connection); continue; }

        const int read_status = read(req.connection,buffer,BUFF_SIZE);
        if(read_status < 0)
            { OnLogPrint(forwardedState,"unable to read from connection",LogType_Error); close(req.connection); continue; }

        req.request = buffer;
        onRequest(&req);
        close(req.connection);
    }

    free(buffer);
    close(socket_descriptor);

    return 0;
}
```

To keep up appearances of any project aggregations boundary: All TCP related functions are encapsulated in simple header style interface
*(I use this pattern a lot in this project)*
```c
//file: tcp_server.h
typedef struct TCPServer_RequestState TCPServer_RequestState;

const char* TCPServer_GetRequestString(TCPServer_RequestState*);
void TCPServer_sendString(TCPServer_RequestState*,const char*);
void TCPServer_sendStringWithLength(TCPServer_RequestState*,const char*,size_t);
void TCPServer_sendFile(TCPServer_RequestState*,FILE*);
void* TCPServer_GetForwardedState(TCPServer_RequestState*);


typedef void (*TCPServer_OnRequest_t)(TCPServer_RequestState* requestState);

typedef enum LogType_t { LogType_Error, LogType_Info } LogType_t;
typedef void (* TCPServer_OnLogPrint_t)(void* forwardedState,const char*,LogType_t type);

int TCPServer_run(int port,TCPServer_OnRequest_t onRequest,TCPServer_OnLogPrint_t OnLogPrint, void* forwardedState);
```

For convince I wrote simple generic function to handle static file serving.
```c
//file: staticFile.h
void sendDefaultOkHeader(TCPServer_RequestState* s,const char* contentType);
void sendDefault404Header(TCPServer_RequestState* s);
void sendDefault500Header(TCPServer_RequestState* s);

void serveStaticFile(TCPServer_RequestState* s, const char* filename);

//file: staticFile.c
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
            printf("sent: %s \n",filename);
            return;
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
```

At that point this server was capable to server static hosted file, but I couldn't stop there:\
I wanted to support at least two feature that requires some form of scripting:
- handling user friendly URL
- website templates, to inject one html file into another
- thinking of future: some way to represents data structures like post

Also, I wanted to write main content in something else than HTML.\
Considering all of this: I decided to use LUA for scripting and Markdown *(using Md4c)* for content.

Please do not look at 2 dirty hack that I still don't  believe that I wrote them in 2025
- First of them is using goto to clean up heap allocated memory
- Second is to cast ptr to (long long) in order to pass and receive it from LuaState in API functions. I wanted to avoid using static *(global)* memory
```c
//file: main.c
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
```

As you can see I used LUA to generate both header and content.\
**Is it bad?** - yes\
**Is it work (for now)?** - yes

I exposed only: "GetFileContent", "MdToHTML", "ServeStaticFile", "Print", "sendDefaultHtmlOkHeader", "sendDefault404Header" to LUA\
Surprisingly, for website that you can see right now it is enough. and event not all of them are necessary. at least for now.
```c
//file: main.c
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
```

And finally bellow is **"entry.lua'**
```lua
require('common')

function HandleRequest(request)
    local request_exploder = string.gmatch(request, "%S+")
    local method = request_exploder()
    local location = request_exploder()

    --Routes
    if location == "/quick-notes" then
        PrintDefaultHtmlOkHeader()
        Print(GetFileContent("template/main.html") % { content = GetFileContent("template/quickNotes.html")})
    --Notes
    elseif location == "/quick-note/writing-http-server-and-blog-in-c-and-lua" then
        PrintDefaultHtmlOkHeader()
        Print(GetFileContent("template/main.html") % { content = MdToHTML(GetFileContent("quickNotes/writingHttpServerAndBlogInCAndLua.md"))})
    --Static
    elseif location == "/public/style.css" then
        ServeStaticFile("public/style.css");
    --Homepage
    else
        PrintDefaultHtmlOkHeader()
        Print(GetFileContent("template/main.html") % { content = GetFileContent("template/quickNotes.html")})
    end
end

--file: common.lua
function interp(s, tab)
    return (s:gsub('($%b{})', function(w) return tab[w:sub(3, -2)] or w end))
end

getmetatable("").__mod = interp
```

and simplified **"template/main.html"**
```html
<!doctype html>
<html lang="en">
<head>
    <title>Qikcik Blog</title>
</head>
<body>
    <h1>Qikcik</h1>
    ${content}
</body>
</html>
```

## Hosting issues

I wanted to host it on CT8 server, but there is a catch - it is running on freebsd.
First issue was missing **"struct sockaddr_in"** header. it was simply fixable by including ``<netinet/in.h>``
```c
source/tcp_server.c:54:24: error: variable has incomplete type 'struct sockaddr_in'
```

Second was trickier: SendFile is slightly different implemented i FreeBsd then in Linux.
After something, I decided that this is too small project to care about host os.
...so I rewrote **TCPServer_sendFile** to use additional buffer and only read/write. *(I keep telling to myself that at least I do not use garbage collector)*
```c
void TCPServer_sendFile(TCPServer_RequestState* s ,FILE* f) {
    OwnedStr buffor = OwnedStr_AllocFromFile(f);

    int write_status = write(s->connection,buffor.str,buffor.capacity);
    if(write_status < 0)
        s->onLogPrintCallback(s->forwardedState,"unable to write to connection",LogType_Error);

    OwnedStr_Free(&buffor);
}
```

## End thought
**Is all of this in any aspect commercial ready product?** - absolutely not\
**Is it safe, or Is it blazingly fast?** - absolutely not\
**Was it great journey?** - absolutely yes\
**Is it enough to serve simple blog?** - I hope so

PS:
There you can find all the [source code](https://github.com/qikcik/qikcikBlog)