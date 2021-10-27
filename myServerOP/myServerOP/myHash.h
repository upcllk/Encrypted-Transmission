#pragma once
#include <iostream>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <string.h>

using namespace std;

// 这么定义后面会有 warning 未设置范围建议使用 enum class
enum HashType {
	T_MD5,
	T_SHA1,
	T_SHA224,
	T_SHA256,
	T_SHA384,
	T_SHA512
};

// error : 此处不能用 enum class， 改为普通 enum --> 为啥 
// 同时 __float128 报错，百度说是因为不支持 c++ 11 ？ 两个新特性 get ？
//enum class HashType {
//	T_MD5,
//	T_SHA1,
//	T_SHA224,
//	T_SHA256,
//	T_SHA384,
//	T_SHA512
//};

class myHash
{
public:
	myHash(HashType type);
	~myHash();

	void addData(string data);
	string getResult();

private:
	inline void MD5Init();
	inline void SHA1Init();
	inline void SHA224Init();
	inline void SHA256Init();
	inline void SHA384Init();
	inline void SHA512Init();

	inline void MD5AddData(string data);
	inline void SHA1AddData(string data);
	inline void SHA224AddData(string data);
	inline void SHA256AddData(string data);
	inline void SHA384AddData(string data);
	inline void SHA512AddData(string data);

	string MD5GetResult();
	string SHA1GetResult();
	string SHA224GetResult();
	string SHA256GetResult();
	string SHA384GetResult();
	string SHA512GetResult();

private:
	HashType m_type;
	MD5_CTX m_md5; 
	SHA_CTX m_sha1;
	SHA256_CTX m_sha224;
	SHA256_CTX m_sha256;
	SHA512_CTX m_sha384;
	SHA512_CTX m_sha512;
};

