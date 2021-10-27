#pragma once
#include <iostream>
#include "CodecFactoryBase.h"
#include "RequestCodec.h"

class RequestFactory
	: public CodecFactoryBase
{
public:
	RequestFactory(std::string enc);
	RequestFactory(RequestInfo* info);

	Codec* createCodec();

	~RequestFactory();
private:
	bool m_flag;
	std::string m_encStr;
	RequestInfo* m_info;
};

