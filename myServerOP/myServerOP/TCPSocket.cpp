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
	// errno 是一个进程共享的吗
	if (ret < 0) {
		if (ret == -1 && errno == ETIMEDOUT) {
			// dosomething
		}
		else {
			printf("connectTimeout 调用异常, 错误号: %d\n", errno);
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

// 返回的是发送了的数据 。。 没发送完成呢， 需要使用者进一步判断吗
int TCPSocket::sendMsg(string sendData, int waitSeconds) {
	// 将要发送的数据写到本地写缓冲区
	// 本地写缓冲区, 一直阻塞
	// 检测write函数对应的fd(通信的文件描述符)的写缓冲区就可以了
	// send(m_connfd, m_buf, sizeof(m_buf), 0);  
	// 返回0->没超时, 返回-1->超时
	int ret = writeTimeOut(waitSeconds);
	if (ret == 0) {
		// 未超时
		// 添加的4字节作为数据头, 存储数据块长度 --> 防止粘包吗
		int writenLen = 0, dataLen = sendData.size() + 4;
		/* 这边用 malloc 是比 new 有什么优点吗， 
		还有网络数据一定要转成 unsigned char* ？ 那为什么 writen 里面用的 char*
		*/
		cout << sendData.size() << "  das fsa   " << dataLen << endl;

		unsigned char* netdata = (unsigned char*)malloc(dataLen);
		if (netdata == NULL) {
			
		}
		// 因为要拷贝到 netdata 用于网络传输
		int netLen = htonl(sendData.size());	// uint32_t
		// uint32_t netLen = htonl((uint32_t)sendData.size());	// uint32_t

		cout << netLen << endl;

		memcpy(netdata, &netLen, 4);	// 拷贝头部
		memcpy(netdata + 4, sendData.data(), sendData.size());

		// 没问题返回发送的实际字节数, 应该 == 第二个参数: dataLen
		// 失败返回: -1
		writenLen = writen(netdata, dataLen);
		if (writenLen < dataLen) {
			// 发送失败
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
		// 后面会 return ret;
	}
	else {
		//失败返回-1，超时返回-1并且errno = ETIMEDOUT
		if (ret == -1 && errno == ETIMEDOUT)
		{
			/*ret = TimeoutError;
			printf("func sckClient_send() mlloc Err:%d\n ", ret);*/
		}
	}
	// 不对啊这个返回值有点乱
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
		} while (ret == -1 && errno == EINTR);		// 好像没见到过 EAGAIN

		if (ret == 0) {
			// 超时返回
			ret = -1;
			errno = ETIMEDOUT;	// 由于不是系统函数返回所以需要人未指定 errno ？ 
		}
		else if (ret == 1) {
			ret = 0;
		}
	}
	// 这边也有个问题吧， 如果设置非阻塞， waitSeconds 为0, 不管是不是可写都返回 0 ？
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
* writen - 发送固定字节数
* @buf: 发送缓冲区
* @count: 要读取的字节数
* 成功返回count，失败返回-1
*/
int TCPSocket::writen(const void* buf, int count) {
	size_t nleft = count;
	ssize_t nwritten;	// 有符号 size_t
	char* data = (char*)buf;
	while (nleft > 0) {
		if ((nwritten = write(m_connfd, buf, nleft)) < 0) {
			if (errno == EINTR) {
				// 被信号打断
				continue;
			}
			return -1;
		}
		else if (nwritten == 0) {
			// 这个是多余的吧
			// 这个返回 0 和 read 还不一样吗
			continue;
		}

		data += nwritten;
		nleft -= nwritten;
	}
	return count;
}

// 从 buf 开始读 count 个字节， 返回读了几个字节
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
			// 为什么跟 writen 的处理不一样
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
	// 返回0 -> 没超时就接收到了数据, -1, 超时或有异常
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
	ret = readn(&netDataLen, 4); //读包头 4个字节 readn 的返回值是读了几个字节
	
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
		// 知道为啥不一样处理了吧
		printf("func readn() err peer closed:%d \n", ret);
		return string();
	}
	dataBuf[dataLen] = '\0';	//多分配一个字节内容，兼容可见字符串 字符串的真实长度仍然为n
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
              The socket is nonblocking and the connection cannot be completed immediately.  (UNIX domain sockets failed with EA‐
              GAIN  instead.)   It is possible to select(2) or poll(2) for completion by selecting the socket for writing.  After
              select(2) indicates writability, use getsockopt(2) to read the SO_ERROR option at  level  SOL_SOCKET  to  determine
              whether  connect()  completed successfully (SO_ERROR is zero) or unsuccessfully (SO_ERROR is one of the usual error
              codes listed here, explaining the reason for the failure).*/
	// 非阻塞模式连接, 返回-1, 并且errno为EINPROGRESS, 表示连接正在进行中
	if (ret < 0 && errno == EINPROGRESS) {
		fd_set conncet_fdset;
		FD_ZERO(&conncet_fdset);
		FD_SET(m_connfd, &conncet_fdset);

		struct timeval timeout;
		timeout.tv_sec = waitSeconds;
		timeout.tv_usec = 0;
		// 如果在这段时间 connect 成功了呢， 不就会发生错误了么

		do {
			ret = select(m_connfd + 1, NULL, &conncet_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);	// 被信号中断

		// ret 返回的是监听的 fd 状态发生变化的个数
		if (ret == 0) {
			// 超时
			ret = -1;
			// errno 
			errno = ETIMEDOUT;
		}
		else if (ret < 0) {
			return -1;
		}
		else if (ret == 1) {
			/* ret返回为1（表示套接字可写），可能有两种情况，一种是连接建立成功，一种是套接字产生错误，*/
			/* 此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取。 */
			// --> 连接建立`失败时`，socket 文件描述符`既可读又可写`。 （由于有未决的错误，从而可读又可写）
			int err;
			socklen_t errLen = sizeof(err);
			// getsockopt 是干啥的来着
			int sockoptret = getsockopt(m_connfd, SOL_SOCKET, SO_ERROR, &err, &errLen);
			if (sockoptret == -1) {
				return -1;
			}
			else if (err == 0) {
				ret = 0;	// 成功建立连接
			}
			else {
				// 连接失败
				errno = err;
				ret = -1;
			}
		}
	}
	if (waitSeconds > 0) {
		setBlock(m_connfd);	// 套接字设置回阻塞模式 --> 还原
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
