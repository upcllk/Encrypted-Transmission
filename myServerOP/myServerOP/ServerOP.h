#pragma once
#include <iostream>
#include <map>
#include "TCPSocket.h"
#include "TCPServer.h"
#include "msg.pb.h"

class ServerOP
{
public:
	ServerOP(std::string json);
	// ����������
	void startServer();
	~ServerOP();

private:
	static void* work(void* args);
	string secKeyAgree(RequestMsg* reqMsg);
	void secKeyCheck();
	void secKeyCancel();

private:
	enum AESKeyLen {
		LEN16 = 16,
		LEN24 = 24,
		LEN32 = 32
	};
	string getRandKey(AESKeyLen len);

 private:
	unsigned short m_port = 9898;
	string m_serverID;
	// ����洢ͨ�ŵ� socket ����
	map<pthread_t, TCPSocket*>m_list;
	// ����Ǽ����� socket ����
	TCPServer* m_server = NULL;
};

