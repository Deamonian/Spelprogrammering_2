// Engine_X.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MessageSystem.h"
#include "Game.h"
#include "GPIIBase.h"

MessageSystem message_system;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) 
{
	System::OpenLog();
	//System::Log("Logfile opening");

	if(!System::FileExists("../data/config.txt")) 
	{
		System::Notify("Could not find a config-file!", "Error!");
		return 1;
	}

	ConfigFile cfg("../data/config.txt");
	if(!cfg.HasKey("width"))  { cfg.Set("width",ToString(1280)); }
	if(!cfg.HasKey("height")) { cfg.Set("height",ToString(720)); }
	
	const int width = cfg.AsInt("width"), height = cfg.AsInt("height");

	// create window class for registration
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_GPIIBASE));
	wc.hIconSm = ::LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_GPIIBASE));
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"kWndCls";

	if(!::RegisterClassEx(&wc)) 
	{
		System::Notify("Could not register window class", "Error!");
		return 1;
	}

	unsigned int style = (WS_OVERLAPPEDWINDOW&~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX));
	RECT rc = { 0, 0, width, height	}; 
	::AdjustWindowRect(&rc, style, FALSE); 

	// create the main window
	HWND hWnd = ::CreateWindow(
		L"kWndCls",
		L"RenderWindow",
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL, 
		NULL,
		hInstance,
		NULL);

	if(hWnd == NULL) 
	{
		System::Notify("Could not create a window", "Error!");
		return 1;
	}

	::ShowWindow(hWnd, nCmdShow);
	::UpdateWindow(hWnd);

	Game game;

	if(!game.Initialize(&message_system, hWnd, cfg))
	{
		System::Notify("Could not initialize Application!\n"\
			"Read log.txt for more information.", "Error!");
		return -1;
	}

	MSG msg = { 0 };
	while(msg.message != WM_QUIT) 
	{
		if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else 
		{
			game.Update();
			::Sleep(2);
		}
	}
	::DestroyWindow(hWnd);

	game.Deinitialize();

	//System::Log("Logfile closing");
	System::CloseLog();

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch(uMsg) {
	case WM_MOUSEMOVE: 
		{
			MouseMoveEvent evt;
			evt.mX = LOWORD(lParam);
			evt.mY = HIWORD(lParam);
			message_system.Notify(&evt);
		};
		break;
	case WM_LBUTTONDOWN: 
		{
			MouseButtonEvent evt;
			evt.mButton = MB_Left;
			evt.mState = true;
			message_system.Notify(&evt);
		};
		break;
	case WM_RBUTTONDOWN: 
		{
			MouseButtonEvent evt;
			evt.mButton = MB_Right;
			evt.mState = true;
			message_system.Notify(&evt);
		};
		break;
	case WM_MBUTTONDOWN: 
		{
			MouseButtonEvent evt;
			evt.mButton = MB_Middle;
			evt.mState = true;
			message_system.Notify(&evt);
		};
		break;
	case WM_LBUTTONUP: 
		{
			MouseButtonEvent evt;
			evt.mButton = MB_Left;
			evt.mState = false;
			message_system.Notify(&evt);
		};
		break;
	case WM_RBUTTONUP: 
		{
			MouseButtonEvent evt;
			evt.mButton = MB_Right;
			evt.mState = false;
			message_system.Notify(&evt);
		};
		break;
	case WM_MBUTTONUP: 
		{
			MouseButtonEvent evt;
			evt.mButton = MB_Middle;
			evt.mState = false;
			message_system.Notify(&evt);
		};
		break;
	case WM_SIZE: 
		{
			ResizeEvent evt;
			evt.mWidth = LOWORD(lParam);
			evt.mHeight = HIWORD(lParam);
			message_system.Notify(&evt);
		};
		break;
	case WM_CLOSE: 
		{
			IEvent evt;
			evt.mType=EVT_QUIT;
			message_system.Notify(&evt);
			::PostQuitMessage(0);
		};
		break;
	case WM_MOUSEWHEEL:
		{
			MouseWheelEvent evt;
			evt.mDelta = HIWORD(wParam) / WHEEL_DELTA;
			evt.mX = LOWORD(lParam);
			evt.mY = HIWORD(lParam);
			message_system.Notify(&evt);
		};
		break;
	case WM_CHAR:
	case WM_UNICHAR:
		{
			CharacterEvent evt;
			evt.mCode = (wchar_t)wParam;
			message_system.Notify(&evt);
		};
		break;
	case WM_KEYDOWN:
		{
			KeyboardEvent evt;
			evt.mKeycode = Convert((int)wParam);
			evt.mMods = (int)lParam;
			evt.mMods = true;
			message_system.Notify(&evt);
		};
		break;
	case WM_KEYUP:
		{
			KeyboardEvent evt;
			evt.mKeycode = Convert((int)wParam);
			evt.mMods = (int)lParam;
			evt.mState = false;
			message_system.Notify(&evt);
		};
		break;
	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	};
	return 0;
}
