#pragma once
#include "pch.h"
#include "framework.h"
class CServerSocket
{
public:
	static CServerSocket* getInstance() {
		if (instance == NULL) {
			instance = new CServerSocket();
		}
		return instance;
	}
	bool InitSocket() {
		server_socket = socket(PF_INET, SOCK_STREAM, 0);
		if (server_socket == -1) return false;
		sockaddr_in server_addr;
		server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(9527);

		if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
			return false;
		if (listen(server_socket, 1) == -1) return false;

		return true;
	}

	bool AceeptClient() {
		sockaddr_in client_addr;
		int client_sz = sizeof(client_addr);
		client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_sz);
		if (client_socket == -1) return false;
	}

	int DealCommand() {
		if (client_socket == -1) return -1;
		char buffer[1024] = "";
		while (true)
		{
			int ret = recv(server_socket, buffer, sizeof(buffer), 0);
			if (ret < 0) return -1;
			//TODO:处理命令
		}
	}

	bool Send(const char* pData, int nSize) {
		if (client_socket == -1) return false;
		return send(server_socket, pData, sizeof(nSize), 0) > 0;
	}
private:
	SOCKET server_socket;
	SOCKET client_socket;

	CServerSocket(const CServerSocket& socket) {
		server_socket = socket.server_socket;
		client_socket = socket.client_socket;
	}
	CServerSocket& operator=(const CServerSocket& socket) {}

	CServerSocket() {
		server_socket = INVALID_SOCKET;
		client_socket = INVALID_SOCKET;
		if (InitSocketEnv() == FALSE) {
			MessageBox(NULL, _T("无法初始化套接字，请检查网络"), _T("初始化错误！"), MB_OK | MB_ICONERROR);
			exit(0);
		}
	}
	~CServerSocket() {
		WSACleanup();
		closesocket(server_socket);
	}
	BOOL InitSocketEnv() {
		WSADATA data;
		if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
			return FALSE;
		}
		return TRUE;
	}
	static CServerSocket* instance;

	class Helper {
	public:
		Helper() {}
		~Helper() {
			if (instance != NULL) {
				delete instance;
				instance = NULL;
			}
		}
	};
	static Helper helper;
};

//extern CServerSocket server;
