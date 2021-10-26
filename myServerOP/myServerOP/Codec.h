#pragma once
#include <iostream>

class Codec
{
public:
	Codec();

	virtual std::string encodeMsg() = 0;
	virtual void* decodeMsg() = 0;

	~Codec();
};
