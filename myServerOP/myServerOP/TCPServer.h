#pragma once
#include "TCPSocket.h"
// ��ʱ��ʱ��

class TCPServer
{
public:
	TCPServer();
	~TCPServer();

	// ���������ü���
	int setListen(unsigned short port);
	// �ȴ������ܿͻ�����������, Ĭ�����ӳ�ʱʱ��Ϊ60s
	TCPSocket* acceptConn(int waitSeconds = 60);
	void closefd();

private:
	int m_lfd;	// ���ڼ������ļ�������
};

