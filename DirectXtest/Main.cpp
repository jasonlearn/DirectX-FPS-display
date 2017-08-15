#include "Includes.h"

/*
 *  Purpose: Exposure to setting up a DirectX environment
 *	This program uses DirectX fullscreen environment, load, and displays a bitmap
 *	
 *	@Project	Comp4995 Assignment#1
 *	@Author		Jason Chan A00698160
 */

/*
 * Class Main, main function and entry point of the project.
 *
 * @param hInstance handle to an instance/module, oS uses this to identify the exe when loaded to memory
 * @param hPrevInstance used in 16-bit Windows, filler variable
 * @param pstrCmdLine command-line arg as a Unicode string
 * @param iCmdShow flag that says whether the main application window will be minimized, maximzed, or shown normally
 *
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pstrCmdLine, int iCmdShow) {
	HWND hWnd;
	MSG msg;
	WNDCLASSEX wc;
	Game g;

	static TCHAR strAppName[] = _T("First DirectX Assignment");

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = Game::StaticWndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_HAND);
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = strAppName;

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL,
		strAppName,
		strAppName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	g.setHandle(hWnd);

	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)&g);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	// Game initialize
	// If initialization fails, calls GameShutDown function to clean up and unload 
	if (FAILED(g.GameInit())) {
		SetError(_T("Initialization Failed"));
		g.GameShutdown();
		// return fail message initialization was unsuccessful
		return E_FAIL;
	}

	// Loops until user hits escapse key esc
	while (TRUE) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// Called GameLoop in Game until esacpe key is entered
			g.GameLoop();
		}
	}

	// Calls GameShutDown function in Game, to clean up and unload.
	g.GameShutdown();
	return msg.wParam;
}
