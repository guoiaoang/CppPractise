#include <Windows.h>  // GetSystemDirectory
#include <iostream>
using namespace std;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif // !MAX_PATH
#ifndef COUTLINE
#define COUTLINE_FUNCINFO_NONE(FUNC) cout << ""#FUNC":" << FUNC << endl; 
#define COUTLINE_FUNCINFO_TWO(FUNC,ARG1,ARG2)   FUNC(ARG1,ARG2); \
	cout << ""#FUNC"("#ARG1", "#ARG2") -> " << ""#FUNC"(" << ARG1 << ", " << ARG2 << ")" << endl;
#define COUTLINE_FUNCINFO_THREE(FUNC,ARG1,ARG2,ARG3)   FUNC(ARG1,ARG2,ARG3); \
	cout << ""#FUNC"("#ARG1", "#ARG2", "#ARG3") -> " \
	<< ""#FUNC"(" << ARG1 << ", " << ARG2 << ", " << ARG3 << ")" << endl;
#endif // !COUTLINE

#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")
void func_sockServer(ULONG S_addr = htonl(INADDR_ANY), u_short sin_port = htons(12345)) {
	// 设置
	SOCKADDR_IN addrServer = {};
	addrServer.sin_addr.S_un.S_addr = S_addr;// htonl(INADDR_ANY); // inet_addr("127.0.0.1");
	addrServer.sin_family = AF_INET;     // IPv4
	addrServer.sin_port = sin_port;// htons(12345);  // 字节序转换
	addrServer.sin_zero;                 // 结构体补齐字节，不错处理
	SOCKADDR_IN addrClient = {};
	int iAddrLen = sizeof(addrClient);
	char szBuf[MAX_PATH] = {};
	// 执行
	WSADATA wd = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wd);
	SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 获取系统分配的SOCKET接口ID
	cout << "sockServer:" << sockServer << endl;
	::bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr));   // 将IP与端口绑定到SOKCRT接口ID
	listen(sockServer, SOMAXCONN);  // 监听IP:端口
	SOCKET sockClient = accept(sockServer, (sockaddr*)&addrClient, &iAddrLen); // 从队列中取出一条连接信息
	cout << "sockClient:" << sockClient << "form " << inet_ntoa(addrClient.sin_addr) << ":" << addrClient.sin_port << endl;
	recv(sockClient, szBuf, MAX_PATH, 0);       // 从连接信息中读取消息
	cout << "recv[" << strlen(szBuf) << "]:" << szBuf << endl;
	send(sockClient, szBuf, strlen(szBuf), 0);  // 返回信息
	closesocket(sockClient);
	closesocket(sockServer);
	WSACleanup();
}

void func_sockClient(ULONG S_addr = inet_addr("127.0.0.1"), u_short sin_port = htons(12345), char*szMsg = "Hello Server") {
	SOCKADDR_IN addrServer = {};
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrServer.sin_port = htons(12345);
	addrServer.sin_family = AF_INET;
	char szRecv[MAX_PATH] = {};
	WSADATA wd = {};
	//
	WSAStartup(MAKEWORD(2, 2), &wd);
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int iConnectRtn = connect(sockClient, (sockaddr*)&addrServer, sizeof(addrServer));
	cout << "sock:" << iConnectRtn << " error code:" << GetLastError() << endl;
	if (iConnectRtn == -1) {
		system("pause");
		closesocket(sockClient);
		WSACleanup();
	}
	int iSendRtn = send(sockClient, szMsg, strlen(szMsg) + 1, 0);
	cout << "[" << szMsg << "] is send " << iSendRtn << "/" << strlen(szMsg)+1 << endl;
	int iRecvRtn = recv(sockClient, szRecv, MAX_PATH, 0);
	cout << "[" << szRecv << "] is recv " << iRecvRtn << "/" << MAX_PATH << endl;
	system("pause");
	closesocket(sockClient);
	WSACleanup();
}

int 
	main(
	_In_ int _Argc, 
	_In_count_(_Argc) _Pre_z_ char ** _Argv, 
	_In_z_ char ** _Env
	)
{
	//func_sockServer();
	func_sockClient();
	system("pause");
	return 0;


	CHAR szDir[MAX_PATH] = {};
	cout << "hello World!" << endl << endl;
	COUTLINE_FUNCINFO_NONE(__FUNCSIG__);
	COUTLINE_FUNCINFO_NONE(__FILE__);
	COUTLINE_FUNCINFO_NONE(__LINE__);
	COUTLINE_FUNCINFO_NONE(__DATE__);
	COUTLINE_FUNCINFO_NONE(__TIMESTAMP__);
	COUTLINE_FUNCINFO_NONE(GetCurrentProcessId());
	COUTLINE_FUNCINFO_NONE(GetCurrentProcess());
	COUTLINE_FUNCINFO_NONE(GetCurrentThread());
	COUTLINE_FUNCINFO_NONE(GetCurrentThreadId());
	COUTLINE_FUNCINFO_NONE(GetSystemDefaultLangID());
	COUTLINE_FUNCINFO_NONE(GetSystemDefaultLCID());
	COUTLINE_FUNCINFO_NONE(GetSystemDefaultUILanguage());
	COUTLINE_FUNCINFO_TWO(GetSystemDirectory, szDir, MAX_PATH);
	COUTLINE_FUNCINFO_TWO(GetWindowsDirectory, szDir, MAX_PATH);
	COUTLINE_FUNCINFO_TWO(GetCurrentDirectory, MAX_PATH, szDir);
	COUTLINE_FUNCINFO_THREE(GetModuleFileName, NULL, szDir, MAX_PATH);

	//
	cout << endl << endl << "_Argc:" << _Argc << endl;
	for (int i = 0; i < _Argc; i++)
		cout << "_Argv(" << i << "):[" << _Argv[i] << ")" << endl << endl;

	cout << "_Env:" << endl;
	for (int i=0;;i++) {
		if(!_Env[i]) break;
		cout << "(" << i << ")[" << _Env[i] << "]" << endl;
	}
	getchar();
	return 0;
}

