/**********************************************
 *类名： CDHStroke
 *作用： 表示虚拟笔刷的轨迹
 *作者： 刘建明
 *更新时间：2005－2－21
 **********************************************/


#pragma once

#include "objbase.h"
#include "GraphicsGems.h"
#include "objImage.h"
#include <vector>
#include "VirtualBrush.h"

typedef struct tagTPosition{
	int		m_x;
	int		m_y;
	double press;
	int		m_angle_xy;//与xy平面所成的角
	int		m_angle_z;//在xy平面投影y轴所成的角，即笔刷旋转角度
}TPosition;

typedef struct BrushAttributeStruct{
	int press;
	int m_angle_xy;//与xy平面所成的角
	int m_angle_z;//在xy平面投影y轴所成的角，即笔刷旋转角度		
}BAttri;

typedef std::vector<BAttri>  BATTRILIST;
typedef std::vector<Point2>  DISCURVE;//用于存储离散的点表示的曲线

class CParentPaper;

class CDHStroke :
	public FCObject
{
public:
	CDHStroke(void);
	~CDHStroke(void);
public:
	void InsertStrokePiece(CDC* pDC,TPosition& p);
	void Init(CParentPaper* lpPaper,COLORREF c){
		m_lpPaper=lpPaper;
		m_c=c;
		m_penWidth=0;
		m_id=0;
	}
	void Init(CVirtualPaper* lpPaper,COLORREF c){
		//m_vb.init(lpPaper);
	}
	virtual void Clear(){
		if(m_disCurve.empty()==false){
			m_disCurve.clear();
			m_pressList.clear();
		}
	}
	virtual void DoFitCurve(double error=3.0){}
	virtual void Draw(FCObjImage& img){}
	virtual void Draw(FCObjImage& img,BYTE alpha){}
	virtual void Draw(CDC* pDC){}
	DISCURVE&	GetDisCurve(){return m_disCurve;}
	BATTRILIST& GetAttriList(){return m_pressList;}
	COLORREF    GetColor(){return m_c;}
	void        SetColor(COLORREF color)
	{
		m_c = color;
	}
	void        SetId(long id){m_id=id;}
	long		getId(){return m_id;}
	int GetPenWidth(){ return m_penWidth;}
	void SetPenWidth(int w){ m_penWidth=w;} 
	BOOL		IsErase(){return m_isErase;}
	CParentPaper	*GetPaper(){return m_lpPaper;}
public:
	void Integer_Bresenham_Line(CDC* pDC,TPosition& p1,TPosition& p2);
	void Integer_DDA_Line(CDC* pDC,TPosition& p1,TPosition& p2);
	void AddPoint(TPosition& p){
		Point2 p2;
		p2.x=p.m_x;
		p2.y=p.m_y;
		m_disCurve.push_back(p2);
		BAttri bAttri;
		bAttri.press=p.press;
		bAttri.m_angle_xy=p.m_angle_xy;
		bAttri.m_angle_z=p.m_angle_z;
		m_pressList.push_back(bAttri);
	}

protected:
	DISCURVE		m_disCurve;//离散点表示的曲线	
	BATTRILIST		m_pressList;
	CParentPaper	*m_lpPaper;
	int             m_penWidth;
	COLORREF		m_c;
	BOOL			m_isErase;
	long			m_id;
	//CVirtualBrush   m_vb;//虚拟画刷
};
