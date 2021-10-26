#include "RequestFactory.h"

RequestFactory::RequestFactory(std::string enc) {
	m_flag = false;
	m_encStr = enc;
}
RequestFactory::RequestFactory(RequestInfo* info) {
	m_flag = true;
	m_info = info;
}

Codec* RequestFactory::createCodec() {
	Codec* codec = nullptr;
	if (m_flag == true)
		codec = new RequestCodec(m_info);
	else
		codec = new RequestCodec(m_encStr);
	return codec;
}

RequestFactory::~RequestFactory() {

}