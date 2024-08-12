//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWin32App.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	KWin32App Class
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KDDraw.h"
#include "KMemBase.h"
#include "KStrBase.h"
#include "KWin32Wnd.h"
#include "KWin32App.h"
#include "KIme.h"
extern "C" {
    #include <shellapi.h>
}
#pragma comment(lib, "Shell32.lib")
//---------------------------------------------------------------------------
static KWin32App* m_pWin32App = NULL;
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
ENGINE_API FILE *sout = NULL;
ENGINE_API FILE *serr = NULL;

#ifndef WM_MOUSEHOVER
	#define WM_MOUSEHOVER 0x02A1
#endif

#define	MOUSE_EVENT_NONE			0
#define	MOUSE_EVENT_HAPPEND			1
#define	MOUSE_HOVER_MSG_SENT		2
#define	MOUSE_HOVER_START_TIME_MIN	3

#define WM_TRAYICON (WM_USER + 1)
#define IDM_TRAY_EXIT 1001
HINSTANCE m_hInstance;
HWND hWnd;
HMENU hMenu;
//m_uMouseHoverStartTime º¬Òå
//MOUSE_EVENT_NONE		--> Î´·¢ÉúÊó±ê»î¶¯ÊÂ¼þ
//MOUSE_EVENT_HAPPEND	--> ÓÐÊó±ê»î¶¯ÊÂ¼þ
//MOUSE_HOVER_MSG_SENT	--> ÎÞÊó±ê»î¶¯ÊÂ¼þµÄ³ÖÐøÊ±¼ä³¬¹ýÁËÉè¶¨µÄÊ±¼äÏÞÖÆ£¬ÒÑ·¢ËÍÁËWM_NCMOUSEHOVERÏûÏ¢
//ÆäËûÖµ				--> ÎÞÊó±ê»î¶¯ÊÂ¼þµÄ³ÖÐøÊ±¼äÎ´³¬¹ýÉè¶¨µÄÊ±¼äÏÞÖÆ£¬´ËÖµ±íÊ¾ÎÞÊó±ê»î¶¯µÄ¿ªÊ¼Ê±¼ä

//---------------------------------------------------------------------------
// º¯Êý:	WndProc
// ¹¦ÄÜ:	´°¿Ú»Øµ÷º¯Êý
// ²ÎÊý:	hWnd		´°¿Ú¾ä±ú
//			uMsg		ÏûÏ¢ÀàÐÍ
//			wParam		²ÎÊý1(32bit)
//			lParam		²ÎÊý2(32bit)
// ·µ»Ø:	LRESULT
//---------------------------------------------------------------------------
LRESULT CALLBACK WndProc(
						 HWND	hWnd,		// handle of window
						 UINT	uMsg,		// type of message
						 WPARAM	wParam, 	// first message parameter 32bit
						 LPARAM	lParam) 	// second message parameter 32bit
{
	return m_pWin32App->MsgProc(hWnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------
// º¯Êý:	KWin32App
// ¹¦ÄÜ:	¹ºÔìº¯Êý
// ²ÎÊý:	void
// ·µ»Ø:	void
//---------------------------------------------------------------------------
KWin32App::KWin32App()
{
	m_bActive = TRUE;
	m_bMultiGame = TRUE;
	m_bShowMouse = TRUE;
	m_uMouseHoverTimeSetting = 0;
	m_uMouseHoverStartTime = MOUSE_EVENT_NONE;
	m_uLastMouseStatus = 0;
	m_nLastMousePos = 0;
	m_uLastMouseStatus = 0;
	m_pWin32App = this;
}
//---------------------------------------------------------------------------
// º¯Êý:	Init
// ¹¦ÄÜ:	³õÊ¼»¯
// ²ÎÊý:	hInstance	ÊµÀý¾ä±ú
// ·µ»Ø:	void
//---------------------------------------------------------------------------
BOOL KWin32App::Init(HINSTANCE hInstance,char *AppName)
{
	m_hInstance = hInstance;
	g_StrCpy(m_szClass, AppName);
	strcat(m_szClass," Class");
	g_StrCpy(m_szTitle, AppName);
	strcat(m_szTitle,""); // them Title tren game
//#ifdef _DEBUG
	{
//		sout = freopen( "c:\\stdout1.txt", "a", stdout );
//		serr = freopen( "c:\\stderr1.txt", "a", stderr );
//		if (sout == NULL || serr == NULL)
//			return 0;
	}
//#endif
	if (!InitClass(hInstance))
		return FALSE;

	if (!InitWindow(hInstance))
		return FALSE;
	//MessageBox(NULL, "02", "Thong bao!!", MB_OK | MB_ICONERROR);
	return GameInit();
}
//---------------------------------------------------------------------------
// º¯Êý:	InitClass
// ¹¦ÄÜ:	³õÊ¼»¯´°¿Ú³ÌÐòÀà
// ²ÎÊý:	hInstance	ÊµÀý¾ä±ú
// ·µ»Ø:	TRUE		³É¹¦
//			FALSE		Ê§°Ü
//---------------------------------------------------------------------------
BOOL KWin32App::InitClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
	
	wc.style			= CS_DBLCLKS;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, MAKEINTATOM(SWORD_ICON));
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName 	= NULL;
	wc.lpszClassName	= m_szClass;

	return RegisterClass(&wc);
}

