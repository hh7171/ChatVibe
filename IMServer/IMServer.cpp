#include <iostream>
#include "CKernel.h"
#include <windows.h>

using namespace std;
int main() {
	CKernel kernel;
	if (!kernel.startServer()) {
		cout << "打开服务器失败" << endl;
		return 1;
	}
	else {
		cout << "打开服务器成功" << endl;
	}
	while (1) {
		Sleep(5000);
		cout << "server running" << endl;
	}
	
	return 0;
}