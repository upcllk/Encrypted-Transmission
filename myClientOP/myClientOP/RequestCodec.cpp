#include "RequestCodec.h"

RequestCodec::RequestCodec() {

}
RequestCodec::RequestCodec(const std::string& encstr) {
	initMessage(encstr);
}
// RequestInfo ��һ���Զ�����������ݸ�ʽ�Ľṹ��
RequestCodec::RequestCodec(RequestInfo* info) {
	initMessage(info);
}

void RequestCodec::initMessage(RequestInfo* info) {
	m_msg.set_cmdtype(info->cmd);
	m_msg.set_clientid(info->clientID);
	m_msg.set_serverid(info->serverID);
	m_msg.set_sign(info->sign);
	m_msg.set_data(info->data);
}
void RequestCodec::initMessage(const std::string& encstr) {
	m_encStr = encstr;
}

// ���뺯���϶��Ƕ� �ṹ�����͵� m_msg ���в���
std::string RequestCodec::encodeMsg() {
	std::string output;
	m_msg.SerializeToString(&output);
	return output;
}

void* RequestCodec::decodeMsg() {
	m_msg.ParseFromString(m_encStr);
	return &m_msg;
}

RequestCodec::~RequestCodec() {

}