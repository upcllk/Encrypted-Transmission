#include "myRSA.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <iostream>
#include <openssl/buffer.h>
#include <string.h>
#include <unistd.h>

myRSA::myRSA() {
	m_publicKey = RSA_new();
	m_privateKey = RSA_new();
}

myRSA::myRSA(string fileName, bool isPrivate) {
	m_publicKey = RSA_new();
	m_privateKey = RSA_new();
	if (isPrivate) {
		initPrivateKey(fileName);
	}
	else {
		initPublicKey(fileName);
	}
}

myRSA::~myRSA() {

}

void myRSA::generateRSAKey(int bits, string pubFileName, string priFileName) {
	RSA* rsa = RSA_new();
	BIGNUM* e = BN_new();
	// 这个数字设不合适 generate key 过程会卡住 太大太小都不行 有什么讲究吗
	BN_set_word(e, 12345);
	RSA_generate_key_ex(rsa, bits, e, NULL);
	m_publicKey = RSAPublicKey_dup(rsa);
	m_privateKey = RSAPrivateKey_dup(rsa);
	BIO* pubBio = BIO_new_file(pubFileName.data(), "w");
	PEM_write_bio_RSAPublicKey(pubBio, rsa);

	BIO_free(pubBio);

	BIO* priBio = BIO_new_file(priFileName.data(), "w");
	PEM_write_bio_RSAPrivateKey(priBio, rsa, NULL, NULL, 0, NULL, NULL);
	BIO_free(priBio);

	RSA_free(rsa);
	BN_free(e);
	cout << "RSAKey 生成成功" << endl;
}

string myRSA::rsaPubKeyEncrypt(string data) {
	int keyLen = RSA_size(m_publicKey);
	char* encodeBuf = new char[keyLen];
	int ret = RSA_public_encrypt(data.size(), (const unsigned char*)data.data(),
		(unsigned char*)encodeBuf, m_publicKey, RSA_PKCS1_PADDING);
	string retStr = string();
	if (ret < 0) {
		ERR_print_errors_fp(stdout);
	}
	else {
		cout << "加密成功" << endl;
		// retStr = toBase64(encode, ret);
		retStr = string(encodeBuf, keyLen);
	}
	delete[] encodeBuf;
	return retStr;
}

string myRSA::rsaPriKeyDecrypt(string encData) {
	int keyLen = RSA_size(m_privateKey);
	char* decBuf = new char[keyLen + 1];

	string retStr = string();
	// 数据加密完成之后, 密文长度 == 秘钥长度
	int ret = RSA_public_decrypt(encData.size(), (const unsigned char*)encData.data(), 
			(unsigned char*)decBuf, m_privateKey, RSA_PKCS1_PADDING);
	if (ret < 0) {
		cout << "私钥解密失败" << endl;
		ERR_print_errors_fp(stdout);
		cout << "再说一遍， 私钥解密失败" << endl;
	}
	else {
		cout << "私钥解密成功" << endl;
		retStr = string(decBuf, keyLen);
	}
	cout << "keyLen : " << keyLen << endl;
	cout << "retStr : " << retStr << endl;

	delete[] decBuf;
	// delete [] base64
	return retStr;
}

string myRSA::rsaSign(string data, SignLevel level) {
	// int RSA_sign(int type, const unsigned char *m, unsigned int m_length,
	//				unsigned char* sigret, unsigned int* siglen, RSA* rsa);
	int len = RSA_size(m_privateKey);
	unsigned int outLen = 0;
	char* signBuf = new char[len];
	string retStr = string();
	int ret = RSA_sign(level, (const unsigned char*)data.data(), 
				data.size(), (unsigned char*)signBuf, &outLen, m_privateKey);
	if (ret == -1)
	{
		cout << "RSA 签名失败 " << endl;
		delete[] signBuf;
		return retStr;
	}
	cout << "sign outlen: " << outLen << ", ret: " << ret << endl;
	retStr = string(signBuf, outLen);
	delete[] signBuf;
	return retStr;
}

bool myRSA::rsaVerify(string data, string signData, SignLevel level) {
	int ret = RSA_verify(level, (const unsigned char*)data.data(), data.size(),
		(const unsigned char*)signData.data(), signData.size(), m_publicKey);
	if (ret == -1)
	{
		ERR_print_errors_fp(stdout);
	}
	if (ret != 1)
	{
		return false;
	}
	return true;
}

bool myRSA::initPrivateKey(string privateFileName) {
	BIO* priBio = BIO_new_file(privateFileName.data(), "r");
	if ((PEM_read_bio_RSAPrivateKey(priBio, &m_privateKey, NULL, NULL) == NULL)) {
		ERR_print_errors_fp(stdout);	// ?
		BIO_free(priBio);
		return false;
	}
	BIO_free(priBio);
	return true;
}

bool myRSA::initPublicKey(string publicFileName) {
	BIO* pubBio = BIO_new_file(publicFileName.data(), "r");
	if ((PEM_read_bio_RSAPublicKey(pubBio, &m_publicKey, NULL, NULL) == NULL)) {
		ERR_print_errors_fp(stdout);
		BIO_free(pubBio);
		return false;
	}
	BIO_free(pubBio);
	return true;
}
