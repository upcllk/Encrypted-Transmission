#include "myHash.h"

// error : 此处不能用 enum class， 改为普通 enum --> 为啥
myHash::myHash(HashType type) {
	m_type = type;
	switch (m_type) {
	case HashType::T_MD5:
		MD5Init();
		break;
	case HashType::T_SHA1:
		SHA1Init();
		break;
	case HashType::T_SHA224:
		SHA224Init();
		break;
	case HashType::T_SHA256:
		SHA256Init();
		break;
	case HashType::T_SHA384:
		SHA384Init();
		break;
	case HashType::T_SHA512:
		SHA512Init();
		break;
	default:
		break;
	}
}

myHash::~myHash() {

}

void myHash::addData(string data) {
	switch (m_type) {
	case HashType::T_MD5:
		MD5AddData(data);
		break;
	case HashType::T_SHA1:
		SHA1AddData(data);
		break;
	case HashType::T_SHA224:
		SHA224AddData(data);
		break;
	case HashType::T_SHA256:
		SHA256AddData(data);
		break;
	case HashType::T_SHA384:
		SHA384AddData(data);
		break;
	case HashType::T_SHA512:
		SHA512AddData(data);
		break;
	default:
		break;
	}
}

string myHash::getResult() {
	string str;
	switch (m_type) {
	case HashType::T_MD5:
		str = MD5GetResult();
		break;
	case HashType::T_SHA1:
		str = SHA1GetResult();
		break;
	case HashType::T_SHA224:
		str = SHA224GetResult();
		break;
	case HashType::T_SHA256:
		str = SHA256GetResult();
		break;
	case HashType::T_SHA384:
		str = SHA384GetResult();
		break;
	case HashType::T_SHA512:
		str = SHA512GetResult();
		break;
	default:
		str = string();
		break;
	}
	return str;
}

inline void myHash::MD5Init() {
	MD5_Init(&m_md5);
}

inline void myHash::SHA1Init() {
	SHA1_Init(&m_sha1);
}

inline void myHash::SHA224Init() {
	SHA224_Init(&m_sha224);
}

inline void myHash::SHA256Init() {
	SHA256_Init(&m_sha256);
}

inline void myHash::SHA384Init() {
	SHA384_Init(&m_sha384);
}

inline void myHash::SHA512Init() {
	SHA512_Init(&m_sha512);
}

inline void myHash::MD5AddData(string data) {
	MD5_Update(&m_md5, data.data(), data.size());
}

inline void myHash::SHA1AddData(string data) {
	SHA1_Update(&m_sha1, data.data(), data.size());
}

inline void myHash::SHA224AddData(string data) {
	SHA224_Update(&m_sha224, data.data(), data.size());
}

inline void myHash::SHA256AddData(string data) {
	SHA256_Update(&m_sha256, data.data(), data.size());
}

inline void myHash::SHA384AddData(string data) {
	SHA384_Update(&m_sha384, data.data(), data.size());
}

inline void myHash::SHA512AddData(string data) {
	SHA512_Update(&m_sha512, data.data(), data.size());
}

string myHash::MD5GetResult() {
	unsigned char md[MD5_DIGEST_LENGTH];
	char res[MD5_DIGEST_LENGTH * 2 + 1];
	MD5_Final(md, &m_md5);
	for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		sprintf(&res[2 * i], "%02x", md[i]);
	}
	// string len is must unless memset(res, '\0', lenofres);
	return string(res, 2 * MD5_DIGEST_LENGTH + 1);
}

string myHash::SHA1GetResult() {
	unsigned char md[SHA_DIGEST_LENGTH];
	char res[SHA_DIGEST_LENGTH * 2 + 1];
	SHA1_Final(md, &m_sha1);
	for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
		sprintf(&res[2 * i], "%02x", md[i]);
	}
	// string len is must unless memset(res, '\0', lenofres);
	return string(res, 2 * SHA_DIGEST_LENGTH + 1);
}

string myHash::SHA224GetResult() {
	unsigned char md[SHA224_DIGEST_LENGTH];
	char res[SHA224_DIGEST_LENGTH * 2 + 1];
	SHA224_Final(md, &m_sha224);
	for (int i = 0; i < SHA224_DIGEST_LENGTH; ++i) {
		sprintf(&res[2 * i], "%02x", md[i]);
	}
	// string len is must unless memset(res, '\0', lenofres);
	return string(res, 2 * SHA224_DIGEST_LENGTH + 1);
}

string myHash::SHA256GetResult() {
	unsigned char md[SHA256_DIGEST_LENGTH];
	char res[SHA256_DIGEST_LENGTH * 2 + 1];
	SHA256_Final(md, &m_sha256);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		sprintf(&res[2 * i], "%02x", md[i]);
	}
	// string len is must unless memset(res, '\0', lenofres);
	return string(res, 2 * SHA256_DIGEST_LENGTH + 1);
}

string myHash::SHA384GetResult() {
	unsigned char md[SHA384_DIGEST_LENGTH];
	char res[SHA384_DIGEST_LENGTH * 2 + 1];
	SHA384_Final(md, &m_sha384);
	for (int i = 0; i < SHA384_DIGEST_LENGTH; ++i) {
		sprintf(&res[2 * i], "%02x", md[i]);
	}
	// string len is must unless memset(res, '\0', lenofres);
	return string(res, 2 * SHA384_DIGEST_LENGTH + 1);
}

string myHash::SHA512GetResult() {
	unsigned char md[SHA512_DIGEST_LENGTH];
	char res[SHA512_DIGEST_LENGTH * 2 + 1];
	SHA512_Final(md, &m_sha512);
	for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
		sprintf(&res[2 * i], "%02x", md[i]);
	}
	// string len is must unless memset(res, '\0', lenofres);
	return string(res, 2 * SHA512_DIGEST_LENGTH + 1);
}

