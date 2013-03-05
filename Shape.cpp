#include "StdAfx.h"
#include ".\shape.h"
#include <vector>
#include "GraphicsGems.h"
#include "FColor.h"

CShape::CShape(void)
{
}

CShape::~CShape(void)
{
}
int CShape::GenShape(BrushWork& bw)
{
	BOOL flag=false;
	FCObjImage img;
	int w=2*m_a+0.5;
	int h=2*m_b+0.5;
	if(m_a<m_b){
		std::swap(w,h);
		std::swap(m_a,m_b);
		flag=true;
	}
	img.Create (w,h, 32) ;	
	::FillMemory (img.GetMemStart(), img.GetPitch()*img.Height(),0x00) ;
	std::vector<Point2> pList;
	std::vector<Point2>::iterator it;
	MidpointEllipse(m_a,m_b,pList);
	int xb,xe,y=0;
	for(it=pList.begin();it!=pList.end();++it){
		xe=it->x+m_a;
		xb=m_a-(it->x);
		y=m_b-(it->y);
		int y2=2*m_b-y;
		//确保不跃界
		if(xb<0)
			xb=0;
		if(xe>=w)
			xe=w-1;
		if(y<0) y=0;
		if(y2>=h) y2=h-1;

		BYTE* pLine1=img.GetBits(xb,y);
		BYTE* pLine2=img.GetBits(xb,y2);
		for(int i=xb;i<=xe;++i,pLine1+=4,pLine2+=4){
			RGBQUAD rgb;
			rgb.rgbBlue=0;
			rgb.rgbGreen=0;
			rgb.rgbRed=0;
			FCColor::CopyPixel(pLine1,&rgb,4);
			FCColor::CopyPixel(pLine2,&rgb,4);
		}		
	}
	//img.Save("D:\\PRETEST.bmp");
	if(flag){
		img.Rotate_Smooth(90);		
	}	
	img.Rotate_Smooth(m_angle);
	w=img.Width();
	h=img.Height();
	RECT rc;
	rc.left=w;
	rc.right=0;
	rc.bottom=0;
	rc.top=h;
	for(int y=0;y<h;++y){
		BYTE* pLine=img.GetBits(0,y);
		for(int x=0;x<w;++x,pLine+=4)
		{
			RGBQUAD rgb;
			FCColor::CopyPixel(&rgb,pLine,4);
			if(rgb.rgbBlue==0xFF&&rgb.rgbGreen==0xFF&&rgb.rgbRed==0xFF)
				continue;
			if(rc.left>x)
				rc.left=x;
			if(rc.right<x)
				rc.right=x;
			if(rc.bottom<y)
				rc.bottom=y;
			if(rc.top>y)
				rc.top=y;
		}
	}
	img.ConvertToGray();
	img.GetSubBlock(&(bw.m_mask),rc);
	w=bw.m_mask.Width();
	h=bw.m_mask.Height();
	int num=0;
	for(int y=0;y<h;++y){
		BYTE* pByte=bw.m_mask.GetBits(0,y);
		for(int x=0;x<w;++x,pByte+=1){
			if((*pByte)==0xFF){
				(*pByte)=0x00;
			}else{
				(*pByte)=0xFF;
				num++;
			}
		}
	}
	bw.m_maskNum=num;
	bw.m_mask.Save("D:\\test111.bmp");
	return num;
}