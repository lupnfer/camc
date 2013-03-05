#pragma once

#include "objbase.h"
#include "VBDefine.h"
/***************************
*���ߣ�������
*ԭʼ������ڣ�2006.9.26
*��;��ɫ�ʵĻ��� ����KLģ��
*���߱����޶����ڣ�
*�������޶��������޸��ߣ��޸����ڣ��޸ĵط����޸�ǰ�����ݣ��޸ĺ����ݣ����õȣ�      
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
