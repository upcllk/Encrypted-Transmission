#pragma once
#include <iostream>
#include "msg.pb.h"
#include "Codec.h"

struct RequestInfo {
	int cmd;
	std::string clientID;
	std::string serverID;
	std::string sign;
	std::string data;
};


class RequestCodec 
	: public Codec
{
private:
	std::string m_encStr;
	RequestMsg m_msg;	// msg.proto  ÄÚ¶¨Òå
public:
	RequestCodec();
	RequestCodec(const std::string& encstr);
	RequestCodec(RequestInfo* info);
	void initMessage(const std::string& encstr);
	void initMessage(RequestInfo* info);
	std::string encodeMsg();
	void* decodeMsg();

	~RequestCodec();
};
/*
+ initMessage(encstr:string) : void 
+ initMessage(info : RequestInfo*) : void 
+ encodeMsg() : string
+ decodeMsg() : void*
+ ~Request()*/
