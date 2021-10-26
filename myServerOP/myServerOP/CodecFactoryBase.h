#pragma once
#include "Codec.h"

class CodecFactoryBase
{
public:
	CodecFactoryBase();
	
	virtual Codec* createCodec() = 0;

	virtual ~CodecFactoryBase();
};

/*CodecFactory
+ CodecFactory()
+ createCodec() : Codec*
+ ~CodecFactory()
*/

