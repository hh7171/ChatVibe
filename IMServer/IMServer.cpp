#include <iostream>
#include "CKernel.h"
#include <windows.h>

using namespace std;
int main() {
	CKernel kernel;
	if (!kernel.startServer()) {
		cout << "�򿪷�����ʧ��" << endl;
		return 1;
	}
	else {
		cout << "�򿪷������ɹ�" << endl;
	}
	while (1) {
		Sleep(5000);
		cout << "server running" << endl;
	}
	
	return 0;
}