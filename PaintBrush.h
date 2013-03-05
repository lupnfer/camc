#pragma once
#include "unit.h"
#include "ParentBrush.h"

#include "ParentPaper.h"
#include "BCStroke.h"
class FCObjLayer;
class CPaintBrush :
	public CUnit
{
public:
	CPaintBrush(void);
	~CPaintBrush(void);

	virtual void OnLButtonDown(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void OnMouseMove(CDC*pDC,CCAMCView*pView,CPoint point);
	virtual void ShowSelectPoint(CDC *pDC,CCAMCView*pView,FCObjLayer* pLayer);

	virtual RECT GetRect();
	virtual void Initial();

	static  CPaintBrush* Instance();
	void	InitialBrushandPaper();
	void    SetStroke(CBCStroke* pStroke);
	CBCStroke* GetStrokePtr()
	{
		return m_pStroke;
	}
	CBCStroke& GetStroke(){
		return m_stroke;
	}
private:
	static CPaintBrush* _instance;
	CParentBrush  *m_brush;
	CParentPaper  *m_paper;
	CBCStroke  m_stroke;
	int        m_stress;
	CPoint     m_p;
	CBCStroke* m_pStroke;
};
