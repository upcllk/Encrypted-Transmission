#include <iostream>
#include "ServerOP.h"
using namespace std;


int main()
{
	ServerOP op("serverData.json");
	op.startServer();

    return 0;
}
