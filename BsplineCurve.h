#pragma once
#include "unit.h"
#include "ObjImage.h"
#include "BCStroke.h"
#include <list>

#define N 1000
static char Prompt[][64] = {"无", "附点", "增点", "删点", "改点", "选点", "选线"};
enum tagPtStatus
{
	NONE, SELECTED, ADJACENT_INSERTED, START, END
};

enum OPID              
{NOP, APPEND, INSERT, DELETe, UPDATE, SELECT_POINT, SELECT_CURVE};


class CBsplineCurve :
	public CUnit
{
public:
	

	CBsplineCurve(void);
	~CBsplineCurve(void);

	//DECLARE_DYNCREATE(CCBsplineCurve)


	void				PutPoint(HDC hdc,int y);
	void			DrawBSpline(int count, const double *Qx, const double *Qy);
	
	void			ThomasAlgorithm(int n, double *Q, double *a, double *b, double *c);
	void			ComputeControlPoints(int n, const double *Q, double *P);
	double		ComputeBSpline(double u, const double *P);
	void			Draw1BSpline(const double *Px, const double *Py);
	
	void				BSpline2Bezier(double *PBSpline, double *PBezier);
	double				ComputeBezier(double u, double *P);
	void				InitControlPoints();
	virtual void		Initial();
	virtual int			IsClassType();
	static CBsplineCurve* Instance();
	
  
	//DECLARE_DYNCREATE(CBsplineCurve)
	
public:
	void OnInsertPoint(CView* pView);
	void OnDeletePoint(CView* pView);
	void OnPassingpointsSelect(CView* pView);
	void OnEndPoints(CView* pView);
	void OnUpdatePoint(CView* pView);
protected:
	//DECLARE_MESSAGE_MAP()
public:
	int   m_bGWidth;
private:
	tagPtStatus PtStatus[N+1];
	tagPtStatus PrePtStatus;

	double Qx[N+1], Qy[N+1];
	double Px[N+2], Py[N+2];
	POINT pt[3*N+1];		// used for Bezier curve
	DWORD dwCount;		// number of passing points of each curve
	OPID  nOperationID;
	int   nSeldPointID;

	static CBsplineCurve* _instance;
	CMenu  m_popMenu;
	CArray <CPoint,CPoint> m_pointList;

public:
	virtual void OnLButtonDown(CDC *pDC,CCAMCView *pView,CPoint point);
	virtual void OnRButtonDown(CDC *pDC, CCAMCView *pView,CPoint point); 
	virtual void OnLButtonUp(CDC *pDC,CCAMCView *pView,CPoint point);
	virtual void OnMouseMove(CDC *pDC, CCAMCView *pView, CPoint point);
	virtual BOOL DrawToImage(CCAMCView* pView,CPoint point);
	virtual void ShowSelectPoint(CDC*pDC);
	virtual BOOL IsInRgn(CPoint point);
	virtual BOOL IsEnd(int tool_type);
	void ToBezierStroke(CBCStroke& stroke);

	void Draw(CDC*pDC);


};
