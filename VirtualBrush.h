#pragma once
#include "objbase.h"
#include "Shape.h"
#include "VirtualPaper.h"
#include "VBDefine.h"
#include "GraphicsGems.h"
/***************************
*作者：刘建明
*原始完成日期：2006.9.26
*最近修改日期：2006.10.13
*用途：实现虚拟的笔刷功能 
*作者本人修订日期：
*其他人修订（包括修改者，修改日期，修改地方，修改前的内容，修改后内容，作用等）      
***************************/

struct BrushAttr{
	double  m_water;       //毛笔中水分子含量	         
	Pigment m_pigment;     //毛笔中墨分子含量
	int     m_bristleNum;  //最大毛发数，决定能产生的最大笔触，在这里，它是由最大笔触面（用户设定）反算出来的。

	//笔刷风格控制参数
	BOOL    m_isInkReduce; //控制笔刷绘制过程中是否减少墨水
	CShape  m_shape;
};
class CVirtualBrush :
	public FCObjGraph
{
public:
	CVirtualBrush(){m_isInkReduce=true;}
	virtual ~CVirtualBrush(void){}
	static CVirtualBrush* Instance();
//protected:
public:
	BrushWork     m_brushWork;//利用8位的灰度图的mask区域来表示最大笔触
	BWLIST        m_bwList;//定制产生的从da到xiao的某种笔触系列
	static CVirtualBrush* _instance;
	//--------------------------------------------------------------------------
private:
	//我们把墨水分为水分子和颜料分子。
	double  m_water;       //毛笔中水分子含量	         
    Pigment m_pigment;     //毛笔中墨分子含量
	int     m_bristleNum;  //最大毛发数，决定能产生的最大笔触，在这里，它是由最大笔触面（用户设定）反算出来的。

	//笔刷风格控制参数
	BOOL    m_isInkReduce; //控制笔刷绘制过程中是否减少墨水
	CShape  m_shape;
protected:
	
public:
	void DrawSection(CVirtualPaper& paper,Point2 p,double press);
	int  GetBrushWork(double press);
	void Customizing(CShape& shape);
	void DipInk(Ink& ik,double brlInkSuck){//浸渍墨水
		m_pigment.m_pigAttr=ik.m_pigAttr;
		m_pigment.m_quan=(brlInkSuck*ik.m_percentage)*m_bristleNum;
		m_water=m_bristleNum*brlInkSuck-m_pigment.m_quan;
	}
	BOOL IsInkReduce(){return m_isInkReduce;}
	void SetInkReduceOrNot(BOOL isInkReduce){m_isInkReduce=isInkReduce;}
	void Init(CShape& shape,Ink& ik,double brlInkSuck){
		Customizing(shape);
		DipInk(ik,brlInkSuck);
	}
	void BeginOneStrokeDraw(FCObjImage* img,CVirtualPaper& paper,RECT& rcInPaper);
	void EndOneStrokeDraw(CVirtualPaper& paper);
	void GetBrushAttribute(BrushAttr& bAttr){
		bAttr.m_bristleNum=m_bristleNum;
		bAttr.m_isInkReduce=m_isInkReduce;
		bAttr.m_pigment=m_pigment;
		bAttr.m_shape=m_shape;
		bAttr.m_water=m_water;
	}
};
