#pragma once
#include "ownedStr.h"


typedef OwnedStr (* HTTP_OnRequest_t)(void* forwardedState,const char* requestStr);
typedef void (* HTTP_OnError_t)(void* forwardedState,const char*);

int HTTP_run(int port,HTTP_OnRequest_t onRequest,HTTP_OnError_t onError, void* forwardedState);