// winmain.cpp

#include "stdafx.h"
#include "MessageSystem.h"
#include "Application.h"
#include "GPIIBase.h"

MessageSystem message_system;

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

int APIENTRY _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow) {
	System::OpenLog();
	System::Log("Logfile opening");

	if(!System::FileExists("../data/config.txt")) {
		System::Notify("Could not find a config-file!","Error!");
		return 1;
	};
	ConfigFile cfg("../data/config.txt");
	if(!cfg.HasKey("width")) {cfg.Set("width",ToString(1280));};
	if(!cfg.HasKey("height")) {cfg.Set("height",ToString(720));};
	if(!cfg.HasKey("m_yaw")) {cfg.Set("m_yaw",ToString(0.022f));};
	if(!cfg.HasKey("m_pitch")) {cfg.Set("m_pitch",ToString(-0.022f));};
	if(!cfg.HasKey("m_sensitivity")) {cfg.Set("m_sensitivity",ToString(50.0f));};
	if(!cfg.HasKey("c_speed")) {cfg.Set("c_speed",ToString(20.0f));};
	if(!cfg.HasKey("c_znear")) {cfg.Set("c_znear",ToString(0.5f));};
	if(!cfg.HasKey("c_zfar")) {cfg.Set("c_zfar",ToString(1000.0f));};

	const int width=cfg.AsInt("width"),height=cfg.AsInt("height");

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

	// create application
	Application application;
	if(!application.Init(&message_system,hWnd,cfg)) {
		System::Notify("Could not initialize Application!\n"\
			"Read log.txt for more information.","Error!");
		return -1;
	};

	MSG msg={0};
	while(msg.message!=WM_QUIT) {
		if(::PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else {
			application.Tick();
			::Sleep(2);
		};
	};

	application.Shut();

	::DestroyWindow(hWnd);

	System::Log("Logfile closing");
	System::CloseLog();

	return 0;
};

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	switch(uMsg) {
	case WM_MOUSEMOVE: 
		{
			MouseMoveEvent evt;
			evt.m_x=LOWORD(lParam);
			evt.m_y=HIWORD(lParam);
			message_system.Notify(&evt);
		};
		break;
	case WM_LBUTTONDOWN: 
		{
			MouseButtonEvent evt;
			evt.m_button=MB_Left;
			evt.m_state=true;
			message_system.Notify(&evt);
		};
		break;
	case WM_RBUTTONDOWN: 
		{
			MouseButtonEvent evt;
			evt.m_button=MB_Right;
			evt.m_state=true;
			message_system.Notify(&evt);
		};
		break;
	case WM_MBUTTONDOWN: 
		{
			MouseButtonEvent evt;
			evt.m_button=MB_Middle;
			evt.m_state=true;
			message_system.Notify(&evt);
		};
		break;
	case WM_LBUTTONUP: 
		{
			MouseButtonEvent evt;
			evt.m_button=MB_Left;
			evt.m_state=false;
			message_system.Notify(&evt);
		};
		break;
	case WM_RBUTTONUP: 
		{
			MouseButtonEvent evt;
			evt.m_button=MB_Right;
			evt.m_state=false;
			message_system.Notify(&evt);
		};
		break;
	case WM_MBUTTONUP: 
		{
			MouseButtonEvent evt;
			evt.m_button=MB_Middle;
			evt.m_state=false;
			message_system.Notify(&evt);
		};
		break;
	case WM_SIZE: 
		{
			ResizeEvent evt;
			evt.m_width=LOWORD(lParam);
			evt.m_height=HIWORD(lParam);
			message_system.Notify(&evt);
		};
		break;
	case WM_CLOSE: 
		{
			IEvent evt;
			evt.m_type=EVT_QUIT;
			message_system.Notify(&evt);
			::PostQuitMessage(0);
		};
		break;
	case WM_MOUSEWHEEL:
		{
			MouseWheelEvent evt;
			evt.m_delta=HIWORD(wParam)/WHEEL_DELTA;
			evt.m_x=LOWORD(lParam);
			evt.m_y=HIWORD(lParam);
			message_system.Notify(&evt);
		};
		break;
	case WM_CHAR:
	case WM_UNICHAR:
		{
			CharacterEvent evt;
			evt.m_code=(wchar_t)wParam;
			message_system.Notify(&evt);
		};
		break;
	case WM_KEYDOWN:
		{
			KeyboardEvent evt;
			evt.m_keycode=Convert((int)wParam);
			evt.m_mods=(int)lParam;
			evt.m_state=true;
			message_system.Notify(&evt);
		};
		break;
	case WM_KEYUP:
		{
			KeyboardEvent evt;
			evt.m_keycode=Convert((int)wParam);
			evt.m_mods=(int)lParam;
			evt.m_state=false;
			message_system.Notify(&evt);
		};
		break;
	default:
		return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
	};
	return 0;
};
