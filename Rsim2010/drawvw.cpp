// drawvw.cpp : implementation of the CDrawView class
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

#include <math.h>
#include <list>
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CRITICAL_SECTION m_cs;

// private clipboard format (list of Draw objects)
CLIPFORMAT CDrawView::m_cfDraw = (CLIPFORMAT)
	::RegisterClipboardFormat(_T("MFC Draw Sample"));
CLIPFORMAT CDrawView::m_cfObjectDescriptor = NULL;

/////////////////////////////////////////////////////////////////////////////
// CDrawView

IMPLEMENT_DYNCREATE(CDrawView, CScrollView)

BEGIN_MESSAGE_MAP(CDrawView, CScrollView)
	//{{AFX_MSG_MAP(CDrawView)
	ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT, OnCancelEdit)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_DRAW_SELECT, OnDrawSelect)
	ON_COMMAND(ID_DRAW_ROUNDRECT, OnDrawRoundRect)
	ON_COMMAND(ID_DRAW_RECT, OnDrawRect)
	ON_COMMAND(ID_DRAW_LINE, OnDrawLine)
	ON_COMMAND(ID_DRAW_ELLIPSE, OnDrawEllipse)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ELLIPSE, OnUpdateDrawEllipse)
	ON_UPDATE_COMMAND_UI(ID_DRAW_LINE, OnUpdateDrawLine)
	ON_UPDATE_COMMAND_UI(ID_DRAW_RECT, OnUpdateDrawRect)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ROUNDRECT, OnUpdateDrawRoundRect)
	ON_UPDATE_COMMAND_UI(ID_DRAW_SELECT, OnUpdateDrawSelect)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVEBACK, OnUpdateSingleSelect)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateAnySelect)
	ON_COMMAND(ID_DRAW_POLYGON, OnDrawPolygon)
	ON_UPDATE_COMMAND_UI(ID_DRAW_POLYGON, OnUpdateDrawPolygon)
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_GRID, OnViewGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, OnUpdateViewGrid)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_OBJECT_FILLCOLOR, OnObjectFillColor)
	ON_COMMAND(ID_OBJECT_LINECOLOR, OnObjectLineColor)
	ON_COMMAND(ID_OBJECT_MOVEBACK, OnObjectMoveBack)
	ON_COMMAND(ID_OBJECT_MOVEFORWARD, OnObjectMoveForward)
	ON_COMMAND(ID_OBJECT_MOVETOBACK, OnObjectMoveToBack)
	ON_COMMAND(ID_OBJECT_MOVETOFRONT, OnObjectMoveToFront)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_SHOWOBJECTS, OnViewShowObjects)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWOBJECTS, OnUpdateViewShowObjects)
	ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, OnUpdateEditProperties)
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_DRAW_ROBOT, OnDrawRobot)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ROBOT, OnUpdateDrawRobot)
	ON_COMMAND(ID_EDIT_HOLD, OnEditHold)
	ON_WM_TIMER()
	ON_COMMAND(ID_GOAL, OnGoal)
	ON_UPDATE_COMMAND_UI(ID_GOAL, OnUpdateGoal)
	ON_COMMAND(ID_START, OnStart)
	ON_UPDATE_COMMAND_UI(ID_START, OnUpdateStart)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVEFORWARD, OnUpdateSingleSelect)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVETOBACK, OnUpdateSingleSelect)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVETOFRONT, OnUpdateSingleSelect)
	ON_UPDATE_COMMAND_UI(ID_EDIT_HOLD, OnUpdateEditHold)
	ON_MESSAGE( U_MSG_CONTROL, EventControl )	// control thread event
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_TRANSLATIONALVELOCITY_0, &CDrawView::OnTranslationalvelocity0)
	ON_COMMAND(ID_TRANSLATIONALVELOCITY_1, &CDrawView::OnTranslationalvelocity1)
	ON_COMMAND(ID_TRANSLATIONALVELOCITY_2, &CDrawView::OnTranslationalvelocity2)
	ON_COMMAND(ID_RANGE_0, &CDrawView::OnRange0)
	ON_COMMAND(ID_RANGE_1, &CDrawView::OnRange1)
	ON_COMMAND(ID_RANGE_2, &CDrawView::OnRange2)
	ON_COMMAND(ID_ANGULARVELOCITY_3, &CDrawView::OnAngularvelocity3)
	ON_COMMAND(ID_ANGULARVELOCITY_6, &CDrawView::OnAngularvelocity6)
	ON_COMMAND(ID_ANGULARVELOCITY_9, &CDrawView::OnAngularvelocity9)
	ON_COMMAND(ID_BEARING_1, &CDrawView::OnBearing1)
	ON_COMMAND(ID_BEARING_2, &CDrawView::OnBearing2)
	ON_COMMAND(ID_BEARING_3, &CDrawView::OnBearing3)
	ON_UPDATE_COMMAND_UI(ID_TRANSLATIONALVELOCITY_0, &CDrawView::OnUpdateTranslationalvelocity0)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawView construction/destruction

CDrawView::CDrawView()
: ZoomRate(1)
{
	m_bRobot = FALSE;
	m_bLbutton = FALSE;
	m_bGrid = TRUE;//FALSE;
	m_gridColor = RGB(0, 0, 128);
	m_bActive = FALSE;
	m_dwDrawMode = SRCCOPY;
	m_bSimulation = FALSE;
	m_bGoal = FALSE;
// new
	if( m_cfObjectDescriptor == NULL )
		m_cfObjectDescriptor = (CLIPFORMAT)::RegisterClipboardFormat(_T("Object Descriptor") );
	m_prevDropEffect = DROPEFFECT_NONE;
// end new

	g_bExit = TRUE;
	g_hViewWnd = this->m_hWnd;
	::InitializeCriticalSection(&m_cs);
}

CDrawView::~CDrawView()
{
	g_bExit = FALSE;
	if(m_pThread != NULL)
	{
		HANDLE hThread = m_pThread->m_hThread;
		::WaitForSingleObject(hThread, INFINITE);
		m_pThread = NULL;
	}
	::DeleteCriticalSection(&m_cs);
}

BOOL CDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
	ASSERT(cs.style & WS_CHILD);
	if (cs.lpszClass == NULL)
		cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS);
	return TRUE;
}

