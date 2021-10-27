#include "TCPSocket.h"

TCPSocket::TCPSocket() {
	m_connfd = socket(AF_INET, SOCK_STREAM, 0);
}
TCPSocket::TCPSocket(int fd) {
	m_connfd = fd;
}

int TCPSocket::connectToHost(string ip, unsigned short port, int waitSeconds) {
	memset(&m_serv_addr, 0, sizeof(m_serv_addr));
	m_serv_addr.sin_family = AF_INET;
	m_serv_addr.sin_port = htons(port);
	inet_pton(AF_INET, ip.data(), &m_serv_addr.sin_addr);
	// m_serv_addr.sin_addr.s_addr = inet_addr(ip.data());
	int ret = connectHostTimeout(waitSeconds);
	// errno ��һ�����̹������
	if (ret < 0) {
		if (ret == -1 && errno == ETIMEDOUT) {
			// dosomething
		}
		else {
			printf("connectTimeout �����쳣, �����: %d\n", errno);
			return errno;
		}
	}
	return ret;
}

void TCPSocket::disConnected() {
	if (m_connfd > 0) {
		close(m_connfd);
	}
}

// ���ص��Ƿ����˵����� ���� û��������أ� ��Ҫʹ���߽�һ���ж���
int TCPSocket::sendMsg(string sendData, int waitSeconds) {
	// ��Ҫ���͵�����д������д������
	// ����д������, һֱ����
	// ���write������Ӧ��fd(ͨ�ŵ��ļ�������)��д�������Ϳ�����
	// send(m_connfd, m_buf, sizeof(m_buf), 0);  
	// ����0->û��ʱ, ����-1->��ʱ
	int ret = writeTimeOut(waitSeconds);
	if (ret == 0) {
		// δ��ʱ
		// ��ӵ�4�ֽ���Ϊ����ͷ, �洢���ݿ鳤�� --> ��ֹճ����
		int writenLen = 0, dataLen = sendData.size() + 4;
		/* ����� malloc �Ǳ� new ��ʲô�ŵ��� 
		������������һ��Ҫת�� unsigned char* �� ��Ϊʲô writen �����õ� char*
		*/
		cout << sendData.size() << "  das fsa   " << dataLen << endl;

		unsigned char* netdata = (unsigned char*)malloc(dataLen);
		if (netdata == NULL) {
			
		}
		// ��ΪҪ������ netdata �������紫��
		int netLen = htonl(sendData.size());	// uint32_t
		// uint32_t netLen = htonl((uint32_t)sendData.size());	// uint32_t

		cout << netLen << endl;

		memcpy(netdata, &netLen, 4);	// ����ͷ��
		memcpy(netdata + 4, sendData.data(), sendData.size());

		// û���ⷵ�ط��͵�ʵ���ֽ���, Ӧ�� == �ڶ�������: dataLen
		// ʧ�ܷ���: -1
		writenLen = writen(netdata, dataLen);
		if (writenLen < dataLen) {
			// ����ʧ��
			if (netdata != NULL) {
				free(netdata);
				netdata = NULL;
			}
			return writenLen;
		}

		if (netdata != NULL) {
			free(netdata);
			netdata = NULL;
		}
		// ����� return ret;
	}
	else {
		//ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
		if (ret == -1 && errno == ETIMEDOUT)
		{
			/*ret = TimeoutError;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);*/
		}
	}
	// ���԰��������ֵ�е���
	return ret;
}

int TCPSocket::writeTimeOut(int waitSeconds) {
	int ret = 0;
	if (waitSeconds > 0) {
		struct timeval timeout;
		timeout.tv_sec = waitSeconds;
		timeout.tv_usec = 0;
		fd_set write_fdset;
		FD_ZERO(&write_fdset);
		FD_SET(m_connfd, &write_fdset);
		do {
			ret = select(m_connfd + 1, NULL, &write_fdset, NULL, &timeout);
		} while (ret == -1 && errno == EINTR);		// ����û������ EAGAIN

		if (ret == 0) {
			// ��ʱ����
			ret = -1;
			errno = ETIMEDOUT;	// ���ڲ���ϵͳ��������������Ҫ��δָ�� errno �� 
		}
		else if (ret == 1) {
			ret = 0;
		}
	}
	// ���Ҳ�и�����ɣ� ������÷������� waitSeconds Ϊ0, �����ǲ��ǿ�д������ 0 ��
	return ret;
}

