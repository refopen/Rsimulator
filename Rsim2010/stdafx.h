//// This is a part of the Microsoft Foundation Classes C++ library.
//// Copyright (C) 1992-1998 Microsoft Corporation
//// All rights reserved.
////
//// This source code is only intended as a supplement to the
//// Microsoft Foundation Classes Reference and related
//// electronic documentation provided with the library.
//// See these sources for detailed information regarding the
//// Microsoft Foundation Classes product.
//
//// stdafx.h : include file for standard system include files,
////  or project specific include files that are used frequently, but
////      are changed infrequently
////
//
//#ifndef _UNICODE
//#define VC_EXTRALEAN
//#endif
//
//#ifndef ULONG_PTR 
//#define ULONG_PTR unsigned long* 
//#endif 
//
//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions (including VB)
//#include <afxole.h>         // MFC OLE 2.0 support
//#include <afxodlgs.h>       // MFC OLE 2.0 dialog support
//#include <afxcmn.h>         // MFC common control support
//
////#include "GDIRect.h"
////#include <gdiplus.h>
////using namespace Gdiplus;
////#pragma comment(lib, "gdiplus")
//
//#include "memdc.h"
//#include <cv.h>
//#include <cxcore.h>
//#include <highgui.h>
//#include <cvcam.h>
//
//extern int g_bControl;
//extern HWND g_hViewWnd;
//
//#ifdef _DEBUG
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif


// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif
#include "targetver.h"
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.


#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.

#include <afxodlgs.h>       // MFC OLE 2.0 dialog support

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "GDIRect.h"
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus")

#include "../opencv/cv.h"
#include "../opencv/cxcore.h"
#include "../opencv/highgui.h"
#include "../opencv/cvcam.h"

#pragma comment(lib, "../opencv/cxcore.lib")
#pragma comment(lib, "../opencv/highgui.lib")
#pragma comment(lib, "../opencv/cv.lib")
#pragma comment(lib, "../opencv/cvcam.lib")

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

extern int g_bControl;
extern int g_bExit;
extern HWND g_hViewWnd;

//#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
//#endif
