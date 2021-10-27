#pragma once
#include "TCPSocket.h"
// 超时的时间

class TCPServer
{
public:
	TCPServer();
	~TCPServer();

	// 服务器设置监听
	int setListen(unsigned short port);
	// 等待并接受客户端连接请求, 默认连接超时时间为60s
	TCPSocket* acceptConn(int waitSeconds = 60);
	void closefd();

private:
	int m_lfd;	// 用于监听的文件描述符
};

