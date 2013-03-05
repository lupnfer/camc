#include "StdAfx.h"
#include ".\parentpaper.h"
#include "FColor.h"
#include "GraphicsGems.h"

extern double V2Length(Vector2* v);
void  aggFillPolygon_Normal_8Bit (FCObjImage & img, const POINT * ppt, int cNumPoint);

CParentPaper::~CParentPaper(void)
{
}


//当与纸张的截面是圆形且产生渐变的效果
void CParentPaper::GenTexture(LPSECTION lpSection,int r,COLORREF c)
{
	
}
COLORREF CParentPaper::AlphaBlendPixel(COLORREF bgColor,COLORREF fgColor,int alphaPecent)
{
	RGBQUAD rgb1,rgb2;
	rgb1.rgbBlue=GetBValue(bgColor);
	rgb1.rgbGreen=GetGValue(bgColor);
	rgb1.rgbRed=GetRValue(bgColor);
	rgb1.rgbReserved=alphaPecent;
	rgb2.rgbBlue=GetBValue(fgColor);
	rgb2.rgbGreen=GetGValue(fgColor);
	rgb2.rgbRed=GetRValue(fgColor);
	FCColor::AlphaBlendPixel(&rgb2,&rgb1);
	return RGB(rgb2.rgbRed,rgb2.rgbGreen,rgb2.rgbBlue);	
}
void CParentPaper::DrawSection(CDC *pDC,TPosition& p,COLORREF c)
{

}
void CParentPaper::DrawSection(FCObjImage& img,TPosition& p,BYTE alpha)
{

}
void CParentPaper::DrawSection(FCObjImage& img,TPosition& p1,Vector2 v1,TPosition& p2,Vector2 v2,BYTE alpha)
{
	double r=(p1.press+0.5)/2;
	Point2 pArray[4]; 
	double u=V2Length(&v1);
	if(u!=0)
		u=r/u;
	pArray[0].x=p1.m_x+u*v1.x;
	pArray[0].y=p1.m_y+u*v1.y;
	pArray[1].x=p1.m_x-u*v1.x;
	pArray[1].y=p1.m_y-u*v1.y;

	r=(p2.press+0.5)/2;
	u=V2Length(&v2);
	if(u!=0)
		u=r/u;
	pArray[3].x=p2.m_x+u*v2.x;
	pArray[3].y=p2.m_y+u*v2.y;
	pArray[2].x=p2.m_x-u*v2.x;
	pArray[2].y=p2.m_y-u*v2.y;

	POINT pt1,pt2;
	pt1.x=p1.m_x;
	pt1.y=p1.m_y;
	pt2.x=p2.m_x;
	pt2.y=p2.m_y;

	if(CParentPaper::IsOnSameSide(pt1,pt2,pArray[1],pArray[2])==false)
	{
		Point2 tmp;
		tmp=pArray[2];
		pArray[2]=pArray[3];
		pArray[3]=tmp;
	}

	POINT pt[4];
	for(int i=0;i<4;++i){
		pt[i].x=pArray[i].x;
		pt[i].y=pArray[i].y;
	}
	aggFillPolygon_Normal_8Bit(img,pt,4);
}
BOOL CParentPaper::IsOnSameSide(POINT p1,POINT p2,Point2 pA,Point2 pB)
{

	int a=p1.y-p2.y;
	int b=(p2.x-p1.x);
	int c=p1.x*p2.y-p2.x*p1.y;	
	return ((((a*pA.x+b*pA.y+c)>=0)&&((a*pB.x+b*pB.y+c)>=0))
		||(((a*pA.x+b*pA.y+c)<=0)&&((a*pB.x+b*pB.y+c)<=0)));

}