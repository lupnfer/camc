#pragma once

#include "dhstroke.h"
#include "GraphicsGems.h"
#include "DHPaper.h"
#include <vector>
#include <list>
#include <algorithm>
#include "objImage.h"
#include "Shape.h"
/* 功能作用：实现分段Bezier曲线表示的轨迹,。采用论文：
 *			 An Algorithm for Automatically Fitting Digitized Curves
 *			 by Philip J. Schneider from "Graphics Gems", Academic Press, 1990
 * 最近更新：2005.4.15
 * 作者：    刘建明
 */
#define MAXPOINTS	1000		/* The most points you can have */
typedef Point2 *BezierCurve;

struct PARAMER{
	double u;
	double press;
	int    m_angle_xy;
	int    m_angle_z;
	PARAMER():u(0.0),press(0),m_angle_xy(0),m_angle_z(90){}
	static bool Less(PARAMER& p1,PARAMER& p2){
		return (p1.u<p2.u);
	}
};

typedef std::vector<PARAMER> PARAMERLIST;

//包涵压力参数的Bezier曲线结构
class BezierPressCurve{
public:
	int			numpoint;//绘 制这条曲线时，所绘的点的个数。
	int			degree;
	BezierCurve bezCurve;
	PARAMERLIST		uList;//带有压力的参数列表，按参数从大到小排列，用于计算绘制Bezier曲线时计算绘制的每一点压力(线性插值)
public:
	BezierPressCurve():numpoint(0),degree(0),bezCurve(NULL){}
	BezierPressCurve(const BezierPressCurve& bezpCurve);
	~BezierPressCurve(){
		if(bezCurve!=NULL)
			free(bezCurve);
	}
	double GetLengthFromParam(double t,double t_begin=0.0,double error=2);
    double GetParamFromLength(double len,double error=2.0);	
};
typedef std::vector<BezierPressCurve> BEZIERSTROKE; 

class FCObjImage;
class CBCStroke: public CDHStroke
{
public:
	CBCStroke(void);
	CBCStroke(CBCStroke& stroke);
	~CBCStroke(void);
	virtual void Clear(){
		if(m_disCurve.empty()==false){
			m_disCurve.clear();
			m_pressList.clear();
			m_bezStroke.clear();
		}
	}
	void DestroyObject(){
		m_disCurve.clear();
		m_pressList.clear();
		m_bezStroke.clear();
	}
public:
	virtual void		DoFitCurve(double error=5){
							int size=m_disCurve.size();
							FitCurve(m_disCurve,size,error);
						}
	void				FitCurve(DISCURVE& d,int nPts,double error);
	void				FitCubic(DISCURVE& d,int	first,
								  int last,Vector2 tHat1,
								  Vector2 tHat2,double	error);

	void				AddBezierCurve(int degress,BezierCurve bezCurve,PARAMERLIST& paramList);
	void	            CopyParamList(int first,int last,double* u,PARAMERLIST& paramList);
	virtual void		Draw(CDC* pDC);
	//virtual void        Draw(FCObjImage& img);
	virtual void		Draw(FCObjImage& img,float zoompaper=1,float zoompress=1);
	virtual void        Draw(FCObjImage& img,BYTE alpha,float zoompaper=1,float zoompress=1);
	
	//void				DrawBezierCurve(CVirtualPaper& paper,BezierPressCurve& bezpCurve,float zoompaper=1,float zoompress=1);
	
	
	void				DrawBezierCurve(CDC* pDC,BezierPressCurve& bezpCurve);
	void				DrawBezierCurve(FCObjImage& img,BezierPressCurve& bezpCurve,BYTE alpha,float zoompaper=1,float zoompress=1);
	//void				DrawBezierCurve(FCObjImage& img,BezierPressCurve& bezpCurve,float zoompaper=1,float zoompress=1);
	
