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


// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif
#include "targetver.h"
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.


#include <afxdisp.h>        // MFC 자동화 클래스입니다.

#include <afxodlgs.h>       // MFC OLE 2.0 dialog support

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // Windows 공용 컨트롤에 대한 MFC 지원입니다.
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
