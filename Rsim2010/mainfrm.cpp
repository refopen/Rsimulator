// mainfrm.cpp : implementation of the CMainFrame class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "stdafx.h"
#include <afxpriv.h>

#include "drawcli.h"

#include "drawdoc.h"
#include "drawobj.h"
#include "cntritem.h"
#include "drawvw.h"

#include "drawobj.h"
#include "drawtool.h"
#include "mainfrm.h"
#include "robot.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
//	ON_CBN_EDITCHANGE(IDC_COMBO_HORIZON, OnEditchangeComboHorizon)
//	ON_CBN_EDITCHANGE(IDC_COMBO_VERTICAL, OnEditchangeComboVertical)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_HOLD,
	ID_INDICATOR_POINTER,	// mouse point indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// Create dialog bar
	if(!m_wndDialogBar.Create(this, IDD_ROBOT_STATUS, CBRS_LEFT, IDD_ROBOT_STATUS))
		return -1;
	 //Enable docking
	m_wndDialogBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndDialogBar);
	m_wndDialogBar.SetDlgItemText(IDC_COMBO_HORIZON, _T("40"));
	m_wndDialogBar.SetDlgItemText(IDC_COMBO_VERTICAL, _T("40"));

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG
/*
void CMainFrame::OnEditchangeComboHorizon()
{

	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;

	// Get the active MDI child window.
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();

	// Get the active view attached to the active MDI child
	// window.
	CDrawView *pView = (CDrawView *) pChild->GetActiveView();

	pView->Invalidate();
}

void CMainFrame::OnEditchangeComboVertical()
{
	CView* pView = GetActiveView();
	pView->Invalidate();
}
*/

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
// 	if( !CMDIFrameWnd::PreCreateWindow(cs) )
// 		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

 	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
 		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;

	cs.x=0; cs.y=0; cs.cx=1600; cs.cy=1200;
	return CMDIFrameWnd::PreCreateWindow(cs);
}
