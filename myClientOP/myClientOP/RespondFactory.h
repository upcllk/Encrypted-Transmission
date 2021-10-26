#pragma once
#include <iostream>
#include "CodecFactoryBase.h"
#include "RespondCodec.h"

class RespondFactory
	: public CodecFactoryBase
{
public:
	RespondFactory(std::string enc);
	RespondFactory(RespondInfo* info);

	Codec* createCodec();

	~RespondFactory();
private:
	bool m_flag;
	std::string m_encStr;
	RespondInfo* m_info;
};

