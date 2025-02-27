#pragma once
#include "tcp_server.h"

void serveStaticFile(TCPServer_RequestState* s, const char* filename);
