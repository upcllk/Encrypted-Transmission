#include "myRSA.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <iostream>
#include <openssl/buffer.h>
#include <string.h>

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
	BN_set_word(e, 114514);
	RSA_generate_key_ex(rsa, bits, e, NULL);
	m_publicKey = RSAPublicKey_dup(rsa);
	m_privateKey = RSAPrivateKey_dup(rsa);

	BIO* pubBio = BIO_new_file(pubFileName.data(), "W");
	PEM_write_bio_RSAPublicKey(pubBio, rsa);
	//// 缓存中的数据刷到文件中
	//BIO_flush(pubIO);
	BIO_free(pubBio);

	BIO* priBio = BIO_new_file(priFileName.data(), "w");
	PEM_write_bio_RSAPrivateKey(priBio, rsa, NULL, NULL, 0, NULL, NULL);
	//BIO_flush(priIO);
	BIO_free(priBio);

	RSA_free(rsa);
	BN_free(e);
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
		// retStr = toBase64(encode, ret);
		retStr = string(encodeBuf, keyLen);
	}
	delete[] encodeBuf;
	return retStr;
}

string myRSA::rsaPriKeyDecrypt(string encData) {
	int keyLen = RSA_size(m_privateKey);
	char* decBuf = new char[keyLen];
	// 数据加密完成之后, 密文长度 == 秘钥长度
	int ret = RSA_public_decrypt(encData.size(), (const unsigned char*)encData.data(), 
			(unsigned char*)decBuf, m_privateKey, RSA_PKCS1_PADDING);
	string retStr = string();
	if (ret < 0) {
		ERR_print_errors_fp(stdout);
	}
	else {
		cout << "私钥解密成功" << endl;
		retStr = string(decBuf, keyLen);
	}
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
		ERR_print_errors_fp(stdout);
		// " "
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
