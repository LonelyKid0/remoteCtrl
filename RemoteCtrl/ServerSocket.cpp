#include "pch.h"
#include "ServerSocket.h"

//CServerSocket server;
CServerSocket* CServerSocket::instance = NULL;
CServerSocket::Helper CServerSocket::helper;
CServerSocket* pserver = CServerSocket::getInstance();