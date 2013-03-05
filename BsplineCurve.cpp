#include "StdAfx.h"
#include ".\bsplinecurve.h"
#include "UnitDraw.h"
#include "CAMCDoc.h"
#include "CAMCView.h"
#include "cmdimgdrawanyline.h"
#include "objCanvas.h"
#include "BCStroke.h"





extern void  aggDrawline_AA_8Bit (FCObjImage & img,
								  const POINT & pt1, const POINT & pt2,
								  int nWidth, int nGray,
								  BOOL bAddArrow, LINE_STYLE lineStyle) ;

//=================================================================================

CBsplineCurve* CBsplineCurve::_instance = NULL;

CBsplineCurve::CBsplineCurve(void)
{
	if(m_popMenu.CreatePopupMenu()==0){
		AfxMessageBox("创建下拉菜单失败！");
	}
	m_popMenu.AppendMenu(MF_POPUP,IDM_SELECT_POINT,"选择节点" );
	m_popMenu.AppendMenu(MF_POPUP,IDM_INSERT_POINT,"插入节点" );
	m_popMenu.AppendMenu(MF_POPUP,IDM_UPDATE_POINT,"更新节点" );
	m_popMenu.AppendMenu(MF_POPUP,IDM_DELETE_POINT,"删除节点" );
	m_popMenu.AppendMenu(MF_POPUP,IDM_END_POINTS,"结束绘制" );
//	CMenu menu1;
//	menu1.LoadMenu(IDR_MENU_BK_WIDTH);
//	m_popMenu.AppendMenu(MF_POPUP,(UINT_PTR)menu1.m_hMenu,"擦除宽度" );
	Initial();
}

CBsplineCurve::~CBsplineCurve(void)
{
	m_popMenu.DestroyMenu();
}
CBsplineCurve* CBsplineCurve::Instance()
{
	if(_instance==NULL){
		_instance=new CBsplineCurve();
	}
	return _instance;
}
void CBsplineCurve::Initial()
{
	dwCount = 0;
	nOperationID = APPEND;
	nSeldPointID = 0;
	PtStatus[nSeldPointID] = PrePtStatus;
	m_PenWidth=1;
	m_PenColor=RGB(0,0,0);
	m_bGWidth=1;
	m_pointList.RemoveAll();

}
int	CBsplineCurve::IsClassType()
{
	return BSPLINE_TOOL;
}
void CBsplineCurve::PutPoint(HDC hdc, int nPointID)
{
	int x = (int)Qx[nPointID];
	int y = (int)Qy[nPointID];

	switch(PtStatus[nPointID])
	{
	case NONE:
		SetPixelV(hdc, x, y, 0x0000ff);
		SetPixelV(hdc, x, y+1, 0x0000ff);
		SetPixelV(hdc, x, y-1, 0x0000ff);
		SetPixelV(hdc, x, y+2, 0x0000ff);
		SetPixelV(hdc, x, y-2, 0x0000ff);
		SetPixelV(hdc, x+2, y, 0x0000ff);
		SetPixelV(hdc, x-2, y, 0x0000ff);
		SetPixelV(hdc, x+1, y, 0x0000ff);
		SetPixelV(hdc, x-1, y, 0x0000ff);
		break;
	case SELECTED:
		SetPixelV(hdc, x, y, 0x00ff00);
		SetPixelV(hdc, x, y+1, 0x00ff00);
		SetPixelV(hdc, x, y-1, 0x00ff00);
		SetPixelV(hdc, x, y+2, 0x00ff00);
		SetPixelV(hdc, x, y-2, 0x00ff00);
		SetPixelV(hdc, x+2, y, 0x00ff00);
		SetPixelV(hdc, x-2, y, 0x00ff00);
		SetPixelV(hdc, x+1, y, 0x00ff00);
		SetPixelV(hdc, x-1, y, 0x00ff00);
		break;
	case ADJACENT_INSERTED:
		{
			for(int x1 = x-3; x1<= x+3; x1++)
				for(int y1 = y-3; y1<= y+3; y1++)
					SetPixelV(hdc, x1, y1, 0x00ff00);
			break;
		}
	case START:
		{
			for(int x1 = x-3; x1<= x+3; x1++)
				for(int y1 = y-3; y1<= y+3; y1++)
					SetPixelV(hdc, x1, y1, 0x0000ff);
			break;
		}
	case END:
		{
			for(int x1 = x-3; x1<= x+3; x1++)
				for(int y1 = y-3; y1<= y+3; y1++)
					SetPixelV(hdc, x1, y1, 0xff0000);
			break;
		}
	}
}