//trungnh icon system tray
void KWin32App::CreateTrayIcon(HWND hWnd)
{
    memset(&m_TrayIconData, 0, sizeof(m_TrayIconData));
    m_TrayIconData.cbSize = sizeof(m_TrayIconData);
    m_TrayIconData.hWnd = hWnd;
    m_TrayIconData.uID = 1;
    m_TrayIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_TrayIconData.uCallbackMessage = WM_USER + 1;   
    m_TrayIconData.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(SWORD_ICON));
    lstrcpy(m_TrayIconData.szTip, TEXT("VLTK"));

    Shell_NotifyIcon(NIM_ADD, &m_TrayIconData);

    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_SHOW, TEXT("Show"));
    AppendMenu(hMenu, MF_STRING, ID_TRAY_HIDE, TEXT("Hide"));
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));
    //MessageBox(NULL, "00.01", "Thong bao!!", MB_OK | MB_ICONERROR);
}

//trungnh icon system tray
void KWin32App::RemoveTrayIcon()
{
    Shell_NotifyIcon(NIM_DELETE, &m_TrayIconData);
}

//---------------------------------------------------------------------------
// º¯Êý:	InitWindow
// ¹¦ÄÜ:	³õÊ¼»¯´°¿Ú
// ²ÎÊý:	hInstance	ÊµÀý¾ä±ú
// ·µ»Ø:	TRUE		³É¹¦
//			FALSE		Ê§°Ü
//---------------------------------------------------------------------------
BOOL KWin32App::InitWindow(HINSTANCE hInstance)
{
	HWND hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		m_szClass,
		m_szTitle,
		//WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX,
        WS_VISIBLE | WS_CAPTION | WS_EX_TOOLWINDOW | WS_SYSMENU | WS_MINIMIZEBOX, 
		0,
		0,
		WND_INIT_WIDTH + 6,
		WND_INIT_HEIGHT + 25,
		NULL,
		NULL,
		hInstance,		NULL);

	if (!hWnd)
	{
		return FALSE;
	}


	g_SetMainHWnd(hWnd);
	g_SetDrawHWnd(hWnd);
	UpdateWindow(hWnd);
	SetFocus(hWnd);

	//trungnh icon system tray
	//MessageBox(NULL, "00", "Thong bao!!", MB_OK | MB_ICONERROR);
	CreateTrayIcon(hWnd);
	//MessageBox(NULL, "01", "Thong bao!!", MB_OK | MB_ICONERROR);
	return TRUE;
}

