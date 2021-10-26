#include <iostream>
#include "ClientOP.h"

using namespace std;

int usage();

int main()
{
	// 这边用单例会不会好点
	ClientOP op("serverData.json");
	while (1) {
		int sel = usage();
		if (sel == 0)
			break;
		switch (sel) {
		case 1:
			// 密钥生成
			op.seckeyAgree();
			cout << "secKeyAgree 完成" << endl;
			break;
		case 2:
			// 密钥检验
			op.seckeyCkeck();
			break;
		case 3:
			// 密钥注销
			op.seckeyCancel();
			break;
		default:
			break;
		}
	}
	cout << "客户退出 .. bye ... " << endl;
    return 0;
}

int usage()
{
	int nSel = -1;
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n  /*     1.密钥协商                                            */");
	printf("\n  /*     2.密钥校验                                            */");
	printf("\n  /*     3.密钥注销                                            */");
	printf("\n  /*     4.密钥查看                                            */");
	printf("\n  /*     0.退出系统                                            */");
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n\n  选择:");

	scanf("%d", &nSel);
	while (getchar() != '\n');

	return nSel;
}