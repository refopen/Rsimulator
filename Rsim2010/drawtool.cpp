// drawtool.cpp - implementation for drawing tools
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
#include "drawcli.h"
#include "drawdoc.h"
#include "drawvw.h"
#include "drawobj.h"
#include "drawtool.h"

#include "math.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawTool implementation

CPtrList CDrawTool::c_tools;

static CSelectTool selectTool;
static CRectTool lineTool(line);
static CRectTool rectTool(rect);
static CRectTool roundRectTool(roundRect);
static CRectTool ellipseTool(ellipse);
static CRectTool robotTool(robot);
static CRectTool goalTool(goal);

CPoint CDrawTool::c_down;
UINT CDrawTool::c_nDownFlags;
CPoint CDrawTool::c_last;
DrawShape CDrawTool::c_drawShape = selection;

CDrawTool::CDrawTool(DrawShape drawShape)
{
	m_drawShape = drawShape;
	c_tools.AddTail(this);
}

CDrawTool* CDrawTool::FindTool(DrawShape drawShape)
{
	POSITION pos = c_tools.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawTool* pTool = (CDrawTool*)c_tools.GetNext(pos);
		if (pTool->m_drawShape == drawShape)
			return pTool;
	}

	return NULL;
}

void CDrawTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{
	// deactivate any in-place active item on this view!
	COleClientItem* pActiveItem = pView->GetDocument()->GetInPlaceActiveItem(pView);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
		ASSERT(pView->GetDocument()->GetInPlaceActiveItem(pView) == NULL);
	}

	pView->SetCapture();
	c_nDownFlags = nFlags;
	c_down = point;
	c_last = point;
}

void CDrawTool::OnLButtonDblClk(CDrawView* /*pView*/, UINT /*nFlags*/, const CPoint& /*point*/)
{
}

void CDrawTool::OnLButtonUp(CDrawView* /*pView*/, UINT /*nFlags*/, const CPoint& point)
{
	ReleaseCapture();

// 	if (point == c_down)
// 		c_drawShape = selection;
}

