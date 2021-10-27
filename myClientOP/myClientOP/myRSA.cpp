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
	// ret 返回加密后的密文长度
	int ret = RSA_public_encrypt(data.size(), (const unsigned char*)data.data(),
		(unsigned char*)encodeBuf, m_publicKey, RSA_PKCS1_PADDING);
	string retStr = string();
	if (ret < 0) {
		ERR_print_errors_fp(stdout);
	}
	else {
		cout << "加密成功" << endl;
		retStr = toBase64(encodeBuf, ret);
		// retStr = string(encodeBuf, keyLen);
	}
	delete[] encodeBuf;
	return retStr;
}

string myRSA::rsaPriKeyDecrypt(string encData) {
	int keyLen = RSA_size(m_privateKey);
	char* decBuf = new char[keyLen + 1];
	string retStr = string();
	// 数据加密完成之后, 密文长度 == 秘钥长度
	cout << "base64 解码 ： " << fromBase64(encData) << endl;
	int ret = RSA_private_decrypt(keyLen, (const unsigned char*)fromBase64(encData).data(),
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
	// 这个签名也要转换为 base64 否则概率段错误
	cout << "原始签名 : " << string(signBuf, outLen) << endl
		<< "长度" << string(signBuf, outLen).size() << endl;
	retStr = toBase64(signBuf, outLen);
	cout << "base 64 签名： " << retStr << endl;
	// retStr = string(signBuf, outLen);
	delete[] signBuf;
	return retStr;
}

bool myRSA::rsaVerify(string data, string signData, SignLevel level) {
	// cout << "base 64 签名： " << signData << endl;
	string signDataFromBase64 = fromBase64(signData);
	/*cout << "原始签名 : " << signDataFromBase64 << endl <<
		"长度" << signDataFromBase64.size() << endl;*/
	int ret = RSA_verify(level, (const unsigned char*)data.data(), data.size(),
		(const unsigned char*)(signDataFromBase64.data()), signDataFromBase64.size(), m_publicKey);
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

string myRSA::toBase64(const char* data, int len)
{
	BIO* mem = BIO_new(BIO_s_mem());
	BIO* bs64 = BIO_new(BIO_f_base64());
	// 创建 BIO 链 bs64 --> mem
	bs64 = BIO_push(bs64, mem);
	// 写数据
	BIO_write(bs64, data, len);
	BIO_flush(bs64);
	// 得到内存对象指针
	BUF_MEM* memPtr;
	BIO_get_mem_ptr(bs64, &memPtr);
	string retStr = string(memPtr->data, memPtr->length);
	BIO_free_all(bs64);
	return retStr;
}

string myRSA::fromBase64(const string data)
{
	int length = data.size();
	BIO* bs64 = BIO_new(BIO_f_base64());
	BIO* mem = BIO_new_mem_buf(data.data(), length);
	BIO_push(bs64, mem);
	char* buf = new char[length];
	memset(buf, '\0', length);
	int ret = BIO_read(bs64, buf, length);
	BIO_free_all(bs64);
	string retStr = string(buf, ret);
	// 不能简单 strlen 因为签名中可能带有 \0 strlen 会提前结束
	// string retStr = string(buf, strlen(buf));
	free(buf);
	return retStr;
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