void CDrawView::OnActivateView(BOOL bActivate, CView* pActiveView,
	CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActiveView, pDeactiveView);

	// invalidate selections when active status changes
	if (m_bActive != bActivate)
	{
		if (bActivate)  // if becoming active update as if active
			m_bActive = bActivate;
		if (!m_selection.IsEmpty())
			OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
		m_bActive = bActivate;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDrawView drawing

void CDrawView::InvalObj(CDrawObj* pObj)
{
	CRect rect = pObj->m_position;
	DocToClient(rect);
	if (m_bActive && IsSelected(pObj))
	{
		rect.left -= 200;
		rect.top -= 200;
		rect.right += 200;
		rect.bottom += 200;
	}
	rect.InflateRect(1, 1); // handles CDrawOleObj objects

	InvalidateRect(rect, FALSE);
}

void CDrawView::OnUpdate(CView* , LPARAM lHint, CObject* pHint)
{
	switch (lHint)
	{
	case HINT_UPDATE_WINDOW:    // redraw entire window
		Invalidate(FALSE);
		break;

	case HINT_UPDATE_DRAWOBJ:   // a single object has changed
		InvalObj((CDrawObj*)pHint);
		break;

	case HINT_UPDATE_SELECTION: // an entire selection has changed
		{
			CDrawObjList* pList = pHint != NULL ?
				(CDrawObjList*)pHint : &m_selection;
			POSITION pos = pList->GetHeadPosition();
			while (pos != NULL)
				InvalObj(pList->GetNext(pos));
		}
		break;

	case HINT_DELETE_SELECTION: // an entire selection has been removed
		if (pHint != &m_selection)
		{
			CDrawObjList* pList = (CDrawObjList*)pHint;
			POSITION pos = pList->GetHeadPosition();
			while (pos != NULL)
			{
				CDrawObj* pObj = pList->GetNext(pos);
				InvalObj(pObj);
				Remove(pObj);   // remove it from this view's selection
			}
		}
		break;

	case HINT_UPDATE_OLE_ITEMS:
		{
			CDrawDoc* pDoc = GetDocument();
			POSITION pos = pDoc->GetObjects()->GetHeadPosition();
			while (pos != NULL)
			{
				CDrawObj* pObj = pDoc->GetObjects()->GetNext(pos);
				if (pObj->IsKindOf(RUNTIME_CLASS(CDrawOleObj)))
					InvalObj(pObj);
			}
		}
		break;

	default:
		ASSERT(FALSE);
		break;
	}
}

void CDrawView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CScrollView::OnPrepareDC(pDC, pInfo);

	// mapping mode is MM_ANISOTROPIC
	// these extents setup a mode similar to MM_LOENGLISH
	// MM_LOENGLISH is in .01 physical inches
	// these extents provide .01 logical inches

	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),
		pDC->GetDeviceCaps(LOGPIXELSY));
	pDC->SetWindowExt(100, -100);

	// set the origin of the coordinate system to the center of the page
	CPoint ptOrg;
	ptOrg.x = GetDocument()->GetSize().cx / 2;
	ptOrg.y = GetDocument()->GetSize().cy / 2;

	// ptOrg is in logical coordinates
	pDC->OffsetWindowOrg(-ptOrg.x,ptOrg.y);
}

BOOL CDrawView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	// do the scroll
	if (!CScrollView::OnScrollBy(sizeScroll, bDoScroll))
		return FALSE;

	// update the position of any in-place active item
	if (bDoScroll)
	{
		UpdateActiveItem();
		UpdateWindow();
	}
	return TRUE;
}

void CDrawView::OnDraw(CDC* pDC)
{
	::EnterCriticalSection(&m_cs);
	CDrawDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CDC dc;
	CDC* pDrawDC = pDC;
	CBitmap bitmap;
	CBitmap* pOldBitmap;

	// only paint the rect that needs repainting
	CRect client;
	pDC->GetClipBox(client);
	CRect rect = client;
	DocToClient(rect);

	if (!pDC->IsPrinting())
	{
		// draw to offscreen bitmap for fast looking repaints
		if (dc.CreateCompatibleDC(pDC))
		{
			if (bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()))
			{
				OnPrepareDC(&dc, NULL);
				pDrawDC = &dc;

				// offset origin more because bitmap is just piece of the whole drawing
				dc.OffsetViewportOrg(-rect.left, -rect.top);
				pOldBitmap = dc.SelectObject(&bitmap);
				dc.SetBrushOrg(rect.left % 8, rect.top % 8);

				// might as well clip to the same rectangle
				dc.IntersectClipRect(client);
			}
		}
	}

	// paint background
	CBrush brush;
	if (!brush.CreateSolidBrush(pDoc->GetPaperColor()))
		return;

	brush.UnrealizeObject();
	pDrawDC->FillRect(client, &brush);

	/* draw the grid line */
	if (!pDC->IsPrinting() /*&& m_bGrid*/ && m_bLbutton == false && m_bRobot/* && m_bSimulation*/)
	{
//		Draw3d();

		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
		CPoint p;
		if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE1)) 
		{
			p = DrawGrid(pDrawDC);
			DrawDTmap(p);
		}
		else if(	pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE2)
				||	pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE3)
				||	pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_PLANNING_TYPE4))
		{
			if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_CHECK_SONAR))	
			{
				m_Control->SonarMeasurement(this);
				DrawSonar(pDrawDC);
			}
			if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_CHECK_LASER))
			{
				m_Control->LaserMeasurement(this);
				DrawLaser(pDrawDC);
			}
			if(g_bControl)
			{
				DrawRobotLoci(pDrawDC);
				DrawEllipse(pDrawDC);
				//DrawLandmarks(pDrawDC);
				//DrawSigmaPoints(pDrawDC);
			}
		}
		
		CDrawRect *robot = pDoc->m_robot;
		double x, y;
		x = robot->CenterPtF().X;
		y = robot->CenterPtF().Y;
		OC.UpdateMap(m_Control->s, x, y, DEG2RAD(m_Control->angle));
		OC.ViewMap();
	}

	pDoc->Draw(pDrawDC, this);

	if (pDrawDC != pDC)
	{
		pDC->SetViewportOrg(0, 0);
		pDC->SetWindowOrg(0,0);
		pDC->SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0,0);
		dc.SetMapMode(MM_TEXT);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dc, 0, 0, m_dwDrawMode);
		//pDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dc, 0, 0, rect.Width()*ZoomRate, rect.Height()*ZoomRate, m_dwDrawMode);
		dc.SelectObject(pOldBitmap);
	}

	//CString strTemp;

	///* attach a tag displaying the sizes of the selected object */
	//POSITION pos = m_selection.GetHeadPosition();
	//while (pos != NULL)
	//{
	//	CDrawObj* pObj = m_selection.GetNext(pos);
	//	CRect position = pObj->m_position;
	//	position.NormalizeRect();
	//	DocToClient(position);
	//	int margin=0;

	//	if(pObj->IsKindOf(RUNTIME_CLASS(CDrawRect))){
	//		CDrawRect* pRect = (CDrawRect *)pObj;
	//		if(pRect->m_nShape == CDrawRect::goal){
	//			strTemp.Format("goal");
	//			margin = 10;
	//		}
	//		else
	//			strTemp.Format("width : %d height : %d", position.Width(), position.Height());
	//	}
	//	else
	//		strTemp.Format("width : %d height : %d", position.Width(), position.Height());

	//	pDC->TextOut(position.TopLeft().x, position.TopLeft().y+margin, strTemp);
	//}

	///* display current screen status on the status pannel */
	//CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	//if(m_dwDrawMode == SRCCOPY)
	//	pFrame->m_wndStatusBar.SetPaneText(1, "released screen");
	//else
	//	pFrame->m_wndStatusBar.SetPaneText(1, "held screen");
	::LeaveCriticalSection(&m_cs);
}

void CDrawView::Remove(CDrawObj* pObj)
{
	POSITION pos = m_selection.Find(pObj);
	if (pos != NULL)
		m_selection.RemoveAt(pos);
}

void CDrawView::PasteNative(COleDataObject& dataObject)
{
	// get file refering to clipboard data
	CFile* pFile = dataObject.GetFileData(m_cfDraw);
	if (pFile == NULL)
		return;

	// connect the file to the archive
	CArchive ar(pFile, CArchive::load);
	TRY
	{
		ar.m_pDocument = GetDocument(); // set back-pointer in archive

		// read the selection
		m_selection.Serialize(ar);
	}
	CATCH_ALL(e)
	{
		ar.Close();
		delete pFile;
		THROW_LAST();
	}
	END_CATCH_ALL

	ar.Close();
	delete pFile;
}