//---------------------------------------------------------------------------
// º¯Êý:	Run
// ¹¦ÄÜ:	Ö÷ÏûÏ¢Ñ­»·
// ²ÎÊý:	void
// ·µ»Ø:	void
//---------------------------------------------------------------------------
void KWin32App::Run()
{
	//MessageBox(NULL, "09", "Thong bao!!", MB_OK | MB_ICONERROR);
	MSG	Msg;

	while (TRUE)
	{
		if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			//if (!GetMessage(&Msg, NULL, 0, 0))
			//	break;
            if (Msg.message == WM_QUIT)
                break;

			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		else if (m_bActive || m_bMultiGame)
		{
			GenerateMsgHoverMsg();
			if (!GameLoop())
			{
				PostMessage(g_GetMainHWnd(), WM_CLOSE, 0, 0);
			}
		}
		else
		{
			WaitMessage();
		}
	}
	//MessageBox(NULL, "10 ", "Thong bao!!", MB_OK | MB_ICONERROR);
	GameExit();
}
//---------------------------------------------------------------------------
// º¯Êý:	GameInit
// ¹¦ÄÜ:	ÓÎÏ·³õÊ¼»¯
// ²ÎÊý:	void
// ·µ»Ø:	TRUE		³É¹¦
//			FALSE		Ê§°Ü
//---------------------------------------------------------------------------
BOOL KWin32App::GameInit()
{
	return TRUE;
}
//---------------------------------------------------------------------------
// º¯Êý:	GameLoop
// ¹¦ÄÜ:	ÓÎÏ·Ö÷Ñ­»·
// ²ÎÊý:	void
// ·µ»Ø:	TRUE		³É¹¦
//			FALSE		Ê§°Ü
//---------------------------------------------------------------------------
BOOL KWin32App::GameLoop()
{
	WaitMessage();
	return TRUE;
}
//---------------------------------------------------------------------------
// º¯Êý:	GameExit
// ¹¦ÄÜ:	ÓÎÏ·ÍË³ö
// ²ÎÊý:	void
// ·µ»Ø:	TRUE		³É¹¦
//			FALSE		Ê§°Ü
//---------------------------------------------------------------------------
BOOL KWin32App::GameExit()
{
	return TRUE;
}
//---------------------------------------------------------------------------
// º¯Êý:	MsgProc
// ¹¦ÄÜ:	´°¿Ú»Øµ÷º¯Êý
// ²ÎÊý:	hWnd		´°¿Ú¾ä±ú
//			uMsg		ÏûÏ¢ÀàÐÍ
//			wParam		²ÎÊý1(32bit)
//			lParam		²ÎÊý2(32bit)
// ·µ»Ø:	TRUE		³É¹¦
//			FALSE		Ê§°Ü
//---------------------------------------------------------------------------
LRESULT KWin32App::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	//trungnh icon system tray
	case WM_USER + 1:
        if (lParam == WM_RBUTTONUP)
        {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hWnd);

            //HMENU hMenu = CreatePopupMenu();
            //AppendMenu(hMenu, MF_STRING, IDM_TRAY_EXIT, TEXT("Exit"));
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
            PostMessage(hWnd, WM_NULL, 0, 0); // To remove the highlight on right click
            //DestroyMenu(hMenu);
        }
        else if (lParam == WM_LBUTTONDBLCLK)
        {
            ShowWindow(hWnd, SW_RESTORE);
            SetForegroundWindow(hWnd);
        }
        break;
    //trungnh icon system tray    
	case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_TRAY_EXIT:
            RemoveTrayIcon();
            PostQuitMessage(0);
            break;
        case ID_TRAY_HIDE:
            ShowWindow(hWnd, SW_HIDE);
            break;
        case ID_TRAY_SHOW:
            ShowWindow(hWnd, SW_RESTORE);
            SetForegroundWindow(hWnd);
            break;
        }
        break;

	case WM_CLOSE:
		if (HandleInput(uMsg, wParam, lParam))
			return 0;
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		//trungnh icon system tray
		RemoveTrayIcon();
		break;
	case WM_SETCURSOR:
		if (m_bShowMouse == FALSE && m_bActive &&
			LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(NULL);
			return TRUE;
		}
        else
        {
            // Èç¹ûÍâ²¿Ê¹ÓÃÎÒÃÇ×Ô¼ºµÄÊó±ê£¬Èç¹ûÃ»ÓÐ·µ»ØTRUE£¬
            // ÄÇÃ´¾Í»áµ¼ÖÂÊó±ê±»ÉèÖÃ»ØÀ´
            return TRUE;
        }
		break;
