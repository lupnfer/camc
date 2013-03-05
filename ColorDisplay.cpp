#include "StdAfx.h"
#include ".\colordisplay.h"

CColorDisplay::CColorDisplay(void)
{
}

CColorDisplay::~CColorDisplay(void)
{
}

void  CColorDisplay::ComputeReflectance(double R,double x,double& Ref,double& T){
	if(x<0)
		x=0;
	double K=(1-R)*(1-R)/(2*R);
	double a=(1+K);
	double b=sqrt(double(a*a-1));
	double ch=cosh(double(b*x));
	double c=a*sinh(double(b*x))+b*cosh(double(b*x));

	Ref=sinh(double(b*x)/c);
	T=b/c;
}

void CColorDisplay::ComputeTwoLayerMixReflectance(double xR1,double xT1,double xR2,double xT2,double& xR,double& xT)
{
	double r1r2=xR1*xR2;
	if(r1r2==1)
		return;
	xR=xR1+(xT1*xT1)*xR2/(1-r1r2);
	xT=xT1*xT2/(1-r1r2);
};

/*
void  CColorDisplay::ComputeTwoLayerMixReflectance(double R1,double T1,double R2,double T2,double& R,double& T)
{
	double r1r2=R1*R2;
	if(r1r2==1)
		return;
	R=R1+(T1*T1)*R2/(1-r1r2);
	T=T1*T2/(1-r1r2);
}
*/
void CColorDisplay::FromColorToReflectance(BYTE r,BYTE g,BYTE b,double& RR,double& GR,double& BR){
	RR=r/double(255);
	GR=g/double(255);
	BR=b/double(255);
}
void CColorDisplay::FromRefToColor(double RR,double GR,double BR,BYTE light,BYTE& r,BYTE& g,BYTE& b){
	r=RR*light;
	g=GR*light;
	b=BR*light;
}
void CColorDisplay::DrawColor(PIGMENTLIST& cbList,BYTE& r,BYTE& g,BYTE& b,BYTE light){
	int size=cbList.size();
	PIGMENTLIST::reverse_iterator it=cbList.rbegin();
	double RRef,RT,GRef,GT,BRef,BT;
	double RRef1,RT1,GRef1,GT1,BRef1,BT1;
	if(it!=cbList.rend()){			
		ComputeReflectance(it->m_pigAttr.R_reflectance,it->m_quan,RRef1,RT1);
		ComputeReflectance(it->m_pigAttr.G_reflectance,it->m_quan,GRef1,GT1);
		ComputeReflectance(it->m_pigAttr.B_reflectance,it->m_quan,BRef1,BT1);
		RRef=RRef1;RT=RT1;
		GRef=GRef1;GT=GT1;
		BRef=BRef1;BT=BT1;
	}
	it++;
	for(;it!=cbList.rend();++it){			
		double RRef2,RT2,GRef2,GT2,BRef2,BT2;
		ComputeReflectance(it->m_pigAttr.R_reflectance,it->m_quan,RRef2,RT2);
		ComputeReflectance(it->m_pigAttr.G_reflectance,it->m_quan,GRef2,GT2);
		ComputeReflectance(it->m_pigAttr.B_reflectance,it->m_quan,BRef2,BT2);
		ComputeTwoLayerMixReflectance(RRef1,RT1,RRef2,RT2,RRef,RT);
		ComputeTwoLayerMixReflectance(GRef1,GT1,GRef2,GT2,GRef,GT);
		ComputeTwoLayerMixReflectance(BRef1,BT1,BRef2,BT2,BRef,BT);
		RRef1=RRef;RT1=RT;
		GRef1=GRef;GT1=GT;
		BRef1=BRef;BT1=BT;
	}
	ComputeTwoLayerMixReflectance(RRef1,RT1,1,0,RRef,RT);
	ComputeTwoLayerMixReflectance(GRef1,GT1,1,0,GRef,GT);
	ComputeTwoLayerMixReflectance(BRef1,BT1,1,0,BRef,BT);
	FromRefToColor(RRef,GRef,BRef,light,r,g,b);
}