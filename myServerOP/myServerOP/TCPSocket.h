#pragma once
#include <iostream>
#include <string.h>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

using namespace std;

class TCPSocket
{
public:
	TCPSocket();
	TCPSocket(int fd);
	int connectToHost(string ip, unsigned short port, int waitSeconds = 60);
	void disConnected();
	int sendMsg(string sendData, int waitSeconds = 60);
	string recvMsg(int waitSeconds = 60);

	~TCPSocket();
private:
	int connectHostTimeout(int waitSeconds);
	int setNonBlock(int fd);
	int setBlock(int fd);
	int writeTimeOut(int waitSeconds);
	int writen(const void* buf, int count);
	int readTimeOut(int waitSeconds);
	int readn(const void* buf, int count);
	
private:
	int m_connfd;
	struct sockaddr_in m_serv_addr;
	char m_buf[BUFSIZ];
};

