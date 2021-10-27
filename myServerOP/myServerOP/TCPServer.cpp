#include "TCPServer.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

TCPServer::TCPServer()
{
}

TCPServer::~TCPServer()
{
}

int TCPServer::setListen(unsigned short port)
{
	int 	ret = 0;
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// �����������׽���
	m_lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_lfd == -1)
	{
		ret = errno;
		return ret;
	}

	int on = 1;
	// ���ö˿ڸ���
	ret = setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}

	// �������׽��ְ󶨱���IP�Ͷ˿�
	socklen_t serv_addr_len = sizeof(serv_addr);
	ret = bind(m_lfd, (struct sockaddr*)&serv_addr, serv_addr_len);
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}

	ret = listen(m_lfd, 128);
	if (ret == -1)
	{
		ret = errno;
		return ret;
	}

	return ret;
}

TCPSocket* TCPServer::acceptConn(int wait_seconds)
{
	int ret;
	if (wait_seconds > 0)
	{
		fd_set accept_fdset;
		struct timeval timeout;
		FD_ZERO(&accept_fdset);
		FD_SET(m_lfd, &accept_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			// ��������
			ret = select(m_lfd + 1, &accept_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);	// ���ź��ж�, �ٴν���ѭ��
		if (ret <= 0)
		{
			return NULL;
		}
	}

	// һ������ ��select�¼���������ʾ�Եȷ�������������֣��ͻ����������ӽ���
	// ��ʱ�ٵ���accept���������
	struct sockaddr_in clit_addr;
	socklen_t clit_addr_len = sizeof(struct sockaddr_in);
	int cfd = accept(m_lfd, (struct sockaddr*)&clit_addr, &clit_addr_len); //�����������׽���
	if (cfd == -1)
	{
		return NULL;
	}

	return new TCPSocket(cfd);
}

void TCPServer::closefd()
{
	close(m_lfd);
}