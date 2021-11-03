#include "ServerOP.h"
#include "TCPServer.h"
#include "TCPSocket.h"
#include <json/json.h>
#include <fstream>
#include <string>
#include <iostream>
#include "RequestFactory.h"
#include "RequestCodec.h"
#include "RespondCodec.h"
#include "RespondFactory.h"
#include "myRSA.h"
#include "myHash.h"

using namespace Json;
using namespace std;

ServerOP::ServerOP(string json) {
	// 读文件， 初始化 m_port
	ifstream ifs(json);
	Reader r;
	Value root;
	r.parse(ifs, root);
	m_port = root["Port"].asInt();
	m_serverID = root["ServerID"].asString();
	// 数据库相关信息
	m_dbUser = root["UserDB"].asString();
	m_dbPwd = root["PwdDB"].asString();
	m_dbConnStr = root["ConnStr"].asString();
	// 实例化一个连接 Orcale 数据库的对象
	m_occi.connectDB(m_dbUser, m_dbPwd, m_dbConnStr);
}

ServerOP::~ServerOP() {
	if (m_server != NULL) {
		delete m_server;
	}
	// delete[] map.second ?
}

void ServerOP::startServer() {
	m_server = new TCPServer;
	m_server->setListen(m_port);
	cout << "set listen .. ok" << endl;
	while (1) {
		cout << "等待客户端连接.. " << endl;
		TCPSocket* tcp = m_server->acceptConn();
		if (tcp == NULL) {
			continue;
		}
		cout << "连接成功， 创建工作线程" << endl;
		// 通信
		pthread_t tid;
		// 这个回调可以是类的静态函数、友元函数、普通的函数
		// 这边有可能有同步问题，
		pthread_create(&tid, NULL, work, this);
		m_list.insert(make_pair(tid, tcp));
	}
}

void* ServerOP::work(void* args)
{
	cout << "start work .. " << endl;
	sleep(1);	// 先这样以后加互斥锁
	string data = string();
	// args --> this
	ServerOP* op = (ServerOP*)args;
	// 从 op  中取出通信的套接字对象 tcp --> pthread_self
	TCPSocket* tcp = op->m_list[pthread_self()];
	// 1. 接受客户端数据 --> 编码
	string msg = tcp->recvMsg();
	// 2. 反序列化得到原始数据 --> ReauestMsg类型
	CodecFactoryBase* factory = new RequestFactory(msg);
	Codec* c = factory->createCodec();
	RequestMsg* reqmsg = (RequestMsg*)c->decodeMsg();

	// 3. 取数据 cmdType 判定
	switch (reqmsg->cmdtype()) {
	case 1:
		// 密钥协商
		cout << "开始密钥协商" << endl;
		data = op->secKeyAgree(reqmsg);
		cout << "密钥协商完成" << endl;
		break;
	case 2:
		// 密钥校验
		op->secKeyCheck();
		break;
	case 3:
		// 密钥取消
		op->secKeyCancel();
		break;
	default:
		break;
	}

	delete factory;
	delete c;
	// tcp 对象如何处理
	tcp->sendMsg(data);
	tcp->disConnected();
	op->m_list.erase(pthread_self());
	delete tcp;

	return NULL;
}

// #include "msg.pb.h"
string ServerOP::secKeyAgree(RequestMsg* reqMsg) {
	// 0. 校验签名 --> 公钥解密 --> requestMsg->data()
	// 
	// 其中一种方式是将收到的公钥数据写入磁盘 然后读磁盘
	ofstream ofs("public.pem");
	ofs << reqMsg->data();
	ofs.close();	// must 

	myRSA rsa("public.pem", false);

	cout << "从客户端收到的公钥： " << reqMsg->data() << endl;

	myHash sha1(T_SHA1);
	sha1.addData(reqMsg->data());
	string hashStr = sha1.getResult();
	cout << "hash 长度 : " << hashStr.size() << endl;
	cout << "sign 长度 : " << reqMsg->sign().size() << endl;
	bool bl = rsa.rsaVerify(hashStr, reqMsg->sign());
	// bool bl = rsa.rsaVerify(hashData, reqMsg->sign());
	// 段错误 --> rsa 加解密操作数据不能长于密钥长度(公钥太长了 因为有 base64 编码)
	// 可通过哈希运算缩小长度
	// bool bl = rsa.rsaVerify(reqMsg->data(), reqMsg->sign());
	RespondInfo info;
	if (bl == false) {
		cout << "签名校验失败" << endl;
		info.status = false;
	}
	else {
		cout << "签名校验成功" << endl;
		// 1. 生成随机字符串 即对称加密的密钥, 长度有要求
		// 对称加密的密钥， 使用 aes 算法， 密钥长度 16 24 32 byte
		string key = getRandKey(AESKeyLen::LEN16);
		cout << "服务器端生成的对称加密密钥 key : " << key << endl;
		// 2. 通过公钥加密
		string secKey = rsa.rsaPubKeyEncrypt(key);
		cout << "服务器端通过公钥 rsa 加密后的 key: " << secKey << endl;
		
		// 3. 初始化回复的数据
		info.status = true;
		info.clientID = reqMsg->clientid();
		info.data = secKey;
		// info.data = key;
		info.seckeyID = 12;	// 需要数据库操作
		info.serverID = m_serverID;

		cout << info.data.size() << endl;
		cout << "info.data : " << info.data << endl;
		
		// 将生成的密钥写入数据库
		NodeSecKeyInfo node;
		strcpy(node.clientID, reqMsg->clientid().data());
		strcpy(node.serverID, reqMsg->serverid().data());
		strcpy(node.seckey, key.data());
		node.seckeyID = m_occi.getKeyID();	// 秘钥的ID
		node.status = 1;
		// 初始化node变量
		bool bl = m_occi.writeSecKey(&node);
		if (bl)
		{
			// 成功
			m_occi.updataKeyID(node.seckeyID + 1);
		}
		else
		{
			// 失败
			info.status = false;
		}
		
	}
	// 4. 序列化并发送
	CodecFactoryBase* factory = new RespondFactory(&info);
	Codec* c = factory->createCodec();
	string encMsg = c->encodeMsg();

	delete factory;
	delete c;

	return encMsg;
}

void ServerOP::secKeyCheck() {
	
}

void ServerOP::secKeyCancel() {
	
}

string ServerOP::getRandKey(AESKeyLen len)
{
	int flag = 0;
	srand(time(NULL));
	string randStr = string();
	char* cs = "~!@#$%^&*()_+{}:";
	for (int i = 0; i < len; ++i) {
		flag = rand() % 4;
		switch (flag) {
		case 0:
			// a - z
			randStr.append(1, 'a' + rand() % 26);
			break;
		case 1:
			// A - Z
			randStr.append(1, 'A' + rand() % 26);
			break;
		case 2:
			// 0 - 9
			randStr.append(1, '0' + rand() % 10);
			break;
		case 3:
			// 特殊字符
			randStr.append(1, cs[rand() % strlen(cs)]);
			break;
		default:
			break;
		}
	}
	return randStr;
}