void CBsplineCurve::DrawBSpline(int count, const double *Qx, const double *Qy)
{
	double Px[N+2], Py[N+2];
	//	double PBezierx[4], PBeziery[4];
	ComputeControlPoints(count, Qx, Px);
	ComputeControlPoints(count, Qy, Py);

	for(int i = 0; i <= count-2; i++)
		Draw1BSpline(Px+i, Py+i);
}

void CBsplineCurve::Draw1BSpline(const double *Px, const double *Py)
{
	int x, y, prex, prey; 
	double count=1000;
	prex = -1;
	prey = -1;
	for(int i = 0; i <= count; i++)
	{
		x = ComputeBSpline((double)i/count, Px);
		y = ComputeBSpline((double)i/count, Py);
		if(prex != x || prey != y){
			m_pointList.Add(CPoint(x,y));
		}

//=================================================================================
		prex = x;
		prey = y;
		
	}
}

/* | b[1] c[1]                      | |x[1]  |    |f[1]  |
| a[2] b[2] c[2]                 | |x[2]  |    |f[2]  |
|      a[3] b[3] c[3]            | |....  |    |....  |
|        ...................     | |....  | =  |....  |
|          ...........           | |....  |    |....  |
|           a[n-1] b[n-1] c[n-1] | |x[n-1]|    |f[n-1]|
|                  a[n]   b[n]   | |x[n]  |    |f[n]  | 

Crout Factorization of Tri-diagonal Linear System 
| b[1] c[1]                      |    | s[1]                               |   | 1 t[1]                      |
| a[2] b[2] c[2]                 |    | r[2] s[2]                          |   |   1    t[2]                 |
|      a[3] b[3] c[3]            |    |      r[3] s[3]                     |   |        1    t[3]            |
|        .............           | =  |            ....                    | * |             ......          |
|          ..............        |    |               .....                |   |                ......       |
|           a[n-1] b[n-1] c[n-1] |    |                 r[n-1] s[n-1]      |   |                    1 t[n-1] |
|                  a[n]   b[n]   |    |                        r[n]   s[n] |   |                      1      | 

i.e. AX = f, equivalent to 1) LY = f and  2) UX = Y */ 
void CBsplineCurve::ThomasAlgorithm(int n, double *Q, double *a, double *b, double *c)
{

	//	1) compute t[i]:
	//		c[1] /= b[1]
	//		c[i] /= (b[i] - a[i]*c[i-1])			(i = 2, 3, ..., n-1)
	c[1] /= b[1];
	int i = 2;
	for(; i < n; i++)
		c[i] /= (b[i] - a[i]*c[i-1]);

	//	2) solve LY = f
	//		x[1] /= b[1]				(y[1] = f[1] / s[1])
	//		x[i] = (x[i] - a[i]*x[i-1]) / (b[i] - a[i]*c[i-1]) (i = 2, 3, ..., n)	(y[i] = (f[i] - r[i]*y[i-1] / a[i])
	Q[1] /= b[1];
	for(i = 2; i <= n; i++)
		Q[i] = (Q[i] - a[i]*Q[i-1]) / (b[i] - a[i]*c[i-1]);

	//	3) solve UX = Y
	//		x[i] -= c[i]*x[i+1]		(i = n-1, n-2, ..., 2, 1)	(x[n] == y[n], x[i] = y[i] - t[i]*x[i+1]
	for(i = n-1; i >= 1; i--)
		Q[i] -= c[i]*Q[i+1];

}

// non-periodic cubic b_spline passing Q[1] and Q[n]
/* | 6 0               | |P[1]  |     |Q[1]  |
| 1 4 1 0           | |P[2]  |     |Q[2]  |
|   1 4 1           | |....  |     |....  |
|   ...........     | |....  | = 6 |....  |
|       ........... | |....  |     |....  |
|           0 1 4 1 | |P[n-1]|     |Q[n-1]|
|               0 6 | |P[n]  |     |Q[n]  | */
// to ensure curve passing Q[1] and Q[n], add two extra points P[0] and P[n+1]
// where P[0] = 2*P[1] - P[2] and P[n+1] = 2*P[n] - P[n-1]