void CDrawView::PasteEmbedded(COleDataObject& dataObject, CPoint point )
{
	BeginWaitCursor();

	// paste embedded
	CDrawOleObj* pObj = new CDrawOleObj(GetInitialPosition());
	ASSERT_VALID(pObj);
	CDrawItem* pItem = new CDrawItem(GetDocument(), pObj);
	ASSERT_VALID(pItem);
	pObj->m_pClientItem = pItem;

	TRY
	{
		if (!pItem->CreateFromData(&dataObject) &&
			!pItem->CreateStaticFromData(&dataObject))
		{
			AfxThrowMemoryException();      // any exception will do
		}

		// add the object to the document
		GetDocument()->Add(pObj);
		m_selection.AddTail(pObj);
		ClientToDoc( point );
		pObj->MoveTo( CRect( point, pObj->m_extent ), this );

		// try to get initial presentation data
		pItem->UpdateLink();
		pItem->UpdateExtent();
	}
	CATCH_ALL(e)
	{
		// clean up item
		pItem->Delete();
		pObj->m_pClientItem = NULL;
		GetDocument()->Remove(pObj);
		pObj->Remove();

		AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH_ALL

	EndWaitCursor();
}

/* Make the map table based on the path planing using rectangluar cell method */
CPoint CDrawView::DrawGrid(CDC* pDC)
{
	CDrawDoc* pDoc = GetDocument();

	COLORREF oldBkColor = pDC->SetBkColor(pDoc->GetPaperColor());

	/* 
		페이지의 사이즈를 document class로 부터 받아온다. 프린터 설정에서 정의된 페이지 사이즈에 의존
		여기서는 수정을 통해 고정된 값으로 설정하였다.
		11.5" x 6.8"
	*/
	CRect rect;
	rect.left = -pDoc->GetSize().cx / 2;
	rect.top = -pDoc->GetSize().cy / 2;
	rect.right = rect.left + pDoc->GetSize().cx;
	rect.bottom = rect.top + pDoc->GetSize().cy;

	rect.left = -500;
	rect.right = 500;
	rect.top = -500;
	rect.bottom = 500;

	// Center lines
	CPen penDash;
	CBrush brush, *pOldBrush;
	brush.CreateSolidBrush(RGB(255, 0, 0));
	pOldBrush = pDC->SelectObject(&brush);
	penDash.CreatePen(PS_DASH, 1, m_gridColor);
	CPen* pOldPen = pDC->SelectObject(&penDash);

	pDC->MoveTo(0, rect.top);
	pDC->LineTo(0, rect.bottom);
	pDC->MoveTo(rect.left, 0);
	pDC->LineTo(rect.right, 0);

	// Major unit lines
	CPen penDot;
	penDot.CreatePen(PS_DOT, 1, m_gridColor);
	pDC->SelectObject(&penDot);

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CPoint ptGridsize;
	ptGridsize.x = pFrame->m_wndDialogBar.GetDlgItemInt(IDC_COMBO_HORIZON);
	ptGridsize.y = pFrame->m_wndDialogBar.GetDlgItemInt(IDC_COMBO_VERTICAL);

	ptGridsize.x = (rect.right - rect.left) / ptGridsize.x;
	ptGridsize.y = (rect.bottom - rect.top) / ptGridsize.y;

	pDoc->m_ptGridsize = ptGridsize;

	int x, y;

	if(ptGridsize.x >= 10 && ptGridsize.y >= 10)
	{
		/* 정의된 크기의 셀들을 그림. */
		for (x = rect.left; x < rect.right; x += ptGridsize.x)
		{
			if (x != 0)
			{
				pDC->MoveTo(x, rect.top);
				pDC->LineTo(x, rect.bottom);
			}
		}

		for (y = rect.top; y < rect.bottom; y += ptGridsize.y)
		{
			if (y != 0)
			{
				pDC->MoveTo(rect.left, y);
				pDC->LineTo(rect.right, y);
			}
		}
	}

	// Outlines
	CPen penSolid;
	penSolid.CreatePen(PS_SOLID, 1, m_gridColor);
	pDC->SelectObject(&penSolid);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right, rect.top);
	pDC->LineTo(rect.right, rect.bottom);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->LineTo(rect.left, rect.top);

	CString strDistance;
	CRect rectGoal;
	ptGoal.x = 0; ptGoal.y = 0;
	ptRobot.x = 0; ptRobot.y = 0;

	/* 998로 모든 맵을 초기화, 장애물의 가중치인 999와 구분 */
	/* 화면 가장자리에서 로봇이 벽으로 질주하는 현상을 방지 */
	int i, j;
	for(i=0; i<WIDTH; i++)	for(j=0; j<HEIGHT; j++)	m_Control->m_nTable[i][j] = 998;
	
//	TRACE("%d radio\n", (pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_MAPTYPE1)));
// 	if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_MAPTYPE1))
//	{
		/* goal로 정의된 object가 포함된 셀을 구별하기 위해 덧칠. */	
		for (x = rect.left, i = 0; x < rect.right; i++, x += ptGridsize.x)
		{
			for (y = rect.top, j = 0; y < rect.bottom; j++, y += ptGridsize.y)
			{
				/* x, y position은 셀의 좌상단 */
				CRect rectUnit(x, y, x+ptGridsize.x, y-ptGridsize.y);
				rectUnit.NormalizeRect();

				if(rectUnit.PtInRect(m_Control->goal))
				{
					pDC->Rectangle(rectUnit);
					rectGoal = rectUnit;
					ptGoal.x = i, ptGoal.y = j;
				}

				/* goal과 robot 개체를 제외한 다른 개체(obstacle)와 겹치는 cell들은 999의 가중치를 부여 */
				POSITION pos = GetDocument()->m_objects.GetHeadPosition();
				while (pos != NULL)
				{
					CDrawObj* pObj = GetDocument()->m_objects.GetNext(pos);
					CDrawRect* pRect = (CDrawRect *)pObj;
					if(pRect->m_nShape == CDrawRect::robot)
					{		
						if(pObj->Intersects(rectUnit))
						{
							ptRobot.x = i;
							ptRobot.y = j;
						}
						continue;
					}
					if(pRect->m_nShape == CDrawRect::goal)
						continue;
					if(	pObj->Intersects(rectUnit) )
					{
						m_Control->m_nTable[i][j] = 999;
						break;
					}
				}
			}
		}
//	}
	if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_MAPTYPE2))
	{
		for(i=1; i<WIDTH-1; i++)
		{
			for(j=1; j<HEIGHT-1; j++)
			{
				if(OC.Rmap[j*WIDTH+i] == 255)	m_Control->m_nTable[i][j] = 999;
				else	m_Control->m_nTable[i][j] = 998;
			}
		}
	}

	CPoint p(i, j);

	m_Control->DistanceTransform(i, j, ptRobot.x, ptRobot.y, ptGoal.x, ptGoal.y);

	if(ptGridsize.x >= 10 && ptGridsize.y >= 10)
	{
		for (x = rect.left, i = 0; x < rect.right; i++, x += ptGridsize.x){
			for (y = rect.top, j = 0; y < rect.bottom; j++, y += ptGridsize.y){
				/* 셀의 중앙에 거리값을 표현 */
	//			if(m_Control->m_nTable[i][j]>999) m_Control->m_nTable[i][j] = 999;
				strDistance.Format("%d", m_Control->m_nTable[i][j]);
				//pDC->TextOut(x+1, y-1, strDistance);
			}
		}
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pDC->SetBkColor(oldBkColor);

	return p;
}

