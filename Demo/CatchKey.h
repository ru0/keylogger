//CatchKey.h
//DECLSPEC_IMPORT
BOOL
WINAPI
StartCatch(
	LPDWORD lpdwVirtualKey,
	int nLength,
	HWND  pWnd
	);

//DECLSPEC_IMPORT
BOOL
WINAPI
StopCatch();
