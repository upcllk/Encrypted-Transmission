#pragma once
#include "Codec.h"
#include "msg.pb.h"
#include <iostream>

struct RespondInfo {
	int status;
	int seckeyID;
	std::string clientID;
	std::string serverID;
	std::string data;
};

class RespondCodec
	: public Codec
{
private:
	std::string m_encStr;
	RespondMsg m_msg;
public:
	RespondCodec();
	RespondCodec(const std::string& encstr);
	RespondCodec(RespondInfo* info);
	void initMessage(const std::string& encStr);
	void initMessage(RespondInfo* info);
	std::string encodeMsg();
	void* decodeMsg();


	~RespondCodec();
};