void CDrawView::OnInitialUpdate()
{
	CSize size = GetDocument()->GetSize();
	CClientDC dc(NULL);
	size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), 100);
	size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), 100);
	SetScrollSizes(MM_TEXT, size);

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	pFrame->m_wndDialogBar.CheckRadioButton(IDC_RADIO_MAPTYPE1, IDC_RADIO_MAPTYPE2, IDC_RADIO_MAPTYPE1);
	pFrame->m_wndDialogBar.CheckRadioButton(IDC_RADIO_PLANNING_TYPE1, IDC_RADIO_PLANNING_TYPE4, IDC_RADIO_PLANNING_TYPE4);

	pFrame->m_wndDialogBar.CheckDlgButton(IDC_CHECK_SONAR, TRUE);
	//pFrame->m_wndDialogBar.CheckDlgButton(IDC_CHECK_LASER, TRUE);

	m_Control = new CRobotControl();
	m_Control->m_pUKF = new CUKF(GetDocument(), this);
	m_Control->InitialUpdate(this);
	if(!GetDocument()->goalVector.empty())
	{
		if(GetDocument()->goalVector.size() != 0)
		{
			m_bGoal = TRUE;
			m_Control->goal.x = GetDocument()->goalVector[0].x;
			m_Control->goal.y = GetDocument()->goalVector[0].y;
		}
	}
	if(GetDocument()->m_bRobotActivated)	
	{
//		m_Control->InitialUpdate(this);
		m_bRobot = TRUE;
	}
	m_pThread = AfxBeginThread(ThreadControl, (LPVOID)this);
}

void CDrawView::SetPageSize(CSize size)
{
	CClientDC dc(NULL);
	size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), 100);
	size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), 100);
	SetScrollSizes(MM_TEXT, size);
	GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_WINDOW, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawView printing

BOOL CDrawView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDrawView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CScrollView::OnBeginPrinting(pDC,pInfo);

	// check page size -- user could have gone into print setup
	// from print dialog and changed paper or orientation
	GetDocument()->ComputePageSize();
}

void CDrawView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands

BOOL CDrawView::IsSelected(const CObject* pDocItem) const
{
	CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
	if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
		pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
	return m_selection.Find(pDrawObj) != NULL;
}

void CDrawView::OnInsertObject()
{
	// Invoke the standard Insert Object dialog box to obtain information
	//  for new CDrawItem object.
	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	BeginWaitCursor();

	// First create the C++ object
	CDrawOleObj* pObj = new CDrawOleObj(GetInitialPosition());
	ASSERT_VALID(pObj);
	CDrawItem* pItem = new CDrawItem(GetDocument(), pObj);
	ASSERT_VALID(pItem);
	pObj->m_pClientItem = pItem;

	// Now create the OLE object/item
	TRY
	{
		if (!dlg.CreateItem(pObj->m_pClientItem))
			AfxThrowMemoryException();

		// add the object to the document
		GetDocument()->Add(pObj);

		// try to get initial presentation data
		pItem->UpdateLink();
		pItem->UpdateExtent();

		// if insert new object -- initially show the object
		if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);
	}
	CATCH_ALL(e)
	{
		// clean up item
		pItem->Delete();
		pObj->m_pClientItem = NULL;
		GetDocument()->Remove(pObj);
		pObj->Remove();

		AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH_ALL

	EndWaitCursor();
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.
void CDrawView::OnCancelEdit()
{
	// deactivate any in-place active item on this view!
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		// if we found one, deactivate it
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);

	// escape also brings us back into select mode
	ReleaseCapture();

	CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
	if (pTool != NULL)
		pTool->OnCancel();

	CDrawTool::c_drawShape = selection;
}

void CDrawView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// need to set focus to this item if it is in the same view
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();
			return;
		}
	}

	CScrollView::OnSetFocus(pOldWnd);
}

CRect CDrawView::GetInitialPosition()
{
	CRect rect(10, 10, 10, 10);
	ClientToDoc(rect);
	return rect;
}

void CDrawView::ClientToDoc(CPoint& point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.DPtoLP(&point);
}

void CDrawView::ClientToDoc(CRect& rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.DPtoLP(rect);
	ASSERT(rect.left <= rect.right);
	ASSERT(rect.bottom <= rect.top);
}

void CDrawView::DocToClient(CPoint& point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.LPtoDP(&point);
}

void CDrawView::DocToClient(CRect& rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.LPtoDP(rect);
	rect.NormalizeRect();
}

void CDrawView::Select(CDrawObj* pObj, BOOL bAdd)
{
	if (!bAdd)
	{
		OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
		m_selection.RemoveAll();
	}

	if (pObj == NULL || IsSelected(pObj))
		return;

	m_selection.AddTail(pObj);
	InvalObj(pObj);
}

// rect is in device coordinates
void CDrawView::SelectWithinRect(CRect rect, BOOL bAdd)
{
	if (!bAdd)
		Select(NULL);

	ClientToDoc(rect);

	CDrawObjList* pObList = GetDocument()->GetObjects();
	POSITION posObj = pObList->GetHeadPosition();
	while (posObj != NULL)
	{
		CDrawObj* pObj = pObList->GetNext(posObj);
		if (pObj->Intersects(rect))
			Select(pObj, TRUE);
	}
}

void CDrawView::Deselect(CDrawObj* pObj)
{
	POSITION pos = m_selection.Find(pObj);
	if (pos != NULL)
	{
		InvalObj(pObj);
		m_selection.RemoveAt(pos);
	}
}

void CDrawView::CloneSelection()
{
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = m_selection.GetNext(pos);
		pObj->Clone(pObj->m_pDocument);
			// copies object and adds it to the document
	}
}

void CDrawView::UpdateActiveItem()
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// this will update the item rectangles by calling
		//  OnGetPosRect & OnGetClipRect.
		pActiveItem->SetItemRects();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDrawView message handlers

void CDrawView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLbutton = true;

	if (!m_bActive)
		return;
	CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
	if (pTool != NULL)
		pTool->OnLButtonDown(this, nFlags, point);
}

void CDrawView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLbutton = false;

	if (!m_bActive)
		return;
	CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
	if (pTool != NULL)
		pTool->OnLButtonUp(this, nFlags, point);
}

void CDrawView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bActive)
		return;
	CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
	if (pTool != NULL)
		pTool->OnMouseMove(this, nFlags, point);

	/* display current mouse pointer on the status pannel */
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CString strPoint;
	strPoint.Format("Mouse pointer : %d, %d", point.x, point.y);
	pFrame->m_wndStatusBar.SetPaneText(2, strPoint);
}

void CDrawView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_bActive)
		return;
	CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
	if (pTool != NULL)
		pTool->OnLButtonDblClk(this, nFlags, point);
}

void CDrawView::OnDestroy()
{
	CScrollView::OnDestroy();

	// deactivate the inplace active item on this view
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
	{
		pActiveItem->Deactivate();
		ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
	}

	cvDestroyAllWindows();
}

void CDrawView::OnDrawSelect()
{
	CDrawTool::c_drawShape = selection;
}

void CDrawView::OnDrawRoundRect()
{
	CDrawTool::c_drawShape = roundRect;
}

void CDrawView::OnDrawRect()
{
	CDrawTool::c_drawShape = rect;
}

void CDrawView::OnDrawLine()
{
	CDrawTool::c_drawShape = line;
}

void CDrawView::OnDrawEllipse()
{
	CDrawTool::c_drawShape = ellipse;
}

void CDrawView::OnDrawPolygon()
{
	CDrawTool::c_drawShape = poly;
}

void CDrawView::OnUpdateDrawEllipse(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(CDrawTool::c_drawShape == ellipse);
}

void CDrawView::OnUpdateDrawLine(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(CDrawTool::c_drawShape == line);
}

void CDrawView::OnUpdateDrawRect(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(CDrawTool::c_drawShape == rect);
}

void CDrawView::OnUpdateDrawRoundRect(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(CDrawTool::c_drawShape == roundRect);
}

void CDrawView::OnUpdateDrawSelect(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(CDrawTool::c_drawShape == selection);
}

void CDrawView::OnUpdateSingleSelect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selection.GetCount() == 1);
}

void CDrawView::OnEditSelectAll()
{
	CDrawObjList* pObList = GetDocument()->GetObjects();
	POSITION pos = pObList->GetHeadPosition();
	while (pos != NULL)
		Select(pObList->GetNext(pos), TRUE);
}

void CDrawView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GetObjects()->GetCount() != 0);
}

