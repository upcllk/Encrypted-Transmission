#include <iostream>
#include "Codec.h"
#include "RequestCodec.h"
#include "RespondCodec.h"
#include "RequestFactory.h"
#include "RespondFactory.h"

using namespace std;

/*RequestInfo							RespondInfo
+ cmd : int								+ status : int
+ clientID : string						+ seckeyID : int
+ serverID :  string					+ clientID :  string
+ sign : string							+ serverID : string
+ data : string							+ data : string */

int main() {
	RequestInfo info{ 1, "client", "server", "x00911", "hello, world" };
	CodecFactoryBase* factory = new RequestFactory(&info);
	Codec* codec = factory->createCodec();
	string str = codec->encodeMsg();
	cout << "序列化后的数据" << str << endl;
	delete(codec);
	delete(factory);
	cout << "encode ok" << endl;

	factory = new RequestFactory(str);
	codec = factory->createCodec();
	RequestMsg* msg =  (RequestMsg*)codec->decodeMsg();
	cout << "decode ok" << endl;
	cout << msg->cmdtype() << " " << msg->clientid() << " " << msg->serverid()
		<< " " << msg->sign() << " " << msg->data() << endl;

	delete(factory);
	delete(codec);

	RespondInfo resinfo = { 114, 514, "1919", "810", "this is a data string" };
	factory = new RespondFactory(&resinfo);
	codec = factory->createCodec();
	str = codec->encodeMsg();
	cout << str << endl;
	delete(factory);
	delete(codec);
	factory = new RespondFactory(str);
	codec = factory->createCodec();
	RespondMsg* res = (RespondMsg*)codec->decodeMsg();
	cout << res->status() << " " << res->seckeyid() << " " << res->clientid()
		<< " " << res->serverid() << res->data() << endl;

	system("pause");
	return 0;
}