//	case WM_SOCKET_READ:
//        if(g_pNetClient)   
//          g_pNetClient->RecvMsg(); 
//		break;

	case WM_IME_CHAR:
		 return HandleInput(uMsg, wParam, lParam);
	case WM_INPUTLANGCHANGEREQUEST:
	case WM_IME_NOTIFY:
		if (g_pIme && 
			g_pIme->WndMsg(hWnd,uMsg,wParam,lParam))
			return 0;
		break;

	case WM_ACTIVATEAPP:
		m_bActive = (BOOL)wParam;
		if (m_bActive && g_pDirectDraw)
			g_pDirectDraw->RestoreSurface();

   	    HandleInput(uMsg, wParam, lParam);
		break;

    case WM_SYSCOMMAND:
        if (wParam == SC_KEYMENU)
        {
            return 0;
        }
	case WM_COPYDATA:
		HandleInput(uMsg, wParam, lParam);
		break;
	default:
		if (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
		{
			HandleInput(uMsg, wParam, lParam);
		}
		else if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
		{
			m_uLastMouseStatus = wParam;
			m_nLastMousePos = lParam;
			m_uMouseHoverStartTime = MOUSE_EVENT_HAPPEND;
			HandleInput(uMsg, wParam, lParam);
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//---------------------------------------------------------------------------
// º¯Êý:	ShowMouse
// ¹¦ÄÜ:	ÉèÖÃÊó±ê¿É¼û
// ²ÎÊý:	bShow		BOOL
// ·µ»Ø:	void
//---------------------------------------------------------------------------
void KWin32App::ShowMouse(BOOL bShow)
{
	m_bShowMouse = bShow;
}
//---------------------------------------------------------------------------
// º¯Êý:	SetMultiGame
// ¹¦ÄÜ:	ÉèÖÃÊÇ·ñÊÇºóÌ¨¼ÌÐøÔËÐÐµÄ¶àÈËÓÎÏ·
// ²ÎÊý:	bMulti		BOOL
// ·µ»Ø:	void
//---------------------------------------------------------------------------
void KWin32App::SetMultiGame(BOOL bMulti)
{
	m_bMultiGame = bMulti;
}

void KWin32App::SetMouseHoverTime(unsigned int nHoverTime)
{
	m_uMouseHoverTimeSetting = nHoverTime;
}

void KWin32App::GenerateMsgHoverMsg()
{
	if (m_uMouseHoverStartTime == MOUSE_HOVER_MSG_SENT)
	{
		static char cCounter = 0;
		if ((++cCounter) == 7)
		{
			cCounter = 0;
			HandleInput(WM_MOUSEHOVER, m_uLastMouseStatus, m_nLastMousePos);
		}
	}
	else if (m_uMouseHoverStartTime >= MOUSE_HOVER_START_TIME_MIN)
	{
		unsigned int	nCurrentTime = timeGetTime();
		if ((nCurrentTime - m_uMouseHoverStartTime) >= m_uMouseHoverTimeSetting)
		{
			HandleInput(WM_MOUSEHOVER, m_uLastMouseStatus, m_nLastMousePos);
			m_uMouseHoverStartTime = MOUSE_HOVER_MSG_SENT;
		}
	}
	else if (m_uMouseHoverStartTime == MOUSE_EVENT_HAPPEND)
	{
		m_uMouseHoverStartTime = MOUSE_EVENT_NONE;
	}
	else if (m_uMouseHoverTimeSetting)
	{
		//´ËÊ±(m_uMouseHoverStartTime == MOUSE_EVENT_NONE)
		m_uMouseHoverStartTime = timeGetTime();
		if (m_uMouseHoverStartTime < MOUSE_HOVER_START_TIME_MIN)
			m_uMouseHoverStartTime = MOUSE_HOVER_START_TIME_MIN;
	}
}