void CDrawView::OnEditClear()
{
	// update all the views before the selection goes away
	GetDocument()->UpdateAllViews(NULL, HINT_DELETE_SELECTION, &m_selection);
	OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);

	// now remove the selection from the document
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj == GetDocument()->m_robot){
			GetDocument()->m_bRobotActivated = FALSE;
			m_dwDrawMode = SRCCOPY;
		}
		GetDocument()->Remove(pObj);
		pObj->Remove();
	}
	m_selection.RemoveAll();
}

void CDrawView::OnUpdateAnySelect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_selection.IsEmpty());
}

void CDrawView::OnUpdateDrawPolygon(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(CDrawTool::c_drawShape == poly);
}

void CDrawView::OnSize(UINT nType, int cx, int cy)
{
	m_dwDrawMode = SRCCOPY;
	CScrollView::OnSize(nType, cx, cy);
	UpdateActiveItem();
}

void CDrawView::OnViewGrid()
{
	m_bGrid = !m_bGrid;
	Invalidate(FALSE);
}

void CDrawView::OnUpdateViewGrid(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bGrid);
}

BOOL CDrawView::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CDrawView::OnObjectFillColor()
{
	CColorDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	COLORREF color = dlg.GetColor();

	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = m_selection.GetNext(pos);
		pObj->SetFillColor(color);
	}
}

void CDrawView::OnObjectLineColor()
{
	CColorDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	COLORREF color = dlg.GetColor();

	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = m_selection.GetNext(pos);
		pObj->SetLineColor(color);
	}
}

void CDrawView::OnObjectMoveBack()
{
	CDrawDoc* pDoc = GetDocument();
	CDrawObj* pObj = m_selection.GetHead();
	CDrawObjList* pObjects = pDoc->GetObjects();
	POSITION pos = pObjects->Find(pObj);
	ASSERT(pos != NULL);
	if (pos != pObjects->GetHeadPosition())
	{
		POSITION posPrev = pos;
		pObjects->GetPrev(posPrev);
		pObjects->RemoveAt(pos);
		pObjects->InsertBefore(posPrev, pObj);
		InvalObj(pObj);
	}
}

void CDrawView::OnObjectMoveForward()
{
	CDrawDoc* pDoc = GetDocument();
	CDrawObj* pObj = m_selection.GetHead();
	CDrawObjList* pObjects = pDoc->GetObjects();
	POSITION pos = pObjects->Find(pObj);
	ASSERT(pos != NULL);
	if (pos != pObjects->GetTailPosition())
	{
		POSITION posNext = pos;
		pObjects->GetNext(posNext);
		pObjects->RemoveAt(pos);
		pObjects->InsertAfter(posNext, pObj);
		InvalObj(pObj);
	}
}

void CDrawView::OnObjectMoveToBack()
{
	CDrawDoc* pDoc = GetDocument();
	CDrawObj* pObj = m_selection.GetHead();
	CDrawObjList* pObjects = pDoc->GetObjects();
	POSITION pos = pObjects->Find(pObj);
	ASSERT(pos != NULL);
	pObjects->RemoveAt(pos);
	pObjects->AddHead(pObj);
	InvalObj(pObj);
}

void CDrawView::OnObjectMoveToFront()
{
	CDrawDoc* pDoc = GetDocument();
	CDrawObj* pObj = m_selection.GetHead();
	CDrawObjList* pObjects = pDoc->GetObjects();
	POSITION pos = pObjects->Find(pObj);
	ASSERT(pos != NULL);
	pObjects->RemoveAt(pos);
	pObjects->AddTail(pObj);
	InvalObj(pObj);
}

void CDrawView::OnEditCopy()
{
	ASSERT_VALID(this);
	ASSERT(m_cfDraw != NULL);

	// Create a shared file and associate a CArchive with it
	CSharedFile file;
	CArchive ar(&file, CArchive::store);

	// Serialize selected objects to the archive
	m_selection.Serialize(ar);
	ar.Close();

	COleDataSource* pDataSource = NULL;
	TRY
	{
		pDataSource = new COleDataSource;
		// put on local format instead of or in addation to
		pDataSource->CacheGlobalData(m_cfDraw, file.Detach());

		// if only one item and it is a COleClientItem then also
		// paste in that format
		CDrawObj* pDrawObj = m_selection.GetHead();
		if (m_selection.GetCount() == 1 &&
			pDrawObj->IsKindOf(RUNTIME_CLASS(CDrawOleObj)))
		{
			CDrawOleObj* pDrawOle = (CDrawOleObj*)pDrawObj;
			pDrawOle->m_pClientItem->GetClipboardData(pDataSource, FALSE);
		}
		pDataSource->SetClipboard();
	}
	CATCH_ALL(e)
	{
		delete pDataSource;
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CDrawView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_selection.IsEmpty());
}

void CDrawView::OnEditCut()
{
	OnEditCopy();
	OnEditClear();
}

void CDrawView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_selection.IsEmpty());
}

void CDrawView::OnEditPaste()
{
	COleDataObject dataObject;
	dataObject.AttachClipboard();

	// invalidate current selection since it will be deselected
	OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
	m_selection.RemoveAll();
	if (dataObject.IsDataAvailable(m_cfDraw))
	{
		PasteNative(dataObject);

		// now add all items in m_selection to document
		POSITION pos = m_selection.GetHeadPosition();
		while (pos != NULL)
			GetDocument()->Add(m_selection.GetNext(pos));
	}
	else
		PasteEmbedded(dataObject, GetInitialPosition().TopLeft() );

	GetDocument()->SetModifiedFlag();

	// invalidate new pasted stuff
	GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_SELECTION, &m_selection);
}

void CDrawView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	// determine if private or standard OLE formats are on the clipboard
	COleDataObject dataObject;
	BOOL bEnable = dataObject.AttachClipboard() &&
		(dataObject.IsDataAvailable(m_cfDraw) ||
		 COleClientItem::CanCreateFromData(&dataObject));

	// enable command based on availability
	pCmdUI->Enable(bEnable);
}

void CDrawView::OnViewShowObjects()
{
	CDrawOleObj::c_bShowItems = !CDrawOleObj::c_bShowItems;
	GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_OLE_ITEMS, NULL);
}

void CDrawView::OnUpdateViewShowObjects(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CDrawOleObj::c_bShowItems);
}

void CDrawView::OnEditProperties()
{
	if (m_selection.GetCount() == 1 && CDrawTool::c_drawShape == selection)
	{
		CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
		ASSERT(pTool != NULL);
		pTool->OnEditProperties(this);
	}
}

void CDrawView::OnUpdateEditProperties(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selection.GetCount() == 1 &&
				   CDrawTool::c_drawShape == selection);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawView diagnostics

#ifdef _DEBUG
void CDrawView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CDrawView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// new
// support for drag/drop

int CDrawView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// register drop target
	if( m_dropTarget.Register( this ) )
		return 0;
	else
		return -1;
}

BOOL CDrawView::GetObjectInfo(COleDataObject* pDataObject,
	CSize* pSize, CSize* pOffset)
{
	ASSERT(pSize != NULL);

	// get object descriptor data
	HGLOBAL hObjDesc = pDataObject->GetGlobalData(m_cfObjectDescriptor);
	if (hObjDesc == NULL)
	{
		if (pOffset != NULL)
			*pOffset = CSize(0, 0); // fill in defaults instead
		*pSize = CSize(0, 0);
		return FALSE;
	}
	ASSERT(hObjDesc != NULL);

	// otherwise, got CF_OBJECTDESCRIPTOR ok.  Lock it down and extract size.
	LPOBJECTDESCRIPTOR pObjDesc = (LPOBJECTDESCRIPTOR)GlobalLock(hObjDesc);
	ASSERT(pObjDesc != NULL);
	pSize->cx = (int)pObjDesc->sizel.cx;
	pSize->cy = (int)pObjDesc->sizel.cy;
	if (pOffset != NULL)
	{
		pOffset->cx = (int)pObjDesc->pointl.x;
		pOffset->cy = (int)pObjDesc->pointl.y;
	}
	GlobalUnlock(hObjDesc);
	GlobalFree(hObjDesc);

	// successfully retrieved pSize & pOffset info
	return TRUE;
}