void CBsplineCurve::ComputeControlPoints(int n, const double *Q, double *P)
{
	/*	Q[1] = 150;
	Q[2] = 45;
	Q[3] = 0;
	Q[4] = 30;
	Q[5] = 56; */
	double a[N+1], b[N+1], c[N+1];
	c[1] = a[n] = 0;
	b[1] = b[n] = 1.0;
	int i = 2;
	for(; i < n; i++)
	{
		a[i] = 1.0 / 6;
		b[i] = 4.0 / 6;
		c[i] = 1.0 / 6;
	}
	for(i = 1; i <= n; i++)
	{
		P[i] = Q[i];
	}
	ThomasAlgorithm(n, P, a, b, c);
	P[0] = 2*P[1] - P[2];
	P[n+1] = 2*P[n] - P[n-1];
}
/*
|-1  3 -3 1| |P[i-1]|
C(u) = (1/6)[u^3 u^2 u 1]| 3 -6  3 0|*| P[i] |, 0 <= u <= 1
|-3  0  3 0| |P[i+1]|
| 1  4  1 0| |P[i+2]|		*/
double CBsplineCurve::ComputeBSpline(double u, const double *P)
{
	double C0, C1, C2, C3, C;
	C3 = -P[0] + 3*P[1] - 3*P[2] + P[3];
	C2 = 3*P[0] - 6*P[1] + 3*P[2];
	C1 = -3*P[0] + 3*P[2];
	C0 = P[0] + 4*P[1] + P[2];
	C = u*u*u*C3 + u*u*C2 + u*C1 + C0;
	C /= 6;
	return C;
}
/*
P(t) = TMB, where T = [t^3 t^2 t 1]
|-1  3 -3 1|
| 3 -6  3 0|
Cubic Bezier Pz(t) = TMzBz, Mz = |-3  3  0 0|, Bz = [P[i] P[i+1] P[i+2] P[i+3]]T
| 1  0  0 0|

|-1  3 -3 1|
1| 3 -6  3 0|
Cubic BSpline  PB(t) = TMBBB, MB = -|-3  3  0 0|, BB = Bz = [P[i] P[i+1] P[i+2] P[i+3]]T
6| 1  0  0 0|

Knowing MB and BB, solve Bz
since MB*BB = Mz*Bz, then Bz = (inverse of Mz)*MB*BB
*/
/*              |0  0   0  1|
|0  0  1/3 1|
inverse of Mz = |0 1/3 2/3 1|
|1  1   1  1|			*/

/*                   |1/6 2/3 1/6  0 |
| 0  2/3 1/3  0 |
(inverse of Mz)*MB = | 0  1/3 2/3  0 |
| 0  1/6 2/3 1/6|	*/

void CBsplineCurve::BSpline2Bezier(double *PBSpline, double *PBezier)
{
	PBezier[1] = (2*PBSpline[1] + PBSpline[2]) / 3;
	PBezier[2] = (2*PBSpline[2] + PBSpline[1]) / 3;
}

double CBsplineCurve::ComputeBezier(double u, double *P)
{
	double C0, C1, C2, C3, C;
	C3 = -P[0] + 3*P[1] - 3*P[2] + P[3];
	C2 = 3*P[0] - 6*P[1] + 3*P[2];
	C1 = -3*P[0] + 3*P[1];
	C0 = P[0];
	C = u*u*u*C3 + u*u*C2 + u*C1 + C0;
	return C;
}

void CBsplineCurve::InitControlPoints()
{
	if(dwCount >= 4)
	{
		double PBezierx[4], PBeziery[4];
		ComputeControlPoints(dwCount,
			Qx, Px);
		ComputeControlPoints(dwCount,
			Qy, Py);
		pt[0].x = (long)Qx[1];
		pt[0].y = (long)Qy[1];
		for(DWORD i = 0; i <= dwCount-2; i++)
		{
			BSpline2Bezier(Px+i, PBezierx);
			BSpline2Bezier(Py+i, PBeziery);
			pt[3*i+1].x = (long)PBezierx[1];
			pt[3*i+2].x = (long)PBezierx[2];
			pt[3*i+3].x = (long)Qx[i+2];
			pt[3*i+1].y = (long)PBeziery[1];
			pt[3*i+2].y = (long)PBeziery[2];
			pt[3*i+3].y = (long)Qy[i+2];
		}
	}
}


