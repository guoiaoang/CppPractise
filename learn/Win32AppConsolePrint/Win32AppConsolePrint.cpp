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
    SetConsoleTitle("�����������̨");
    freopen("CONOUT$","w+t",stdout);
    freopen("CONIN$","r+t",stdin);
	//FreeConsole();
#endif  // CONSOLE_LOG
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut,FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	std::cout << "��ǰ�ļ���" << __FILE__ << std::endl;
	std::cout << "�������ӣ�" << __FUNCSIG__ << std::endl;
	SetConsoleTextAttribute(hOut,FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	std::cout << "����ʱ�䣺" << __DATE__ << " - " << __TIME__ << std::endl;

	return;
	for(;0;){
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); 
		// ��ȡ��׼����豸��� 
		CONSOLE_SCREEN_BUFFER_INFO bInfo; // ���ڻ�������Ϣ 
		GetConsoleScreenBufferInfo(hOut, & bInfo ); 
		// ��ȡ���ڻ�������Ϣ 
		char strTitle[255]; 
		GetConsoleTitle(strTitle, 255); // ��ȡ���ڱ��� 
		//printf("��ǰ���ڱ����ǣ�%s\n", strTitle); 
		//_getch(); 
		COORD size = {1024, 1024}; 
		SetConsoleScreenBufferSize(hOut,size); // �������û�������С 
		//_getch(); 
		SMALL_RECT rc = {0,0, 1024-1, 1024-1}; // ���ô���λ�úʹ�С 
		SetConsoleWindowInfo(hOut,true ,&rc); 
		SetConsoleTextAttribute(hOut,FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	}
}

// ���ڴ�����(�Զ��� ������Ϣ)
LRESULT CALLBACK WndProc(HWND hWnd,UINT msgID,
	WPARAM wParam,LPARAM lParam)
{
	switch(msgID)
	{
	case WM_CREATE:
		addConsole();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);// GetMessage����0?
		break;
	}
	return DefWindowProc(hWnd,msgID,wParam,lParam);
}
// ע�ᴰ����
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
// ��������
HWND CreateMain(LPSTR lpClassName,LPSTR lpWndName)
{
	HWND hWnd = CreateWindowEx(0,lpClassName,lpWndName,
		WS_OVERLAPPEDWINDOW,100,100,700,500,
		NULL,NULL,g_hInstance,NULL);
	return hWnd;
}
// ��ʾ����
void Display(HWND hWnd)
{
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);
}
// ��Ϣѭ��
void Message()
{
	MSG nMsg = {0};
	while(GetMessage(&nMsg,NULL,0,0))
	{
		TranslateMessage(&nMsg);
		DispatchMessage(&nMsg);
	}
}
// ��ں���
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