int TCPSocket::readTimeOut(int waitSeconds) {
	int ret = -1;
	if (waitSeconds > 0) {
		struct timeval timeout;
		timeout.tv_sec = waitSeconds;
		timeout.tv_usec = 0;
		fd_set read_fdset;
		FD_ZERO(&read_fdset);
		FD_SET(m_connfd, &read_fdset);
		do {
			ret = select(m_connfd + 1, &read_fdset, NULL, NULL, &timeout);
		} while (ret == -1 && errno == EINTR);
		if (ret == 0) {
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1) {
			ret = 0;
		}
	}
	return ret;
}

/*
* writen - ���͹̶��ֽ���
* @buf: ���ͻ�����
* @count: Ҫ��ȡ���ֽ���
* �ɹ�����count��ʧ�ܷ���-1
*/
int TCPSocket::writen(const void* buf, int count) {
	size_t nleft = count;
	ssize_t nwritten;	// �з��� size_t
	char* data = (char*)buf;
	while (nleft > 0) {
		if ((nwritten = write(m_connfd, buf, nleft)) < 0) {
			if (errno == EINTR) {
				// ���źŴ��
				continue;
			}
			return -1;
		}
		else if (nwritten == 0) {
			// ����Ƕ���İ�
			// ������� 0 �� read ����һ����
			continue;
		}

		data += nwritten;
		nleft -= nwritten;
	}
	return count;
}

// �� buf ��ʼ�� count ���ֽڣ� ���ض��˼����ֽ�
int TCPSocket::readn(const void* buf, int count) {
	size_t nleft = count;
	ssize_t nread = 0;
	char* data = (char*)buf;
	while (nleft > 0) {
		if ((nread = read(m_connfd, data, nleft)) < 0) {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}
		else if (nread == 0) {
			// Ϊʲô�� writen �Ĵ���һ��
			return count - nleft;
		}
		cout << "nread = " << nread << endl;
		data += nread;
		nleft -= nread;
	}
	return count;
}


string TCPSocket::recvMsg(int waitSeconds) {
	int ret;
	// ����0 -> û��ʱ�ͽ��յ�������, -1, ��ʱ�����쳣
	cout << "ready to readTimeOut" << endl;
	ret = readTimeOut(waitSeconds);
	cout << "readTimeOut returns " << ret << endl;
	if (ret != 0) {
		if (ret == -1 || errno == ETIMEDOUT) {
			printf("readTimeout(timeout) err: TimeoutError \n");
			return string();
		}
		else {
			printf("readTimeout(timeout) err: %d \n", ret);
			return string();
		}
	}
	int netDataLen = 0;
	ret = readn(&netDataLen, 4); //����ͷ 4���ֽ� readn �ķ���ֵ�Ƕ��˼����ֽ�
	
	cout << "readn head returns " << ret << endl;

	if (ret == -1) {
		// errno while readn;
		return string();
	}
	else if (ret < 4) {
		// errno while readn;
		return string();
	}
	// int dataLen = ntohl(ret);   --> BBBBBBBBBUUUUUUUUUUUUUUUGGGGGGGGGGGGG
	int dataLen = ntohl(netDataLen);
	cout << "ret == " << ret << " dataLen == " << dataLen << endl;
	char* dataBuf = (char*)malloc(dataLen + 1);
	if (dataBuf == NULL) {
		// errno while malloc
		cout << "databuf malloc error " << endl;
		return string();
	}

	ret = readn(dataBuf, dataLen);

	cout << "readn data returns " << ret << endl;

	if (ret == -1) {
		// errno while readn;
		return string();
	}
	else if (ret < dataLen) {
		// ֪��Ϊɶ��һ�������˰�
		printf("func readn() err peer closed:%d \n", ret);
		return string();
	}
	dataBuf[dataLen] = '\0';	//�����һ���ֽ����ݣ����ݿɼ��ַ��� �ַ�������ʵ������ȻΪn
	string dataRecv = string(dataBuf);
	free(dataBuf);

	return dataRecv;
}

