// drawtool.h - interface for CDrawTool and derivatives
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


#ifndef __DRAWTOOL_H__
#define __DRAWTOOL_H__

#include "drawobj.h"

class CDrawView;

enum DrawShape
{
	selection,
	line,
	rect,
	roundRect,
	ellipse,
	poly,
	robot,
	goal
};

class CDrawTool
{
// Constructors
public:
	CDrawTool(DrawShape nDrawShape);

// Overridables
	virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnKeyDown(CDrawView *pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnEditProperties(CDrawView* pView);
	virtual void OnCancel();

// Attributes
	DrawShape m_drawShape;

	static CDrawTool* FindTool(enum DrawShape drawShape);
	static CPtrList c_tools;
	static CPoint c_down;
	static UINT c_nDownFlags;
	static CPoint c_last;
	static DrawShape c_drawShape;
};

class CSelectTool : public CDrawTool
{
// Constructors
public:
	CSelectTool();

// Implementation
	virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnEditProperties(CDrawView* pView);
};

class CRectTool : public CDrawTool
{
// Constructors
public:
	CRectTool(DrawShape drawShape);

// Implementation
	virtual void OnKeyDown(CDrawView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);

	friend class CDrawRect;
};

////////////////////////////////////////////////////////////////////////////

#endif // __DRAWTOOL_H__