DROPEFFECT CDrawView::OnDragEnter(COleDataObject* pDataObject,
	DWORD grfKeyState, CPoint point)
{
	ASSERT(m_prevDropEffect == DROPEFFECT_NONE);
	m_bDragDataAcceptable = FALSE;
	if (!COleClientItem::CanCreateFromData(pDataObject))
		return DROPEFFECT_NONE;

	m_bDragDataAcceptable = TRUE;
	GetObjectInfo(pDataObject, &m_dragSize, &m_dragOffset);
	CClientDC dc(NULL);
	dc.HIMETRICtoDP(&m_dragSize);
	dc.HIMETRICtoDP(&m_dragOffset);

	return OnDragOver(pDataObject, grfKeyState, point);
}

DROPEFFECT CDrawView::OnDragOver(COleDataObject*,
	DWORD grfKeyState, CPoint point)
{
	if(m_bDragDataAcceptable == FALSE)
		return DROPEFFECT_NONE;

	point -= m_dragOffset;  // adjust target rect by original cursor offset

	// check for point outside logical area -- i.e. in hatched region
	// GetTotalSize() returns the size passed to SetScrollSizes
	CRect rectScroll(CPoint(0, 0), GetTotalSize());

	CRect rectItem(point,m_dragSize);
	rectItem.OffsetRect(GetDeviceScrollPosition());

	DROPEFFECT de = DROPEFFECT_NONE;
	CRect rectTemp;
	if (rectTemp.IntersectRect(rectScroll, rectItem))
	{
		// check for force link
		if ((grfKeyState & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT))
			de = DROPEFFECT_NONE; // DRAWCLI isn't a linking container
		// check for force copy
		else if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
			de = DROPEFFECT_COPY;
		// check for force move
		else if ((grfKeyState & MK_ALT) == MK_ALT)
			de = DROPEFFECT_MOVE;
		// default -- recommended action is move
		else
			de = DROPEFFECT_MOVE;
	}

	if (point == m_dragPoint)
		return de;

	// otherwise, cursor has moved -- need to update the drag feedback
	CClientDC dc(this);
	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
		// erase previous focus rect
		dc.DrawFocusRect(CRect(m_dragPoint, m_dragSize));
	}
	m_prevDropEffect = de;
	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
		m_dragPoint = point;
		dc.DrawFocusRect(CRect(point, m_dragSize));
	}
	return de;
}

BOOL CDrawView::OnDrop(COleDataObject* pDataObject,
	DROPEFFECT /*dropEffect*/, CPoint point)
{
	ASSERT_VALID(this);

	// clean up focus rect
	OnDragLeave();

	// offset point as appropriate for dragging
	GetObjectInfo(pDataObject, &m_dragSize, &m_dragOffset);
	CClientDC dc(NULL);
	dc.HIMETRICtoDP(&m_dragSize);
	dc.HIMETRICtoDP(&m_dragOffset);
	point -= m_dragOffset;

	// invalidate current selection since it will be deselected
	OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
	m_selection.RemoveAll();
	if (m_bDragDataAcceptable)
		PasteEmbedded(*pDataObject, point);

	// update the document and views
	GetDocument()->SetModifiedFlag();
	GetDocument()->UpdateAllViews(NULL, 0, NULL);      // including this view

	return TRUE;
}

void CDrawView::OnDragLeave()
{
	CClientDC dc(this);
	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
		dc.DrawFocusRect(CRect(m_dragPoint,m_dragSize)); // erase previous focus rect
		m_prevDropEffect = DROPEFFECT_NONE;
	}
}


void CDrawView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// make sure window is active
	GetParentFrame()->ActivateFrame();

	CPoint local = point;
	ScreenToClient(&local);
	ClientToDoc(local);

	CDrawObj* pObj;
	pObj = GetDocument()->ObjectAt(local);
	if(pObj != NULL)
	{
		if(!IsSelected(pObj))
			Select( pObj, FALSE );          // reselect item if appropriate
		UpdateWindow();

		CMenu menu;
		if (menu.LoadMenu(ID_POPUP_MENU))
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);

			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
								   point.x, point.y,
								   AfxGetMainWnd()); // route commands through main window
		}
	}
}

void CDrawView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	if (pInfo->m_bPreview == FALSE)
//		((CDrawDoc*)GetDocument())->m_pSummInfo->RecordPrintDate();
	OnDraw(pDC);
}

void CDrawView::OnFilePrint() 
{
	CScrollView::OnFilePrint();
	GetDocument()->ComputePageSize();	
}

void CDrawView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (!m_bActive)
		return;
	CDrawTool* pTool = CDrawTool::FindTool(robot);
	if (pTool != NULL)
		pTool->OnKeyDown(this, nChar, nRepCnt, nFlags);
}

void CDrawView::OnDrawRobot() 
{
	CDrawTool::c_drawShape = robot;
	m_bRobot = TRUE;
}

void CDrawView::OnUpdateDrawRobot(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CDrawTool::c_drawShape == robot);
}

void CDrawView::OnEditHold() 
{
	if(m_dwDrawMode == SRCAND)
		m_dwDrawMode = SRCCOPY;
	else
		m_dwDrawMode = SRCAND;

	Invalidate(FALSE);
}


void CDrawView::OnGoal() 
{
	CDrawTool::c_drawShape = goal;
}

void CDrawView::OnUpdateGoal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CDrawTool::c_drawShape == goal);	
}

void CDrawView::OnStart() 
{
	m_bSimulation = !m_bSimulation;
	if(m_bSimulation)
	{
		SetTimer(0, DRAW_PERIOD, NULL);
		g_bControl = TRUE;
	}
	else
	{
		g_bControl = FALSE;
		KillTimer(0);
	}

// 	DWORD	dwThreadID;
// 	m_hThreadControl = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadControl, this, 0, &dwThreadID);
// 	//m_hThreadControl = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadControl, 0, 0, &dwThreadID);
// 
// 	//--> 쓰레드 생성에 실패하면
// 	if (! m_hThreadControl)
// 	{
// 		AfxMessageBox("Failed to create thread");
//	}

// 		m_Control->Update(this);
// 		for(int i=0; i<100; i++) for(int j=0; j<100; j++);
}

void CDrawView::OnUpdateStart(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_bSimulation);
}

void CDrawView::OnUpdateEditHold(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_dwDrawMode == SRCAND);
}

void CDrawView::Draw3d()
{
	p3d.LoadPoints();
	p3d.ShowPoints();

	int i;
	for(i=0; i<p3d.no; i++)
	{
		OC.Mapping(p3d.x[i], p3d.z[i]);
	}

	OC.ViewMap();
}

