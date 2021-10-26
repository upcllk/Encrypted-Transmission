#include "RespondCodec.h"

RespondCodec::RespondCodec() {

}
RespondCodec::RespondCodec(const std::string& encStr) {
	initMessage(encStr);
}
RespondCodec::RespondCodec(RespondInfo* info) {
	initMessage(info);
}

void RespondCodec::initMessage(const std::string& encStr) {
	m_encStr = encStr;
}
void RespondCodec::initMessage(RespondInfo* info) {
	m_msg.set_status(info->status);
	m_msg.set_seckeyid(info->seckeyID);
	m_msg.set_clientid(info->clientID);
	m_msg.set_serverid(info->serverID);
	m_msg.set_data(info->data);
}

std::string RespondCodec::encodeMsg() {
	std::string output;
	m_msg.SerializeToString(&output);
	return output;
}

void* RespondCodec::decodeMsg() {
	m_msg.ParseFromString(m_encStr);
	return &m_msg;
}

RespondCodec::~RespondCodec() {

}

/*+ encodeMsg() : string
+ decodeMsg() : void*
+ ~Request()*/