	void				DrawHarryBrush(FCObjImage& img,BezierPressCurve& bezpCurve,BYTE alpha,float zoom=1);
	static  void		InsertParamer(PARAMER& p,PARAMERLIST& paramList);
	void                Zoom(double ratio,double zoompress);
	//void                ReDraw(FCObjImage& img);
public:
	static	double		*Reparameterize(DISCURVE& d,/*  Array of digitized points	*/
										int	first,int last,		/*  Indices defining region	*/
										double	*u,			/*  Current parameter values	*/
										BezierCurve	bezCurve);	/*  Current fitted curve	*/
	static	double		NewtonRaphsonRootFind(BezierCurve	Q,			/*  Current fitted curve	*/
											  Point2 		P,		/*  Digitized point		*/
											  double 		u);		/*  Parameter value for "P"	*/
	static	Point2		BezierII(int degree,		/* The degree of the bezier curve	*/
								 Point2 *V,		/* Array of control points		*/
								 double  t);	/* Parametric value to find point for	*/
	static	double 		B0(double u);
	static	double      B1(double u);
	static	double      B2(double u);
	static  double      B3(double u);
	static	Vector2		ComputeLeftTangent(DISCURVE&  d,			/*  Digitized points*/
											int		end);		/*  Index to "left" end of region */
	static	Vector2		ComputeRightTangent(DISCURVE& d,			/*  Digitized points*/
											int		end);		/*  Index to "right" end of region */
	static	Vector2		ComputeCenterTangent(DISCURVE& d,			/*  Digitized points*/
											 int	center);		/*  Index to "center" end of region */
	static	double		ComputeMaxError(DISCURVE& d,			/*  Array of digitized points	*/
										int		first,int last,		/*  Indices defining region	*/
										BezierCurve	bezCurve,		/*  Fitted Bezier curve		*/
										double	*u,			/*  Parameterization of points	*/
										int		*splitPoint);		/*  Point of maximum error	*/
	static	double		*ChordLengthParameterize(DISCURVE& d,			/* Array of digitized points */
												 int   first, 
												 int   last);		/*  Indices defining region	*/
	static	BezierCurve	GenerateBezier(DISCURVE& d,			/*  Array of digitized points	*/
									   int		first,int last,		/*  Indices defining region	*/
									   double	*uPrime,		/*  Parameter values for region */
									   Vector2	tHat1,Vector2 tHat2);	/*  Unit tangents at endpoints	*/
	static	Vector2		V2AddII(Vector2 a,Vector2 b);
	static	Vector2		V2ScaleIII(Vector2 v,double s);
	static	Vector2		V2SubII(Vector2	a,Vector2 b);
	void	SetRect(RECT rect){
		m_rcSave.bottom=rect.bottom;
		m_rcSave.left=rect.left;
		m_rcSave.right=rect.right;
		m_rcSave.top=rect.top;
	}
	RECT GetRect()
	{
		return m_rcSave;
	}
	BEZIERSTROKE& GetStroke(){return m_bezStroke;}
	BOOL IsInRgn(CPoint p);
	//void UpDateUndo(FCObjImage& imgSrc,RECT rcSrc);
	void SetBrushAttribute(BrushAttr& bAttr){
		m_bAttr.m_bristleNum=bAttr.m_bristleNum;
		m_bAttr.m_isInkReduce=bAttr.m_isInkReduce;
		m_bAttr.m_pigment=bAttr.m_pigment;
		m_bAttr.m_water=bAttr.m_water;
		m_bAttr.m_shape=bAttr.m_shape;
	}
public:
	BEZIERSTROKE	m_bezStroke;//分段bezier曲线表示的笔画轨迹
	
	RECT			m_rcSave;
	double          m_zoompaper;
	double			m_zoompress;

	//FCObjImage		m_Undo ;
	CBCStroke*		m_lpStrokeNext;
	CBCStroke*      m_lpStrokePre;
protected:
	BrushAttr       m_bAttr;
};