void CDrawView::DrawDTmap(CPoint p)
{
	// Show distance transformed image
	int x, y;
	IplImage *dmap, *dmapResize;
	dmap = cvCreateImage(cvSize(p.x, p.y), IPL_DEPTH_8U, 3);
	dmapResize = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	for(y=0; y<p.y; y++)	for(x=0; x<p.x; x++)
	{
		dmap->imageData[y*dmap->widthStep+x*3+0] = (unsigned char)((m_Control->m_nTable[x][y]/999.0)*255);
		dmap->imageData[y*dmap->widthStep+x*3+1] = (unsigned char)((m_Control->m_nTable[x][y]/999.0)*255);
		dmap->imageData[y*dmap->widthStep+x*3+2] = (unsigned char)((m_Control->m_nTable[x][y]/999.0)*255);
	}

	x = ptGoal.x; y = ptGoal.y;
	dmap->imageData[y*dmap->widthStep+x*3+0] = (unsigned char)0;
	dmap->imageData[y*dmap->widthStep+x*3+1] = (unsigned char)0;
	dmap->imageData[y*dmap->widthStep+x*3+2] = (unsigned char)255;
	if(m_bRobot)
	{
		x = ptRobot.x; y = ptRobot.y;
		dmap->imageData[y*dmap->widthStep+x*3+0] = (unsigned char)255;
		dmap->imageData[y*dmap->widthStep+x*3+1] = (unsigned char)0;
		dmap->imageData[y*dmap->widthStep+x*3+2] = (unsigned char)0;
	}

	int m = m_Control->Path.size()-1;
	while(m-->1)
	{
		x = m_Control->Path[m].x; y = m_Control->Path[m].y;
		dmap->imageData[y*dmap->widthStep+x*3+0] = (unsigned char)0;
		dmap->imageData[y*dmap->widthStep+x*3+1] = (unsigned char)255;
		dmap->imageData[y*dmap->widthStep+x*3+2] = (unsigned char)0;
	}
	
	cvNamedWindow("d-map");
	cvResize(dmap, dmapResize, 0);
	if(pFrame->m_wndDialogBar.IsDlgButtonChecked(IDC_RADIO_MAPTYPE1)) 
		cvFlip(dmapResize, dmapResize, 0);
	// 화면 좌표와 y축이 반대이므로
	//cvShowImage("d-map", dmap);
	cvShowImage("d-map", dmapResize);

	cvReleaseImage(&dmapResize);
	cvReleaseImage(&dmap);
}

void CDrawView::DrawSonar(CDC* pDC)
{
	CDrawRect *robot = GetDocument()->m_robot;
	double x = robot->CenterPtF().X;
	double y = robot->CenterPtF().Y;

	DPOINT tempXY, tempXY2, tempXY3, tempXY4, tempXY5;
	CPoint ixy1, ixy2, ixy3, ixy4;
	double turretA, sonarA;
	int oldROP2;
	int i;

	turretA = m_Control->angle * (PI / 180.0);
	oldROP2 = pDC->SetROP2(R2_MASKPEN);	
	
	CPen pen, *oldpen;
	CBrush brush, *oldbrush;

	pen.CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
	brush.CreateSolidBrush(RGB(230, 230, 150));
	oldpen = pDC->SelectObject(&pen);
	oldbrush = pDC->SelectObject(&brush);

	// 소나 위치 좌표 변환
	for(i=0; i<10; i++)
	{
		if(m_Control->s[i] > (SONARMAXLIMIT - 100.0)) continue;

 		tempXY.x = x + (cos(turretA)*m_Control->sonarPos[i].x - sin(turretA)*m_Control->sonarPos[i].y);
 		tempXY.y = y + (sin(turretA)*m_Control->sonarPos[i].x + cos(turretA)*m_Control->sonarPos[i].y);
		
		// upper left
		tempXY2.x = tempXY.x - m_Control->s[i];
		tempXY2.y = tempXY.y + m_Control->s[i];

		// lower right
		tempXY3.x = tempXY.x + m_Control->s[i];
		tempXY3.y = tempXY.y - m_Control->s[i];

		sonarA = DEG2RAD(m_Control->angle) + m_Control->sonarDirection[i];

		// start point
		tempXY4.x = tempXY.x + m_Control->s[i]*cos(sonarA-SONARBEAMANGLE);
		tempXY4.y = tempXY.y + m_Control->s[i]*sin(sonarA-SONARBEAMANGLE);

		// end point
		tempXY5.x = tempXY.x + m_Control->s[i]*cos(sonarA+SONARBEAMANGLE);
		tempXY5.y = tempXY.y + m_Control->s[i]*sin(sonarA+SONARBEAMANGLE);

// 		ixy1.x = (m_ShowAreaSize/2) + (int)floor(tempXY2.x*m_ZoomRate+0.5) + m_AutoScrollX; 
// 		ixy1.y = (m_ShowAreaSize/2) - (int)floor(tempXY2.y*m_ZoomRate+0.5) + m_AutoScrollY; 
// 		ixy2.x = (m_ShowAreaSize/2) + (int)floor(tempXY3.x*m_ZoomRate+0.5) + m_AutoScrollX; 
// 		ixy2.y = (m_ShowAreaSize/2) - (int)floor(tempXY3.y*m_ZoomRate+0.5) + m_AutoScrollY; 
// 		ixy3.x = (m_ShowAreaSize/2) + (int)floor(tempXY4.x*m_ZoomRate+0.5) + m_AutoScrollX; 
// 		ixy3.y = (m_ShowAreaSize/2) - (int)floor(tempXY4.y*m_ZoomRate+0.5) + m_AutoScrollY; 
// 		ixy4.x = (m_ShowAreaSize/2) + (int)floor(tempXY5.x*m_ZoomRate+0.5) + m_AutoScrollX; 
//		ixy4.y = (m_ShowAreaSize/2) - (int)floor(tempXY5.y*m_ZoomRate+0.5) + m_AutoScrollY; 

		ixy1.x = tempXY2.x;
		ixy1.y = tempXY2.y;
		ixy2.x = tempXY3.x;
		ixy2.y = tempXY3.y;
		ixy3.x = tempXY4.x;
		ixy3.y = tempXY4.y;
		ixy4.x = tempXY5.x;
		ixy4.y = tempXY5.y;

		pDC->Pie(ixy1.x, ixy1.y, ixy2.x, ixy2.y, ixy3.x, ixy3.y, ixy4.x, ixy4.y);
	}
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SetROP2(oldROP2);
}

void CDrawView::DrawLaser(CDC *pDC)
{
	CDrawRect *robot = GetDocument()->m_robot;
	double x = robot->CenterPtF().X;
	double y = robot->CenterPtF().Y; 

	DPOINT tempXY;
	CPoint ixy1, ixy2, ixy3, ixy4;
	double turretA, laserA;
	int oldROP2;
	int i;

	turretA = m_Control->angle * (PI / 180.0);
	oldROP2 = pDC->SetROP2(R2_MASKPEN);	
	
	CPen pen, *oldpen;
	CBrush brush, *oldbrush;

	//pen.CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
	pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	brush.CreateSolidBrush(RGB(230, 230, 150));
	oldpen = pDC->SelectObject(&pen);
	oldbrush = pDC->SelectObject(&brush);

	// 레이져 위치 좌표 변환
	for(i=0; i<360; i++)
	{
		if(m_Control->l[i] > (SONARMAXLIMIT - 100.0)) continue;

 		tempXY.x = x + (cos(turretA)*m_Control->laserPos[i].x - sin(turretA)*m_Control->laserPos[i].y);
 		tempXY.y = y + (sin(turretA)*m_Control->laserPos[i].x + cos(turretA)*m_Control->laserPos[i].y);
		
		laserA = DEG2RAD(m_Control->angle) + m_Control->laserDirection[i];

		pDC->MoveTo(tempXY.x, tempXY.y);
		pDC->LineTo(tempXY.x+m_Control->l[i]*cos(laserA), tempXY.y+m_Control->l[i]*sin(laserA));
	}
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	pDC->SetROP2(oldROP2);
}

