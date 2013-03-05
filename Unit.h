#pragma once
#include "StdAfx.h"
#include "objbase.h"
#include "CAMCView.h"
#include "MainFrm.h"
#include "CAMCDoc.h"
#include <afxtempl.h> 

#define CDLINE  0


class CUnit :
	public FCObjGraph
{
public:
	CUnit(void);
	~CUnit(void);
	CUnit(CUnit&unit);
	CUnit operator=(CUnit&unit);
	
public:
	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void ShowSelectPoint(CDC *pDC){}
	virtual void Draw(CDC*pDC);
	virtual void Initial();
	virtual void DrawMask(CDC*pDC,CPoint first,CPoint second){};
	virtual BOOL IsEnd(int class_type){ return TRUE;}
	virtual int  IsClassType(){return DEFAULT;}
	virtual void SetZoomRate(int rate){};
	virtual RECT GetRect()
	{
		
		m_rect.bottom=m_rect.left=m_rect.right=m_rect.top=0;
		return m_rect;
	};

public:

	UINT m_PenStyle;
	int m_PenWidth;
	int m_BkMode;
	int m_DrawingMode;
	int m_DrawStatus;
	COLORREF m_PenColor;
	COLORREF m_BackgroundColor;
	RECT  m_rect;
};
