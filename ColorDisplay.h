#pragma once

#include "objbase.h"
#include "VBDefine.h"
/***************************
*作者：刘建明
*原始完成日期：2006.9.26
*用途：色彩的绘制 基于KL模型
*作者本人修订日期：
*其他人修订（包括修改者，修改日期，修改地方，修改前的内容，修改后内容，作用等）      
***************************/
class CColorDisplay 
{
public:
	CColorDisplay(void);
	virtual ~CColorDisplay(void);
public:
	static void ComputeTwoLayerMixReflectance(double xR1,double xT1,double xR2,double xT2,double& xR,double& xT);
	static void ComputeReflectance(double R,double x,double& Ref,double& T);	
	static void FromColorToReflectance(BYTE r,BYTE g,BYTE b,double& RR,double& GR,double& BR);
	static void FromRefToColor(double RR,double GR,double BR,BYTE light,BYTE& r,BYTE& g,BYTE& b);
	static void DrawColor(PIGMENTLIST& cbList,BYTE& r,BYTE& g,BYTE& b,BYTE light=255);
};
