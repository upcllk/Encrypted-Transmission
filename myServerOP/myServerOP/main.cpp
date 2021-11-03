#include <iostream>
#include "ServerOP.h"
using namespace std;


int main()
{
	ServerOP op("serverData.json");
	cout << "server ... start .. " << endl;
	op.startServer();

    return 0;
}
