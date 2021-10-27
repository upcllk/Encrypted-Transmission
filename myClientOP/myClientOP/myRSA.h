#pragma once
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace std;

enum SignLevel
{
	Level1 = NID_md5,
	Level2 = NID_sha1,
	Level3 = NID_sha224,
	Level4 = NID_sha256,
	Level5 = NID_sha384,
	Level6 = NID_sha512
};


class myRSA
{
public:
	myRSA();
	myRSA(string fileName, bool isPrivate = false);
	~myRSA();
	void generateRSAKey(int bits, 
		string pubFileName = "public.pem", string priFileName = "private.pem");
	// ��Կ����
	string rsaPubKeyEncrypt(string data);
	// ˽Կ����
	string rsaPriKeyDecrypt(string encData);
	// ����ǩ��
	string rsaSign(string data, SignLevel level = Level3);
	// ��֤ǩ��
	bool rsaVerify(string data, string signData, SignLevel level = Level3);

private:
	bool initPublicKey(string publicFile);
	bool initPrivateKey(string privateFile);
	string toBase64(const char* data, int len);
	string fromBase64(const string data);

private:
	RSA* m_publicKey;
	RSA* m_privateKey;

};

