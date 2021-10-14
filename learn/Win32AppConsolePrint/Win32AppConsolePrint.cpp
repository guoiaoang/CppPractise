#include <windows.h>
#include <iostream>
//#pragma warning(push)
#pragma warning(disable:4996)  // freopen_s
//#pragma warning(pop)
HINSTANCE g_hInstance = 0;
void addConsole(){
#define CONSOLE_LOG 1
#if CONSOLE_LOG
    AllocConsole();
    SetConsoleTitle("调试输出控制台");
    freopen("CONOUT$","w+t",stdout);
    freopen("CONIN$","r+t",stdin);
	//FreeConsole();
#endif  // CONSOLE_LOG
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut,FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	std::cout << "当前文件：" << __FILE__ << std::endl;
	std::cout << "函数链接：" << __FUNCSIG__ << std::endl;
	SetConsoleTextAttribute(hOut,FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	std::cout << "编译时间：" << __DATE__ << " - " << __TIME__ << std::endl;

	return;
	for(;0;){
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); 
		// 获取标准输出设备句柄 
		CONSOLE_SCREEN_BUFFER_INFO bInfo; // 窗口缓冲区信息 
		GetConsoleScreenBufferInfo(hOut, & bInfo ); 
		// 获取窗口缓冲区信息 
		char strTitle[255]; 
		GetConsoleTitle(strTitle, 255); // 获取窗口标题 
		//printf("当前窗口标题是：%s\n", strTitle); 
		//_getch(); 
		COORD size = {1024, 1024}; 
		SetConsoleScreenBufferSize(hOut,size); // 重新设置缓冲区大小 
		//_getch(); 
		SMALL_RECT rc = {0,0, 1024-1, 1024-1}; // 重置窗口位置和大小 
		SetConsoleWindowInfo(hOut,true ,&rc); 
		SetConsoleTextAttribute(hOut,FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	}
}

// 窗口处理函数(自定义 处理消息)
LRESULT CALLBACK WndProc(HWND hWnd,UINT msgID,
	WPARAM wParam,LPARAM lParam)
{
	switch(msgID)
	{
	case WM_CREATE:
		addConsole();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);// GetMessage返回0?
		break;
	}
	return DefWindowProc(hWnd,msgID,wParam,lParam);
}
// 注册窗口类
void Register(LPSTR lpClassName,WNDPROC wndProc)
{
	WNDCLASSEX wce = {0};
	wce.cbSize = sizeof(wce);
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wce.hCursor = NULL;
	wce.hIcon = NULL;
	wce.hIconSm = NULL;
	wce.hInstance = g_hInstance;
	wce.lpfnWndProc = wndProc;
	wce.lpszClassName = lpClassName;
	wce.lpszMenuName = NULL;
	wce.style = CS_HREDRAW|CS_VREDRAW;
	RegisterClassEx(&wce);
}
// 创建窗口
HWND CreateMain(LPSTR lpClassName,LPSTR lpWndName)
{
	HWND hWnd = CreateWindowEx(0,lpClassName,lpWndName,
		WS_OVERLAPPEDWINDOW,100,100,700,500,
		NULL,NULL,g_hInstance,NULL);
	return hWnd;
}
// 显示窗口
void Display(HWND hWnd)
{
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);
}
// 消息循环
void Message()
{
	MSG nMsg = {0};
	while(GetMessage(&nMsg,NULL,0,0))
	{
		TranslateMessage(&nMsg);
		DispatchMessage(&nMsg);
	}
}
// 入口函数
int CALLBACK WinMain(HINSTANCE hIns,HINSTANCE hPreIns,
	LPSTR lpCmdLine,int nShowCmd)
{
	g_hInstance = hIns;
	Register("Main",WndProc);
	HWND hWnd = CreateMain("Main","Window");
	Display(hWnd);
	Message();
	return 0;
}