//------------------------------------virtual 函数－－－－－－－－－－
void CBsplineCurve::OnLButtonDown(CDC *pDC,CCAMCView *pView,CPoint point)
{
	if(pView->m_CurrentTool==BSPLINE_TOOL){
		if(nOperationID == APPEND)
		{	
			dwCount++;
			Qx[dwCount] = point.x;
			Qy[dwCount] = point.y;
			InitControlPoints();
			if(dwCount == 1)
				PtStatus[dwCount] = START;
			if(dwCount >= 2)
				PtStatus[dwCount] = END;
			if(dwCount > 2)
				PtStatus[dwCount-1] = NONE;
			pView->Invalidate();
		}
		if(nOperationID == SELECT_POINT)
		{
			PtStatus[nSeldPointID] = PrePtStatus;
			double x, y;
			x = point.x;
			y = point.y;
			double distance, MinDistance = 1e10;
			for(DWORD i = 1; i <= dwCount; i++)
			{
				double x1, y1;
				x1 = Qx[i] - x;
				y1 = Qy[i] - y;
				distance = x1*x1 + y1*y1;
				if(distance < MinDistance)
				{
					MinDistance = distance;
					nSeldPointID = i;
				}
			}
			if(MinDistance>25)
			{
				nSeldPointID=0;

			}
			PrePtStatus = PtStatus[nSeldPointID];
			PtStatus[nSeldPointID] = SELECTED;
			pView->Invalidate();
			//nOperationID=UPDATE;
			return;
		}
		if(nOperationID == UPDATE)
		{
			Qx[nSeldPointID] = point.x;
			Qy[nSeldPointID] = point.y;
			InitControlPoints();
			pView->Invalidate();
		}
		if(nOperationID == INSERT)
		{
			static double x, y;
			x = point.x;
			y = point.y;
			double MaxFactor = 0;
			double factor;
			DWORD i = 1;
			for(; i < dwCount; i++)
			{
				double x1, y1, x2, y2;
				x1 = Qx[i];
				y1 = Qy[i];
				x2 = Qx[i+1];
				y2 = Qy[i+1];
				double d1, d2, d12;
				d1 = (x1-x)*(x1-x) + (y1-y)*(y1-y);
				d2 = (x2-x)*(x2-x) + (y2-y)*(y2-y);
				d12 = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
				factor = (d12 - d1 - d2) / 2 / sqrt(d1*d2);
				if(factor > MaxFactor)
				{
					MaxFactor = factor;
					nSeldPointID = i;
				}
			}
			if(MaxFactor > 0.2)
			{
				tagPtStatus PtStatus1 = PtStatus[nSeldPointID];
				tagPtStatus PtStatus2 = PtStatus[nSeldPointID+1];
				PtStatus[nSeldPointID] = ADJACENT_INSERTED;
				PtStatus[nSeldPointID+1] = ADJACENT_INSERTED;
				pView->Invalidate();
				Draw(pDC);
				int nSel;
				nSel = MessageBox(pView->m_hWnd,"要在这两个点之间插入吗？",
					"confirm insert", MB_YESNO);

				PtStatus[nSeldPointID] = PtStatus1;
				PtStatus[nSeldPointID+1] = PtStatus2;

				if(nSel == IDYES)
				{
					// insert this point
					for(DWORD i = dwCount;
						i > nSeldPointID;
						i--)
					{
						Qx[i+1] = Qx[i];
						Qy[i+1] = Qy[i];
						PtStatus[i+1] = PtStatus[i];
					}
					Qx[i+1] = x;
					Qy[i+1] = y;
					PtStatus[i+1] = SELECTED;
					nSeldPointID = i+1;
					dwCount++;
					// recompute control points
					InitControlPoints();
					nOperationID = UPDATE;
					SetWindowText(pView->m_hWnd, Prompt[nOperationID]);
					PrePtStatus = NONE;
				}
				pView->Invalidate();
			}

		}
	}
}
void CBsplineCurve::OnRButtonDown(CDC *pDC, CCAMCView *pView,CPoint point)
{

	if(nSeldPointID>0){
		EnableMenuItem(m_popMenu.m_hMenu, IDM_DELETE_POINT, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_popMenu.m_hMenu, IDM_INSERT_POINT, MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		EnableMenuItem(m_popMenu.m_hMenu, IDM_DELETE_POINT, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(m_popMenu.m_hMenu, IDM_INSERT_POINT, MF_BYCOMMAND | MF_ENABLED);
	}
	ClientToScreen(pView->m_hWnd, &point);
	UINT uFlags;
	uFlags = TPM_LEFTALIGN | TPM_RIGHTBUTTON;
	m_popMenu.TrackPopupMenu(uFlags,point.x, point.y,pView, NULL);		
}
void CBsplineCurve::OnLButtonUp(CDC*pDC,CCAMCView*pView,CPoint point)
{
	FCObjCanvas& objCanvas = pView->GetCanvas();
	POINT ptoffset = pView->GetCanvasOffset();
	pView->m_CurrentDrawStatus = DEFAULT;
}
BOOL CBsplineCurve::DrawToImage(CCAMCView* pView,CPoint point)
{
	FCObjCanvas& objCanvas = pView->GetCanvas();
	
	POINT offset=pView->GetCanvasOffset();

	RGBQUAD rgb,rgbback;
	rgb.rgbBlue =GetBValue(m_PenColor);
	rgb.rgbGreen =GetGValue(m_PenColor);
	rgb.rgbRed = GetRValue(m_PenColor);
	rgb.rgbReserved = 0;
	rgbback.rgbBlue =255;
	rgbback.rgbGreen =255;
	rgbback.rgbRed = 255;
	rgbback.rgbReserved = 0;
	if(dwCount<=2){
		m_pointList.Add(CPoint(Qx[1],Qy[1]));	
		m_pointList.Add(CPoint(Qx[2],Qy[2]));	
	}else{
		DrawBSpline(dwCount,Qx,Qy);
	}
	for(int i=0;i<m_pointList.GetSize();i++)
	{
		objCanvas.MapRealPoint (m_pointList.GetAt(i));
	}
	objCanvas.MapRealPoint (offset);
	CBCStroke stroke;
	stroke.Init(CDHPaper::Instance(),m_PenColor);
	ToBezierStroke(stroke);
	return TRUE;	
}

void CBsplineCurve::Draw(CDC* pDC)
{
	CPen m_pen,pen2;
	m_pen.CreatePen(m_PenStyle,m_PenWidth,m_PenColor);
	pen2.CreatePen(m_PenStyle,m_bGWidth,RGB(0,255,255));
	CPen* pen=(CPen*)pDC->SelectObject(&pen2);
	HDC hdc=pDC->m_hDC;
	for(DWORD i = 1; i <= dwCount; i++)
		PutPoint(hdc,i);					
	if(dwCount == 2)
	{
		MoveToEx(hdc, (int)Qx[1], (int)Qy[1], (LPPOINT)NULL);
		LineTo(hdc, (int)Qx[2], (int)Qy[2]); 
		pDC->SelectObject(&m_pen);
		MoveToEx(hdc, (int)Qx[1], (int)Qy[1], (LPPOINT)NULL);
		LineTo(hdc, (int)Qx[2], (int)Qy[2]); 
	}
	if(dwCount == 3)
	{
		pt[0].x = (long)Qx[1];
		pt[1].x = (long)(4*Qx[2] - Qx[3]) / 3;
		pt[2].x = (long)(4*Qx[2] - Qx[1]) / 3;
		pt[3].x = (long)Qx[3];
		pt[0].y = (long)Qy[1];
		pt[1].y = (long)(4*Qy[2] - Qy[3]) / 3;
		pt[2].y = (long)(4*Qy[2] - Qy[1]) / 3;
		pt[3].y = (long)Qy[3];
		PolyBezier(hdc, pt, 4);
		pDC->SelectObject(&m_pen);
		PolyBezier(hdc, pt, 4);
	}
	if(dwCount>= 4){
		PolyBezier(hdc, pt, 3*dwCount-2);
		pDC->SelectObject(&m_pen);
		PolyBezier(hdc, pt, 3*dwCount-2);
	}
	pDC->SelectObject(pen); 
}

void CBsplineCurve::OnInsertPoint(CView* pView)
{
	nOperationID = INSERT;
	SetWindowText(pView->m_hWnd, Prompt[nOperationID]);
}

void CBsplineCurve::OnDeletePoint(CView* pView)
{
	nOperationID = DELETe;
	SetWindowText(pView->m_hWnd, Prompt[nOperationID]);
	int nSel;
	nSel = MessageBox(pView->m_hWnd, "要删除该点吗？",
		"confirm delete", MB_YESNO);
	if(nSel == IDYES)
	{
		// delete this point
		if(nSeldPointID == dwCount)
		{	// delete the END point,
			// so set its immediate predecessor to END
			if(dwCount> 2)
				PtStatus[nSeldPointID-1] = END;
		}
		else
			for(DWORD i = nSeldPointID;i < dwCount;i++)
			{
				Qx[i] = Qx[i+1];
				Qy[i] = Qy[i+1];
				PtStatus[i] = PtStatus[i+1];
				if(nSeldPointID == 1)
				{	// delete the START point,
					// so set its immediate successor to START
					if(dwCount >= 2)
						PtStatus[1] = START;
				}
			}
			dwCount--;
			InitControlPoints();							
			InvalidateRect(pView->m_hWnd, NULL, TRUE);
	}
}
void CBsplineCurve::OnPassingpointsSelect(CView* pView)
{
	nOperationID = SELECT_POINT;
	SetWindowText(pView->m_hWnd, Prompt[nOperationID]);
}
void CBsplineCurve::OnUpdatePoint(CView* pView)
{
	nOperationID = UPDATE;
	SetWindowText(pView->m_hWnd, Prompt[nOperationID]);
}
// 结束当前B样条曲线的绘制，并且把当前B样条曲线写入图像中。
void CBsplineCurve::OnEndPoints(CView* pView)
{
	nOperationID = NOP;
	SetWindowText(pView->m_hWnd, Prompt[nOperationID]);
	CCAMCView* pV=static_cast<CCAMCView*>(pView);
	DrawToImage(pV,GetGraphObjPos());
}

void CBsplineCurve::ShowSelectPoint(CDC*pDC)
{
	CRect rect;
	CBrush brush;
	brush.CreateSolidBrush(RGB(255,0,0));
	CPen m_pen;
	m_pen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	CPen *OldPen=pDC->SelectObject(&m_pen);
	int oldBkMode=pDC->SetBkMode(OPAQUE); 
	CBrush *OldBrush=pDC->SelectObject(&brush);
	int oldDrawingMode=pDC->SetROP2(R2_NOTXORPEN);
	for(int i=1;i<=dwCount;++i)
	{
		rect=CRect(CPoint(Qx[i],Qy[i]),CPoint(Qx[i],Qy[i]));
		rect.InflateRect(3,3);
		pDC->Rectangle(rect);
	}
	pDC->SelectObject(OldPen);
	pDC->SetBkMode(oldBkMode); 
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(oldDrawingMode);	
}
BOOL CBsplineCurve::IsInRgn(CPoint point)
{
	CRect rect;
	for(int i=1;i<=dwCount;++i){			
		rect=CRect(CPoint(Qx[i],Qy[i]),CPoint(Qx[i],Qy[i]));
		rect.InflateRect(3,3);
		if(rect.PtInRect(point))
			return TRUE;
	}
	return FALSE;
}
void CBsplineCurve::OnMouseMove(CDC *pDC, CCAMCView *pView, CPoint point)
{

}
BOOL CBsplineCurve::IsEnd(int tool_type){
	if(nOperationID==NOP&&tool_type==BSPLINE_TOOL)
		return TRUE;
	return FALSE;
}
void CBsplineCurve::ToBezierStroke(CBCStroke& stroke)
{
	int size=m_pointList.GetSize();
	int i=0;
	for(;i<size;i+=10)
	{		
		CPoint p=m_pointList.GetAt(i);
		TPosition position;
		position.m_x=p.x;
		position.m_y=p.y;
		position.m_angle_xy=90;
		position.m_angle_z=90;
		position.press=m_PenWidth;
		stroke.AddPoint(position);
	}
	if((i-9)<size){
		CPoint p=m_pointList.GetAt(size-1);
		TPosition position;
		position.m_x=p.x;
		position.m_y=p.y;
		position.m_angle_xy=90;
		position.m_angle_z=90;
		position.press=m_PenWidth;
		stroke.AddPoint(position);
	}
	stroke.SetPenWidth(m_PenWidth);
}