int CDrawView::DrawRobotLoci(CDC *pDC)
{
	CDrawRect *robot = GetDocument()->m_robot;
	int x = robot->CenterPtF().X;						//x-coordinate error
	int y = robot->CenterPtF().Y;						//y-coordinate error
	std::list<CPoint> *lp = &GetDocument()->robotLoci;
	std::list<CPoint> *lp2 = &GetDocument()->robotLociTrue;
	std::list<CPoint> *lp3 = &GetDocument()->robotLociEstimated;
	std::list<CPoint> *lp4 = &GetDocument()->robotLociOdometry;
	static long cnt = 50;
//	TRACE("cnt : %d\n", cnt);
	if(++cnt > 0)
	{
		cnt = 0;
 		lp->push_back(CPoint(x, y));
		x = m_Control->m_pUKF->xtrue->data.db[0];
		y = m_Control->m_pUKF->xtrue->data.db[1];
		lp2->push_back(CPoint(x, y));
		x = m_Control->m_pUKF->XX->data.db[0];
		y = m_Control->m_pUKF->XX->data.db[1];
		lp3->push_back(CPoint(x, y));
		x = m_Control->m_pUKF->Xodometry->data.db[0];
		y = m_Control->m_pUKF->Xodometry->data.db[1];
		lp4->push_back(CPoint(x, y));
	}
	
	std::list<CPoint>::iterator i;

	i=lp->begin();
	pDC->MoveTo(i->x, i->y);
	do 
	{
		pDC->LineTo(i->x, i->y);
		i++;
	} while(i!=lp->end());

	CPen pen1, pen2, pen3;
	pen1.CreatePen(PS_DASH, 1, RGB(100, 190, 100));
	CPen* pOldPen = pDC->SelectObject(&pen1);

	i=lp2->begin();
	pDC->MoveTo(i->x, i->y);
	do 
	{
		pDC->LineTo(i->x, i->y);
		i++;
	} while(i!=lp2->end());

	pDC->SelectObject(pOldPen);

	pen2.CreatePen(PS_DASH, 1, RGB(190, 100, 100));
	pOldPen = pDC->SelectObject(&pen2);

	i=lp3->begin();
	pDC->MoveTo(i->x, i->y);
	do 
	{
		pDC->LineTo(i->x, i->y);
		i++;
	} while(i!=lp3->end());

	pDC->SelectObject(pOldPen);

	pen3.CreatePen(PS_DASH, 1, RGB(100, 100, 190));
	pOldPen = pDC->SelectObject(&pen3);

	i=lp4->begin();
	pDC->MoveTo(i->x, i->y);
	do 
	{
		pDC->LineTo(i->x, i->y);
		i++;
	} while(i!=lp4->end());

	pDC->SelectObject(pOldPen);

	return 0;
}

UINT ThreadControl(LPVOID pThreadParams)
{
	static int cnt=0;
	CDrawView *pView = (CDrawView*)pThreadParams;
	CRobotControl *pControl = (CRobotControl*)pView->m_Control;

	while(g_bExit)
	{
		if(g_bControl)
		{
			::EnterCriticalSection(&m_cs);
			//pView->m_Control->Update(pView);
			pView->m_elaped.Begin();
			//pControl->Update(pView);
			pView->m_elaped.End();
			pView->m_elaped.Display("update");		
			//TRACE("send : %d\n", cnt++);
			//::SendMessage( g_hViewWnd, U_MSG_CONTROL, 0, 0 );
			//::SendMessage( pView->m_hWnd, U_MSG_CONTROL, cnt, 0 );
			::LeaveCriticalSection(&m_cs);
		} else Sleep(100);
	}
	return 0;
}

void CDrawView::OnTimer(UINT nIDEvent)
{
	CClientDC dc(this);
	m_Control->Update(this);
	Invalidate(FALSE);
	CScrollView::OnTimer(nIDEvent);
}

long CDrawView::EventControl(WPARAM wParam, LPARAM lParam)
{
	m_Control->Update(this);
	return 0;
}

int CDrawView::DrawEllipse(CDC *pDC)
{
	CvMat *e = m_Control->m_pUKF->EllipseV;
	Graphics gr(pDC->GetSafeHdc());

	int i, j;

	for(i=1; i<EP; i++)
	{
		gr.DrawLine(&Pen(Color(Color::Magenta)), (REAL)e->data.db[i-1], (REAL)e->data.db[e->cols+i-1],
			(REAL)e->data.db[i], (REAL)e->data.db[e->cols+i]);
	}

	int nf = (m_Control->m_pUKF->XX->rows-3)/2;	// number of features
	for(i=0; i<nf; i++)
	{
		e = m_Control->m_pUKF->EllipseF[i];
		for(j=1; j<EP; j++)
		{
			gr.DrawLine(&Pen(Color(Color::Magenta)), (REAL)e->data.db[j-1], (REAL)e->data.db[e->cols+j-1],
				(REAL)e->data.db[j], (REAL)e->data.db[e->cols+j]);
		}
	}

	return 0;
}

BOOL CDrawView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(zDelta > 0) ZoomRate += 0.025;//0.1;
	else ZoomRate -= 0.025;//0.1;

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

int CDrawView::DrawLandmarks(CDC *pDC)
{
	int i;
	std::vector<DPOINT> templm = m_Control->m_pUKF->lm;
	Graphics gr(pDC->GetSafeHdc());

	for(i=0; i<templm.size(); i++)
	{
		gr.DrawEllipse(&Pen(Color::Red), RectF(templm[i].x, templm[i].y, 10, 10));
	}
	return 0;
}

void CDrawView::OnTranslationalvelocity0()
{
	m_Control->m_pUKF->sigmaV = 0.3;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnTranslationalvelocity1()
{
	m_Control->m_pUKF->sigmaV = 0.6;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnTranslationalvelocity2()
{
	m_Control->m_pUKF->sigmaV = 3.0;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnRange0()
{
	m_Control->m_pUKF->sigmaR = 0.1;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnRange1()
{
	m_Control->m_pUKF->sigmaR = 0.2;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnRange2()
{
	m_Control->m_pUKF->sigmaR = 10.0;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnAngularvelocity3()
{
	m_Control->m_pUKF->sigmaG = 3.0;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnAngularvelocity6()
{
	m_Control->m_pUKF->sigmaG = 6.0;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnAngularvelocity9()
{
	m_Control->m_pUKF->sigmaG = 30.0;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnBearing1()
{
	m_Control->m_pUKF->sigmaB = 1.0;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnBearing2()
{
	m_Control->m_pUKF->sigmaB = 2.0;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnBearing3()
{
	m_Control->m_pUKF->sigmaB = 3.0;
	m_Control->m_pUKF->SetNoiseParameter();
}

void CDrawView::OnUpdateTranslationalvelocity0(CCmdUI *pCmdUI)
{
	
}

int CDrawView::DrawSigmaPoints(CDC *pDC)
{
	CvMat *st_1 = m_Control->m_pUKF->m_sigmaPointst_1;
	CvMat *st = m_Control->m_pUKF->m_sigmaPointst;
	Graphics gr(pDC->GetSafeHdc());

	int i, j;
	// vehicle
	for(j=0; j<st_1->cols; j++)
	{
		gr.DrawEllipse(&Pen(Color(Color::Black)), st_1->data.db[0*st_1->cols+j], st_1->data.db[1*st_1->cols+j], 3, 3);
	}
	//for(j=0; j<st->cols; j++)
	//{
	//	gr.DrawEllipse(&Pen(Color(Color::Red)), st->data.db[0*st_1->cols+j], st->data.db[1*st_1->cols+j], 1, 1);
	//}

	// features
	//for(i=3; i<st->rows; i+=2)
	//{
	//	for(j=0; j<st->cols; j++)
	//	{
	//		gr.DrawEllipse(&Pen(Color(Color::Cyan)), st->data.db[i*st_1->cols+j], st->data.db[((i+1)*st_1->cols)+j], 1, 1);
	//	}
	//}

	return 0;
}
