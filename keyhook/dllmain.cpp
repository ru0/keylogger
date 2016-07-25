//CatchKey.cpp
#define _WIN32_WINNT  0x0500				// 设置系统版本, 确保可以使用底层键盘钩子
#define WM_MY_SHORTS (WM_USER + 105)
#include "windows.h"

// 全局变量
LPDWORD		g_lpdwVirtualKey = NULL;		// Keycode 数组的指针
int			g_nLength = 0;					// Keycode 数组的大小
HINSTANCE	g_hInstance = NULL;				// 模块实例句柄
HHOOK		g_hHook = NULL;					// 钩子句柄
HWND		g_hWnd  = NULL;
// DLL 入口函数
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	// 保存模块实例句柄
	g_hInstance = (HINSTANCE)hModule;
	
	// 在进程结束或线程结束时卸载钩子
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxA(NULL,"DLL_PROCESS_ATTACH",":)",MB_ICONINFORMATION);
		break;
	case DLL_THREAD_ATTACH:
		MessageBoxA(NULL,"DLL_THREAD_ATTACH",":)",MB_ICONINFORMATION);
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		delete g_lpdwVirtualKey;
		if (g_hHook != NULL) UnhookWindowsHookEx(g_hHook);
		break;
	}
    return TRUE;
}

/// 底层键盘钩子函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION && wParam == WM_KEYUP)
	{
		BOOL bctrl   =   GetAsyncKeyState(VK_CONTROL)>>((sizeof(SHORT)*8)-1); 
		KBDLLHOOKSTRUCT* pStruct = (KBDLLHOOKSTRUCT*)lParam;
		LPDWORD tmpVirtualKey = g_lpdwVirtualKey;
		if (pStruct->vkCode == 80 && bctrl)
			MessageBoxA(NULL,"WTF",":)",MB_ICONINFORMATION);
			PostMessage(g_hWnd,WM_MY_SHORTS,0,1);
			return TRUE;	
	}
	
	// 传给系统中的下一个钩子
	return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

BOOL WINAPI StartCatch(LPDWORD lpdwVirtualKey, int nLength,  HWND  pWnd)
{
	g_hWnd = pWnd;
	// 如果已经安装键盘钩子则返回 FALSE
	if (g_hHook != NULL) return FALSE;
	// 将用户传来的 keycode 数组保存在全局变量中
	g_lpdwVirtualKey = (LPDWORD)malloc(sizeof(DWORD) * nLength);
	LPDWORD tmpVirtualKey = g_lpdwVirtualKey;
	for (int i = 0; i < nLength; i++)
	{
		*tmpVirtualKey++ = *lpdwVirtualKey++;
	}
	g_nLength = nLength;
	
	// 安装底层键盘钩子
	g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_hInstance, NULL);
	if (g_hHook == NULL) return FALSE;
	return TRUE;
	
}


BOOL WINAPI StopCatch()
{
	// 卸载钩子
	if (UnhookWindowsHookEx(g_hHook) == 0) return FALSE;
	g_hHook = NULL;
	return TRUE;
}