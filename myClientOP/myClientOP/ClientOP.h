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

	// ��ԿЭ��
	bool seckeyAgree();

	// ��ԿУ��
	bool seckeyCkeck();

	// ��Կע��
	bool seckeyCancel();

	~ClientOP();
private:
	ClientInfo m_info;
};

