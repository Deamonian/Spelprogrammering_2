// winmain.cpp

#include "stdafx.h"
#include "GPIIBase.h"

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow) {
	System::OpenLog();
	System::Log("Logfile opening");
	const int width=1280,height=720;

	// create window class for registration
	WNDCLASSEX wc;
	wc.cbSize=sizeof(WNDCLASSEX);
	wc.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wc.lpfnWndProc=WndProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=::LoadIcon(wc.hInstance,MAKEINTRESOURCE(IDI_GPIIBASE));
	wc.hIconSm=::LoadIcon(wc.hInstance,MAKEINTRESOURCE(IDI_GPIIBASE));
	wc.hCursor=::LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=(HBRUSH)::GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName=NULL;
	wc.lpszClassName=L"kWndCls";

	if(!::RegisterClassEx(&wc)) {
		System::Notify("Could not register window class","Error!");
		return 1;
	};

	unsigned int style=(WS_OVERLAPPEDWINDOW&~(WS_THICKFRAME|WS_MAXIMIZEBOX|WS_MINIMIZEBOX));
	RECT rc={0,0,width,height}; 
	::AdjustWindowRect(&rc,style,FALSE); 

	// create the main window
	HWND hWnd=::CreateWindow(
		L"kWndCls",
		L"RenderWindow",
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right-rc.left,
		rc.bottom-rc.top,
		NULL,NULL,
		hInstance,
		NULL);
	if(hWnd==NULL) {
		System::Notify("Could not create a window","Error!");
		return 1;
	};

	::ShowWindow(hWnd,nCmdShow);
	::UpdateWindow(hWnd);

	MSG msg={0};
	while(msg.message!=WM_QUIT) {
		if(::PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else {
			// here be code
		};
		::Sleep(2);
	};
	::DestroyWindow(hWnd);

	System::Log("Logfile closing");
	System::CloseLog();

	return 0;
};

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch(uMsg) {
	case WM_CLOSE:
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	};
	return 0;
};
