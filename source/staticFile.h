#pragma once
#include "tcp_server.h"

void sendDefaultOkHeader(TCPServer_RequestState* s,const char* contentType);
void sendDefault404Header(TCPServer_RequestState* s);
void sendDefault500Header(TCPServer_RequestState* s);


void serveStaticFile(TCPServer_RequestState* s, const char* filename);
