// CAMCView.h : CCAMCView 类的接口
//
#include <afxmt.h>

#include <Windows.h>

#pragma once

class CCAMCDoc;
class CUnit;
class CPaintBrush;


class CCAMCView : public CScrollView
{
protected: // 仅从序列化创建
	CCAMCView();
	DECLARE_DYNCREATE(CCAMCView)
// 属性
public:
	CCAMCDoc* GetDocument() const;

// 操作
public:
	int m_CurrentTool;
	CUnit * m_pCurrentDraw;
	int	m_CurrentDrawStatus;

private:
	
	UINT m_prs;
	CPaintBrush * m_pBrush;
	unsigned int  m_penState;
	COLORREF m_colorTmp;
	CMenu    m_popMenu;
private:
	BITMAPINFO *m_pBmpInfo;		//BITMAPINFO 结构指针，显示图像时使用
	char		m_chBmpBuf[2048];		//BIMTAPINFO 存储缓冲区，m_pBmpInfo即指向此缓冲区
	HPALETTE	m_hPalette; 
	HPALETTE	m_hOldPal;
public:
	
	void SetZoomRateText();
public:
	void    Draw();
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
//	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//所以绘制的工作都在这里进行
	virtual void DrawInMemory(CDC* pMemDC);
// 实现
public:
	virtual ~CCAMCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	//{{AFX_MSG(CCAMCView)

	
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()


public:
	virtual void OnInitialUpdate();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnInsertPoint();
	afx_msg void OnDeletePoint();
	afx_msg void OnUpdatePoint();
	afx_msg void OnPassingpointsSelect();
	afx_msg void OnEndPoints();

public:	
	
	void LPToCP(POINT& point);
	void CPToLP(POINT& point);
	POINT GetCanvasOffset();
	FCObjCanvas& GetCanvas();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	void UpdateRectView();
	
	afx_msg void OnViewProperties();
	afx_msg void OnDbEdit();
	afx_msg void OnSetOriginPoint();
	afx_msg void OnUpdateSetOriginPoint(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // CAMCView.cpp 的调试版本
inline CCAMCDoc* CCAMCView::GetDocument() const
   { return reinterpret_cast<CCAMCDoc*>(m_pDocument); }
#endif

