#include "StdAfx.h"
#include "DIBApi.h"


#include <algorithm>
#include <math.h>



void WINAPI  RGB_to_HSV(BYTE rb, BYTE gb,BYTE bb,
				 double &h, double &s, double &v)
{

	float m,n,delta;
	float r,g,b;
	r=float(rb/255.0);
	g=float(gb/255.0);
	b=float(bb/255.0);
	m=max(r,max(g,b));
	n=min(r,min(g,b));
	v=m;
	if(m!=0)
		s=(m-n)/m;
	else
		s=0;
	if(s==0)
		h=-1;//represents undefined
	else
	{
		delta=m-n;
		if(r==m)
			h=(g-b)/delta;
		else if(g==m)
			h=2+(b-r)/delta;
		else if(b==m)
			h=4+(r-g)/delta;
		h=h*60;
		if(h<0)
			h=h+360;
	}//end else
}

void WINAPI  HSV_to_RGB(double h, double s, double v,
						BYTE &r, BYTE &g, BYTE &b)
{
	int i;
	double f,p,q,t;
	double R,G,B;
	if(s!=0)
	{
		if(h==360) h=0;
		h/=60;
		i=(int)h;
		f=h-i;
		p=v*(1-s);
		q=v*(1-s*f);
		t=v*(1-(s*(1-f)));
		switch(i)
		{
		case 0:
			R=v;
			G=t;
			B=p;
			break;
		case 1:
			R=q;
			G=v;
			B=p;
			break;
		case 2:
			R=p;
			G=v;
			B=t;
			break;
		case 3:
			R=p;
			G=q;
			B=v;
			break;
		case 4:
			R=t;
			G=p;
			B=v;
			break;
		case 5:
			R=v;
			G=p;
			B=q;
			break;
		}//end switch
		r=R*255;
		g=G*255;
		b=B*255;
	}//end if
}

void WINAPI  RGB_to_HLS(BYTE r, BYTE g, BYTE b,double &h, double &l, double &s)
{
	double R,G,B,H,L,S,max,min,delta;
	R=(double)r/255.0;
	G=(double)g/255.0;
	B=(double)b/255.0;
	max=R;
	if(max<G) max=G;
	if(max<B) max=B;
	min=R;
	if(min>G) min=G;
	if(min>B) min=B;
	L=(max+min)/2.0;
	if(max==min)
	{
		S=0;
		H=-1.0;
	}
	else
	{
		delta=max-min;

		if(L<0.5) 
			S=delta/(max+min);
		else
			S=delta/(2.0-max-min);
		if(R==max)
			H=(G-B)/delta;
		else
		{
			if(G==max)
				H=2.0+(B-R)/delta;
			else
				if(B==max) H=4+(R-G)/delta;
		}
		H*=60.0;
		if(H<0.0) H=H+360.0;
	}
	h=H;
	l=L;
	s=S;
}

void WINAPI  HLS_to_RGB(double h, double l, double s,
						BYTE &r, BYTE &g, BYTE &b)
{
	BYTE R1,B1,G1;
	double R,G,B,H,L,S,m1,m2;
	H=h;
	L=l;
	S=s;
	if(L<=0.5)
		m2=L*(1.0+S);
	else
		m2=L+S-L*S;
	m1=2.0*L-m2;
	if(S==0.0)
	{
		if(H==-1.0)
		{
			R=L;
			G=L;
			B=L;
		}
		else
		{
			R=0.0;
			B=0.0;
			G=0.0;
		}
	}
	else
	{
		R=value(m1,m2,H+120.0);
		G=value(m1,m2,H);
		B=value(m1,m2,H-120.0);
	}
	R1=(int)(R*255.0);
	G1=(int)(G*255.0);
	B1=(int)(B*255.0);
	if(R1>255.0) R1=255;
	if(G1>255.0) G1=255;
	if(B1>255.0) B1=255;
	if(R1<0) R1=0;
	if(G1<0) G1=0;
	if(B1<0) B1=0;
	r=(unsigned char) R1;
	g=(unsigned char) G1;
	b=(unsigned char) B1;
}

double WINAPI ComputeHSVSimilarity(float h1, float s1, float v1, float h2, float s2, float v2)
{
   double sim;
   float arc=float(3.1415926535/180);
   
   h1=h1*arc;
   h2=h2*arc; 
   double huesqr=(v1-v2)*(v1-v2);
   double cossqr=(s1*cos(h1)-s2*cos(h2))*(s1*cos(h1)-s2*cos(h2));
   double sinsqr=(s1*sin(h1)-s2*sin(h2))*(s1*sin(h1)-s2*sin(h2));
   sim=1-sqrt((huesqr+cossqr+sinsqr)/3);
   return sim; 
}

double WINAPI ComputeRGBSimilarity(int r1, int g1, int b1, int r2, int g2, int b2)
{
	double sim;
	sim=(r1-r2)*(r1-r2)+(g1-g2)*(g1-g2)+(b1-b2)*(b1-b2);
	sim/=255*255*3;
	sim=1-sqrt(sim);
	return sim; 
}
double WINAPI ComputeRGBDifference(int r1, int g1, int b1, int r2, int g2, int b2)
{
	double sim;
	sim=(r1-r2)*(r1-r2)+(g1-g2)*(g1-g2)+(b1-b2)*(b1-b2);
	//sim/=255*255*3;
	sim=sqrt(sim);
	return sim; 
}
void WINAPI ConvertRGB(BYTE ro, BYTE go, BYTE bo, BYTE rn, BYTE gn, BYTE bn,
					   BYTE r1, BYTE g1, BYTE b1, BYTE &r, BYTE &g, BYTE &b)
{
	r=r1+(ro-rn);
	g=g1+(go-gn);
	b=b1+(bo-bn);
}

void WINAPI ConvertHSV(double ho, double so, double vo, double hn, double sn, double vn,
					   double h1, double s1, double v1, double &h, double &s, double &v)
{
	h=h1+(ho-hn);
	s=s1+(so-sn);
	v=v1+(vo-vn);
}


double value(double n1,double n2,double hue)
{
	if(hue>360.0) hue-=360.0;
	if(hue<0.0) hue+=390.0;
	if(hue<60.0) return(n1+(n2-n1)*hue/60.0);
	if(hue<180.0) return n2;
	if(hue<240.0) return (n1+(n2-n1)*(240.0-hue)/60.0);
	return (n1);
}
void WINAPI  RGB_to_GRAY(BYTE r,BYTE g,BYTE b,BYTE& gray)
{
	gray=(BYTE)(r*0.299+g*0.587+b*0.114);
}