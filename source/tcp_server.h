#pragma once
#include "ownedStr.h"

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
