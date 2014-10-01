#ifndef _DRAW_VIEW__
#define _DRAW_VIEW__

#include "OccupancyGrid.h"	// Added by ClassView
#include "P3d.h"	// Added by ClassView
#include "robot.h"
#include "Elapsed.h"

// Hints for UpdateAllViews/OnUpdate
#define HINT_UPDATE_WINDOW      0
#define HINT_UPDATE_DRAWOBJ     1
#define HINT_UPDATE_SELECTION   2
#define HINT_DELETE_SELECTION   3
#define HINT_UPDATE_OLE_ITEMS   4

#define DRAW_SCALE				3.0
#define DRAW_PERIOD				0.1

//--> thread event define
#define  U_MSG_CONTROL		(WM_USER + 10)

class CDrawObj;

class CDrawView : public CScrollView
{
protected: // create from serialization only
	CDrawView();
	DECLARE_DYNCREATE(CDrawView)

// Attributes
public:
	CDrawDoc* GetDocument()
		{ return (CDrawDoc*)m_pDocument; }
	void SetPageSize(CSize size);
	CRect GetInitialPosition();

// Operations
public:
	void DocToClient(CRect& rect);
	void DocToClient(CPoint& point);
	void ClientToDoc(CPoint& point);
	void ClientToDoc(CRect& rect);
	void Select(CDrawObj* pObj, BOOL bAdd = FALSE);
	void SelectWithinRect(CRect rect, BOOL bAdd = FALSE);
	void Deselect(CDrawObj* pObj);
	void CloneSelection();
	void UpdateActiveItem();
	void InvalObj(CDrawObj* pObj);
	void Remove(CDrawObj* pObj);
	void PasteNative(COleDataObject& dataObject);
	void PasteEmbedded(COleDataObject& dataObject, CPoint point );

// Implementation
protected:
	// added for drop-target functionality
	COleDropTarget m_dropTarget;        // for drop target functionality
	CPoint m_dragPoint;                 // current position
	CSize m_dragSize;                   // size of dragged object
	CSize m_dragOffset;                 // offset between pt and drag object corner
	DROPEFFECT m_prevDropEffect;
	BOOL m_bDragDataAcceptable;
	BOOL m_bGrid;

	BOOL GetObjectInfo(COleDataObject* pDataObject,
		CSize* pSize, CSize* pOffset);
	// end of drop-target additions

public:
	virtual ~CDrawView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);  // overriden to record time/date
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActiveView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	CPoint DrawGrid(CDC* pDC);

	// added for drop-target functionality
	virtual BOOL OnDrop(COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject,
		DWORD grfKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject,
		DWORD grfKeyState, CPoint point);
	virtual void OnDragLeave();
	static CLIPFORMAT m_cfObjectDescriptor;
	// end of drop-target additions

	static CLIPFORMAT m_cfDraw; // custom clipboard format

	CDrawObjList m_selection;
	COLORREF m_gridColor;
	BOOL m_bActive; // is the view active?
	BOOL m_bGoal;
	BOOL m_bRobot;
	CPoint ptGoal;
	CPoint ptRobot;
protected:
	virtual void OnInitialUpdate(); // called first time after construct

	// Printing support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// OLE Container support
public:
	int DrawEllipse(CDC *pDC);
	long EventControl(WPARAM wParam, LPARAM lParam);
	int DrawRobotLoci(CDC *pDC);
	void DrawLaser(CDC* pDC);
	void DrawSonar(CDC* pDC);
	bool m_bLbutton;
	void DrawDTmap(CPoint p);
	OccupancyGrid OC;
	P3d p3d;
	void Draw3d();
	BOOL m_bSimulation;
	DWORD m_dwDrawMode;
	CRobotControl *m_Control;
	virtual BOOL IsSelected(const CObject* pDocItem) const;
	CWinThread *m_pThread;

// Generated message map functions
protected:
	//{{AFX_MSG(CDrawView)
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEdit();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDrawSelect();
	afx_msg void OnDrawRoundRect();
	afx_msg void OnDrawRect();
	afx_msg void OnDrawLine();
	afx_msg void OnDrawEllipse();
	afx_msg void OnUpdateDrawEllipse(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawRect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawRoundRect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawSelect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSingleSelect(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditClear();
	afx_msg void OnUpdateAnySelect(CCmdUI* pCmdUI);
	afx_msg void OnDrawPolygon();
	afx_msg void OnUpdateDrawPolygon(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnViewGrid();
	afx_msg void OnUpdateViewGrid(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnObjectFillColor();
	afx_msg void OnObjectLineColor();
	afx_msg void OnObjectMoveBack();
	afx_msg void OnObjectMoveForward();
	afx_msg void OnObjectMoveToBack();
	afx_msg void OnObjectMoveToFront();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnViewShowObjects();
	afx_msg void OnUpdateViewShowObjects(CCmdUI* pCmdUI);
	afx_msg void OnEditProperties();
	afx_msg void OnUpdateEditProperties(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFilePrint();
	afx_msg void OnDrawRobot();
	afx_msg void OnUpdateDrawRobot(CCmdUI* pCmdUI);
	afx_msg void OnEditHold();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGoal();
	afx_msg void OnUpdateGoal(CCmdUI* pCmdUI);
	afx_msg void OnStart();
	afx_msg void OnUpdateStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditHold(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CRectTool;
public:
	double ZoomRate;
	CElapsed m_elaped;
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	int DrawLandmarks(CDC *pDC);
	afx_msg void OnTranslationalvelocity0();
	afx_msg void OnTranslationalvelocity1();
	afx_msg void OnTranslationalvelocity2();
	afx_msg void OnRange0();
	afx_msg void OnRange1();
	afx_msg void OnRange2();
	afx_msg void OnAngularvelocity3();
	afx_msg void OnAngularvelocity6();
	afx_msg void OnAngularvelocity9();
	afx_msg void OnBearing1();
	afx_msg void OnBearing2();
	afx_msg void OnBearing3();
	afx_msg void OnUpdateTranslationalvelocity0(CCmdUI *pCmdUI);
	int DrawSigmaPoints(CDC *pDC);
};
/////////////////////////////////////////////////////////////////////////////

#endif