void CDrawTool::OnMouseMove(CDrawView* /*pView*/, UINT /*nFlags*/, const CPoint& point)
{
	c_last = point;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CDrawTool::OnKeyDown(CDrawView *pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
}

void CDrawTool::OnEditProperties(CDrawView* /*pView*/)
{
}

void CDrawTool::OnCancel()
{
	c_drawShape = selection;
}

////////////////////////////////////////////////////////////////////////////
// CResizeTool

enum SelectMode
{
	none,
	netSelect,
	move,
	size
};

SelectMode selectMode = none;
int nDragHandle;
CPoint lastPoint;

CSelectTool::CSelectTool()
	: CDrawTool(selection)
{
}

void CSelectTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);

	CDrawObj* pObj;
	selectMode = none;

	// Check for resizing (only allowed on single selections)
	if (pView->m_selection.GetCount() == 1)
	{
		pObj = pView->m_selection.GetHead();
		if(pObj->IsKindOf(RUNTIME_CLASS(CDrawRect))){
			CDrawRect *pRect = (CDrawRect *)pObj;
			if(!(pRect->m_nShape == CDrawRect::goal)){
				nDragHandle = pObj->HitTest(local, pView, TRUE);
				if (nDragHandle != 0)
				selectMode = size;
			}
		}
		else{
			nDragHandle = pObj->HitTest(local, pView, TRUE);
			if (nDragHandle != 0)
				selectMode = size;
		}
	}

	// See if the click was on an object, select and start move if so
	if (selectMode == none)
	{
		pObj = pView->GetDocument()->ObjectAt(local);

		if (pObj != NULL)
		{
			selectMode = move;

			if (!pView->IsSelected(pObj))
				pView->Select(pObj, (nFlags & MK_SHIFT) != 0);

			// Ctrl+Click clones the selection...
			if ((nFlags & MK_CONTROL) != 0)
				pView->CloneSelection();
		}
	}

	// Click on background, start a net-selection
	if (selectMode == none)
	{
		if ((nFlags & MK_SHIFT) == 0)
			pView->Select(NULL);

		selectMode = netSelect;

		CClientDC dc(pView);
		CRect rect(point.x, point.y, point.x, point.y);
		rect.NormalizeRect();
		dc.DrawFocusRect(rect);
	}

	lastPoint = local;
	CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CSelectTool::OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point)
{
	if ((nFlags & MK_SHIFT) != 0)
	{
		// Shift+DblClk deselects object...
		CPoint local = point;
		pView->ClientToDoc(local);
		CDrawObj* pObj = pView->GetDocument()->ObjectAt(local);
		if (pObj != NULL)
			pView->Deselect(pObj);
	}
	else
	{
		// "Normal" DblClk opens properties, or OLE server...
		if (pView->m_selection.GetCount() == 1)
			pView->m_selection.GetHead()->OnOpen(pView);
	}

	CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CSelectTool::OnEditProperties(CDrawView* pView)
{
	if (pView->m_selection.GetCount() == 1)
		pView->m_selection.GetHead()->OnEditProperties();
}

void CSelectTool::OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point)
{
	if (pView->GetCapture() == pView)
	{
		if (selectMode == netSelect)
		{
			CClientDC dc(pView);
			CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
			rect.NormalizeRect();
			dc.DrawFocusRect(rect);

			pView->SelectWithinRect(rect, TRUE);
		}
		else if (selectMode != none)
		{
			pView->GetDocument()->UpdateAllViews(pView);
		}
	}

	/* release cilpping */
	ClipCursor(NULL);
	ReleaseCapture();
	pView->Invalidate();

	CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CSelectTool::OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point)
{
	if (pView->GetCapture() != pView)
	{
		if (c_drawShape == selection && pView->m_selection.GetCount() == 1)
		{
			CDrawObj* pObj = pView->m_selection.GetHead();
			CPoint local = point;
			pView->ClientToDoc(local);
			if(pObj->IsKindOf(RUNTIME_CLASS(CDrawRect))){
				CDrawRect *pRect = (CDrawRect *)pObj;
				if(!(pRect->m_nShape == CDrawRect::goal)){
					int nHandle = pObj->HitTest(local, pView, TRUE);
					if (nHandle != 0){
						SetCursor(pObj->GetHandleCursor(nHandle));
						return; // bypass CDrawTool
					}
				}
			}
			else{
				int nHandle = pObj->HitTest(local, pView, TRUE);
				if (nHandle != 0){
					SetCursor(pObj->GetHandleCursor(nHandle));
					return; // bypass CDrawTool
				}
			}
		}
		if (c_drawShape == selection)
			CDrawTool::OnMouseMove(pView, nFlags, point);
		return;
	}

	if (selectMode == netSelect)
	{
		CClientDC dc(pView);
		CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
		rect.NormalizeRect();
		dc.DrawFocusRect(rect);
		rect.SetRect(c_down.x, c_down.y, point.x, point.y);
		rect.NormalizeRect();
		dc.DrawFocusRect(rect);

		CDrawTool::OnMouseMove(pView, nFlags, point);
		return;
	}

	CPoint local = point;
	pView->ClientToDoc(local);
	CPoint delta = (CPoint)(local - lastPoint);

	/* Clip the mouse pointer moving range within the client rectangle */
	CRect rectClient, rectClientScreen;
	pView->GetClientRect(&rectClient);
	rectClientScreen = rectClient;
	pView->ClientToScreen(&rectClientScreen);
	ClipCursor(&rectClientScreen);
	pView->SetCapture();

	POSITION pos = pView->m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = pView->m_selection.GetNext(pos);
		CRect position = pObj->m_position;

		if (selectMode == move)
		{
			// 움직이는 개체가 goal이면 좌표를 저장
			if(pObj->IsKindOf(RUNTIME_CLASS(CDrawRect))){
				CDrawRect *pRect = (CDrawRect *)pObj;
				if((pRect->m_nShape == CDrawRect::goal)){
					pView->m_Control->goal = local;
					pView->Invalidate();
				}
			}

			/* object들이 client region에서만 움직이도록 한다. */
			position += CPoint(delta.x, 0);

			CRect Rect(position);
			pView->DocToClient(Rect);
			if( (Rect & rectClient) != Rect )
				position -= CPoint(delta.x, 0);

			position += CPoint(0, delta.y);
			Rect = position;
			pView->DocToClient(Rect);
			if( (Rect & rectClient) != Rect )
				position -= CPoint(0, delta.y);

			pObj->MoveTo(position, pView);
		}
		else if (nDragHandle != 0)
		{
			// 개체가 goal이면 크기 고정
			if(pObj->IsKindOf(RUNTIME_CLASS(CDrawRect)))
			{
				CDrawRect *pRect = (CDrawRect *)pObj;
				if((pRect->m_nShape == CDrawRect::goal)) continue;
			}

			pObj->MoveHandleTo(nDragHandle, local, pView);
		}
	}

	lastPoint = local;

	if (selectMode == size && c_drawShape == selection)
	{
		c_last = point;
		SetCursor(pView->m_selection.GetHead()->GetHandleCursor(nDragHandle));
		return; // bypass CDrawTool
	}

	c_last = point;

	if (c_drawShape == selection)
		CDrawTool::OnMouseMove(pView, nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
// CRectTool (does rectangles, round-rectangles, ellipses and robots)

CRectTool::CRectTool(DrawShape drawShape)
	: CDrawTool(drawShape)
{
}

void CRectTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{
	CDrawTool::OnLButtonDown(pView, nFlags, point);

	CPoint local = point;
	pView->ClientToDoc(local);

	CDrawRect* pObj = new CDrawRect(CRect(local, CSize(0, 0)));
	switch (m_drawShape)
	{
	default:
		ASSERT(FALSE); // unsuported shape!

	case rect:
		pObj->m_nShape = CDrawRect::rectangle;
		break;

	case roundRect:
		pObj->m_nShape = CDrawRect::roundRectangle;
		break;

	case ellipse:
		pObj->m_nShape = CDrawRect::ellipse;
		break;

	case line:
		pObj->m_nShape = CDrawRect::line;
		break;
		
	case robot:
		pObj->m_nShape = CDrawRect::robot;
		break;

	case goal:
		pObj->m_nShape = CDrawRect::goal;
		pObj->m_logbrush.lbColor = RGB(255, 0, 0);
		pObj->m_position.InflateRect(CSize(3, 3));
		break;

	}

	pView->GetDocument()->Add(pObj);
	pView->Select(pObj);

	/* Next line is variable according to the next procedure */
//	pView->OnObjectMoveForward();

	selectMode = size;
	nDragHandle = 1;
	lastPoint = local;
}

void CRectTool::OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point)
{
	CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CRectTool::OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local(point);
	pView->ClientToDoc(local);
	if(CDrawTool::c_drawShape == goal)
	{
		pView->m_Control->goal = local;
		pView->GetDocument()->goalVector.push_back(local);
		pView->m_bGoal = TRUE;

// 		for(int i=0; i<pView->GetDocument()->goalVector.size(); i++)
// 		{
// 			TRACE("goal vector : [%d][%d]\n", pView->GetDocument()->goalVector[i].x, pView->GetDocument()->goalVector[i].y);
// 		}
//		TRACE("\n");

		selectTool.OnLButtonUp(pView, nFlags, point);
		return;
	}
	if (point == c_down)
	{
		// Except for goal point
		// Don't create empty objects...
		CDrawObj *pObj = pView->m_selection.GetTail();
		pView->GetDocument()->Remove(pObj);
		pObj->Remove();
		selectTool.OnLButtonDown(pView, nFlags, point); // try a select!
	}
	else if(m_drawShape == robot)
	{
		CDrawRect *pRect = (CDrawRect *)pView->m_selection.GetTail();
		pView->GetDocument()->m_bRobotActivated = TRUE;
		pView->GetDocument()->m_robot = pRect;
//		pView->m_Control->InitialUpdate(pView);
	}
	selectTool.OnLButtonUp(pView, nFlags, point);
}

