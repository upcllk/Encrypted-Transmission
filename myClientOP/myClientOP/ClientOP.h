#pragma once
#include <string>
using namespace std;

struct ClientInfo {
	string serverID;
	string clientID;
	string serverIP;
	unsigned short serverPort;
};

class ClientOP
{
public:
	ClientOP(string jsonFile);

	// 密钥协商
	bool seckeyAgree();

	// 密钥校验
	bool seckeyCkeck();

	// 密钥注销
	bool seckeyCancel();

	~ClientOP();
private:
	ClientInfo m_info;
};