int TCPSocket::connectHostTimeout(int waitSeconds) {
	int ret;
	setNonBlock(m_connfd);
	socklen_t serv_addr_len = sizeof(m_serv_addr);
	if (waitSeconds > 0) {
		setNonBlock(m_connfd);
	}

	ret = connect(m_connfd, (struct sockaddr*)&m_serv_addr, serv_addr_len);
	/*  EINPROGRESS
              The socket is nonblocking and the connection cannot be completed immediately.  (UNIX domain sockets failed with EA�\
              GAIN  instead.)   It is possible to select(2) or poll(2) for completion by selecting the socket for writing.  After
              select(2) indicates writability, use getsockopt(2) to read the SO_ERROR option at  level  SOL_SOCKET  to  determine
              whether  connect()  completed successfully (SO_ERROR is zero) or unsuccessfully (SO_ERROR is one of the usual error
              codes listed here, explaining the reason for the failure).*/
	// ������ģʽ����, ����-1, ����errnoΪEINPROGRESS, ��ʾ�������ڽ�����
	if (ret < 0 && errno == EINPROGRESS) {
		fd_set conncet_fdset;
		FD_ZERO(&conncet_fdset);
		FD_SET(m_connfd, &conncet_fdset);

		struct timeval timeout;
		timeout.tv_sec = waitSeconds;
		timeout.tv_usec = 0;
		// ��������ʱ�� connect �ɹ����أ� ���ͻᷢ��������ô

		do {
			ret = select(m_connfd + 1, NULL, &conncet_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);	// ���ź��ж�

		// ret ���ص��Ǽ����� fd ״̬�����仯�ĸ���
		if (ret == 0) {
			// ��ʱ
			ret = -1;
			// errno 
			errno = ETIMEDOUT;
		}
		else if (ret < 0) {
			return -1;
		}
		else if (ret == 1) {
			/* ret����Ϊ1����ʾ�׽��ֿ�д�������������������һ�������ӽ����ɹ���һ�����׽��ֲ�������*/
			/* ��ʱ������Ϣ���ᱣ����errno�����У���ˣ���Ҫ����getsockopt����ȡ�� */
			// --> ���ӽ���`ʧ��ʱ`��socket �ļ�������`�ȿɶ��ֿ�д`�� ��������δ���Ĵ��󣬴Ӷ��ɶ��ֿ�д��
			int err;
			socklen_t errLen = sizeof(err);
			// getsockopt �Ǹ�ɶ������
			int sockoptret = getsockopt(m_connfd, SOL_SOCKET, SO_ERROR, &err, &errLen);
			if (sockoptret == -1) {
				return -1;
			}
			else if (err == 0) {
				ret = 0;	// �ɹ���������
			}
			else {
				// ����ʧ��
				errno = err;
				ret = -1;
			}
		}
	}
	if (waitSeconds > 0) {
		setBlock(m_connfd);	// �׽������û�����ģʽ --> ��ԭ
	} 
	return ret;

}

int TCPSocket::setNonBlock(int fd) {
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1) {
		return -1;
	}
	flags |= O_NONBLOCK;
	int ret = fcntl(fd, F_SETFL, flags);
	return ret;
}

int TCPSocket::setBlock(int fd) {
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1) {
		return -1;
	}
	flags &= ~O_NONBLOCK;
	int ret = fcntl(fd, F_SETFL, flags);
	return ret;
}

TCPSocket::~TCPSocket() {
	
}