void CRectTool::OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
	selectTool.OnMouseMove(pView, nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
/* regulate moving & rotation of the specified robot */
void CRectTool::OnKeyDown(CDrawView *pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!pView->GetDocument()->m_bRobotActivated){
		return;
	}

	CDrawRect *robot = pView->GetDocument()->m_robot;
	CRect position = robot->m_position;
	int cx=0, cy=0;

	switch(nChar){
	case VK_NUMPAD7:
		/*
		robot->m_nTheta+=10;
		if(robot->m_nTheta > 350)
			robot->m_nTheta %= 360;
		*/
		pView->m_Control->vl++;
		break;
	case VK_NUMPAD9:
		/*
		robot->m_nTheta-=10;
		if(robot->m_nTheta < 0){
			robot->m_nTheta %= 360;
			robot->m_nTheta += 360;
		}
		*/
		pView->m_Control->vr++;
		break;
	case VK_NUMPAD2:
		cx = 0; cy = -10;
		break;
	case VK_NUMPAD4:
		cx = -10; cy = 0;
		break;
	case VK_NUMPAD6:
		cx = 10; cy = 0;
		break;
	case VK_NUMPAD8:
		cx = 0; cy = 10;
		break;
	default:;
	}

	CRect rectClient;
	pView->GetClientRect(&rectClient);
	CPoint delta(cx, cy);
	position += delta;
	robot->m_position = position;
	BOOL bIntersect = FALSE;

	/* Do not permit to intersect each object */
	POSITION pos = pView->GetDocument()->m_objects.GetHeadPosition();
	while (pos != NULL && !bIntersect){
		CDrawObj* pObj = pView->GetDocument()->m_objects.GetNext(pos);
		if( position == pObj->m_position )
			continue;
		if(	robot->Intersects(pObj->m_position) )
			bIntersect = TRUE;
	}

	robot->m_position = position-delta;

	CRect Rect(position);
	pView->DocToClient(Rect);
	if( (Rect & rectClient) != Rect )
		bIntersect = TRUE;

	if(!bIntersect)
		robot->MoveTo(position, pView);

	pView->Invalidate(FALSE);
}
