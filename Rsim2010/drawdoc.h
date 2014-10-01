// drawdoc.h : interface of the CDrawDoc class
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

#include "drawobj.h"
//#include "summinfo.h"
#include <vector>
#include <list>

class CDrawView;

class CDrawDoc : public COleDocument
{
protected: // create from serialization only
	CDrawDoc();
	DECLARE_DYNCREATE(CDrawDoc)

// Attributes
public:
	CDrawObjList* GetObjects() { return &m_objects; }
	const CSize& GetSize() const { return m_size; }
	void ComputePageSize();
	int GetMapMode() const { return m_nMapMode; }
	COLORREF GetPaperColor() const { return m_paperColor; }

// Operations
public:
	CDrawObj* ObjectAt(const CPoint& point);
	void Draw(CDC* pDC, CDrawView* pView);
	void Add(CDrawObj* pObj);
	void Remove(CDrawObj* pObj);
	CDrawObjList m_objects;
	CDrawRect* m_robot;
	BOOL m_bRobotActivated;
	std::vector<CPoint> goalVector;
	std::list<CPoint> robotLoci;
	std::list<CPoint> robotLociTrue;
	std::list<CPoint> robotLociEstimated;
	std::list<CPoint> robotLociOdometry;

// Implementation
public:
	CPoint m_ptGridsize;
	virtual ~CDrawDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

	CSize m_size;
	int m_nMapMode;
	COLORREF m_paperColor;

// Generated message map functions
protected:
	//{{AFX_MSG(CDrawDoc)
	afx_msg void OnViewPaperColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
