#pragma once
#include "objbase.h"
#include "objSelect.h"
#include "VBDefine.h"
/***************************
*作者：刘建明
*原始完成日期：2006.9.26
*用途：笔刷形状的定义，用于实现各种风格形状的笔刷 
*作者本人修订日期：
*其他人修订（包括修改者，修改日期，修改地方，修改前的内容，修改后内容，作用等）      
***************************/

struct BrushWork{
	FCObjSelect m_mask;
	int         m_maskNum;
};
typedef std::vector<BrushWork> BWLIST;//多种颜料的含量列表
class CShape :
	public FCObject
{
public:
	CShape();
	CShape(const CShape& shape){
		m_a=shape.m_a;
		m_b=shape.m_b;
		m_angle=shape.m_angle;
	}
	virtual ~CShape(void);
protected:
	float m_a;//椭圆的横轴
	float m_b;//椭圆的纵轴
	int m_angle;

public:
	void Init(float a,float b,int angle){
		m_a=a;
		m_b=b;
		m_angle=angle;
	}
	float GetA(){return m_a;}
	float GetB(){return m_b;}
	float GetAngle(){return m_angle;}
	virtual int  GenShape(BrushWork& bw);//产生笔触，并返回毛发数量
	virtual bool  IsValid(){	if(m_a>0&&m_b>0) return true; return false;	}

};
