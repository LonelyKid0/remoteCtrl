#pragma once
#include "pch.h"
#include "framework.h"

class CPacket {
public:
	CPacket():sHead(0),nLength(0),sCmd(0),sSum(0){}
	CPacket(const CPacket& packet) {
		sHead = packet.sHead;
		nLength = packet.nLength;
		sCmd = packet.sCmd;
		strData = packet.strData;
		sSum = packet.sSum;
	}
	CPacket& operator= (const CPacket& packet){
		if (this != &packet) {
			sHead = packet.sHead;
			nLength = packet.nLength;
			sCmd = packet.sCmd;
			strData = packet.strData;
			sSum = packet.sSum;
		}
		return *this;
	}
	CPacket(const BYTE* pData,size_t& nSize) {
		size_t i = 0;
		for (; i < nSize; i++)
		{
			if (*(WORD*)(pData + i) == 0xFEFF) {
				sHead = *(WORD*)(pData + i);
				i += 2;
				break;//找到包头
			}
		}
		if (i + 4 + 2 + 2 >= nSize) {
			nSize = 0;
			return;
		}

		nLength = *(DWORD*)(pData + i); i += 4;
		if (nLength + i > nSize) { //包只读到了一半
			nSize = 0;
			return;
		}
		sCmd = *(WORD*)(pData + i); i += 2;
		if (nLength>4) {
			strData.resize(nLength-4);
			memcpy((void*)strData.c_str(),pData+i,nLength-4);
			i += nLength - 4;
		}
		sSum = *(WORD*)(pData + i); i += 2;
		WORD sum = 0;
		for (size_t j = 0; j < strData.size(); j++)
		{
			sum += BYTE(strData[j]) & 0xFF;
		}
		if (sum == sSum) {
			nSize = i;
			return;
		}
		nSize = 0;
	}
	~CPacket(){}
	WORD sHead; //FE FF
	DWORD nLength;//包长度，从命令开始，到和校验结束
	WORD sCmd;
	std::string strData;
	WORD sSum;
	
};

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
		m_server_socket = socket(PF_INET, SOCK_STREAM, 0);
		if (m_server_socket == -1) return false;
		sockaddr_in server_addr;
		server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(9527);

		if (bind(m_server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
			return false;
		if (listen(m_server_socket, 1) == -1) return false;

		return true;
	}

	bool AceeptClient() {
		sockaddr_in client_addr;
		int client_sz = sizeof(client_addr);
		m_client_socket = accept(m_server_socket, (sockaddr*)&client_addr, &client_sz);
		if (m_client_socket == -1) return false;
	}
#define BUFFER_SIZE 4096
	int DealCommand() {
		if (m_client_socket == -1) return -1;
		char* buffer = new char[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		size_t index = 0;
		while (true)
		{
			size_t len = recv(m_server_socket, buffer+index, BUFFER_SIZE -index, 0);
			if (len < 0) return -1;
			//TODO:处理命令
			index += len;
			len = index;
			m_packet = CPacket((BYTE*)buffer,len);
			if (len > 0) {
				memmove(buffer,buffer+len, BUFFER_SIZE -len);
				index -= len;
				return m_packet.sCmd;
			}
		}
		return -1;
	}

	bool Send(const char* pData, int nSize) {
		if (m_client_socket == -1) return false;
		return send(m_server_socket, pData, sizeof(nSize), 0) > 0;
	}
private:
	SOCKET m_server_socket;
	SOCKET m_client_socket;
	CPacket m_packet;

	CServerSocket(const CServerSocket& socket) {
		m_server_socket = socket.m_server_socket;
		m_client_socket = socket.m_client_socket;
	}
	CServerSocket& operator=(const CServerSocket& socket) {}

	CServerSocket() {
		m_server_socket = INVALID_SOCKET;
		m_client_socket = INVALID_SOCKET;
		if (InitSocketEnv() == FALSE) {
			MessageBox(NULL, _T("无法初始化套接字，请检查网络"), _T("初始化错误！"), MB_OK | MB_ICONERROR);
			exit(0);
		}
	}
	~CServerSocket() {
		WSACleanup();
		closesocket(m_server_socket);
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
