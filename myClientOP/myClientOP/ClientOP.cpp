#include "ClientOP.h"
#include <json/json.h>
#include <fstream>
#include <sstream>
#include "RequestFactory.h"
#include "RequestCodec.h"
#include "myRSA.h"
#include "TCPSocket.h"
#include "myHash.h"
#include "RespondCodec.h"
#include "RespondFactory.h"

using namespace std;
using namespace Json;


ClientOP::ClientOP(string jsonFile) {
	// 解析 json 文件 ： 读 --> Value	
	ifstream ifs(jsonFile);
	Reader r;
	Value root;
	r.parse(ifs, root);
	cout << "开始读" << endl;
	cout << root["ServerID"].asString().size() << endl;
	m_info.serverID = root["ServerID"].asString();
	m_info.clientID = root["ClientID"].asString();
	m_info.serverIP = root["ServerIP"].asString();
	m_info.serverPort = root["ServerPort"].asInt();
	cout << "json test" << endl;
	cout << m_info.serverID << endl << m_info.clientID << endl;
	cout << "要链接的服务器 : " << m_info.serverIP << ", " << m_info.serverPort << endl;
}

// 非对称加密的密钥长度
const int KEYLEN = 1024;	

bool ClientOP::seckeyAgree() {
	// 生成密钥对， 读出公钥
	myRSA* rsa = new myRSA;
	rsa->generateRSAKey(KEYLEN);
	// 读公钥文件
	ifstream ifs("public.pem");
	// ifstream ifs("private.pem");
	stringstream str;
	str << ifs.rdbuf();
	ifs.close();
	
	cout << "生成的公钥: " << str.str() << endl;

	// 要初始化序列化数据 序列化的类对象 --> 工厂类创建
	RequestInfo reqInfo;
	reqInfo.clientID = m_info.clientID;
	reqInfo.serverID = m_info.serverID;
	reqInfo.cmd = 1;
	reqInfo.data = str.str();		// 非对称加密的公钥 

	// 创建哈希对象 --> 公钥过长会导致验证签名失败
	myHash sha1(T_SHA1);
	sha1.addData(str.str());
	reqInfo.sign = rsa->rsaSign(sha1.getResult());
	// reqInfo.sign = rsa->rsaSign(str.str());		// 公钥签名

	cout << "签名完成 .. " << endl;

	CodecFactoryBase* factory = new RequestFactory(&reqInfo);
	Codec* c = factory->createCodec();
	// encStr 是待发送的数据
	string encStr = c->encodeMsg();
	// 不要忘了释放资源
	delete factory;
	delete c;
	// 套接字通信， 当前是客户端，发送给服务器端
	TCPSocket* tcp = new TCPSocket;
	int ret = tcp->connectToHost(m_info.serverIP, m_info.serverPort);
	if (ret != 0) {
		cout << "连接服务器失败 .. " << endl;
		return false;
	}
	cout << "连接服务器成功 .. " << endl;

	tcp->sendMsg(encStr);
	string recvMsg = tcp->recvMsg();
	cout << "成功接收到服务器的数据 " << endl;

	factory = new RespondFactory(recvMsg);
	c = factory->createCodec();
	RespondMsg* resData = (RespondMsg*)c->decodeMsg();
	if (!resData->status()) {
		cout << "客户端收到数据，但服务器端处理失败" << endl;
		return false;
	}

	// 将得到的密文解密 --> 对称加密的密钥明文
	cout << "从服务器端收到的密钥加密后的值: " << resData->data() << endl;
	string key = rsa->rsaPriKeyDecrypt(resData->data());
	cout << "客户端收到对称加密密钥 key : " << key << endl << endl;
	cout << "key.size() = " << key.size() << endl;

	delete factory;
	delete c;
	delete rsa;
	delete tcp;

	return true;
}

bool ClientOP::seckeyCkeck()
{
	return false;
}

bool ClientOP::seckeyCancel()
{
	return false;
}

ClientOP::~ClientOP() {

}