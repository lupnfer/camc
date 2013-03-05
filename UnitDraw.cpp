#include "StdAfx.h"
#include ".\unitdraw.h"
#include<math.h>
void CUnitDraw::DrawPixel(FCObjImage* _pImg,int x,int y,COLORREF c)
{
	if(_pImg==NULL)
		return;
	if(_pImg->ColorBits() == 32){
		_pImg->SetPixel(x,y,GetRValue(c),GetGValue(c),GetBValue(c));
	}else{
		BYTE gray=255;
		gray=(BYTE)(GetRValue(c)*0.299+GetGValue(c)*0.587+GetBValue(c)*0.114);
		//RGB_to_GRAY(GetRValue(c),GetGValue(c),GetBValue(c),gray);
		_pImg->SetGrayPixel(x,y,gray);
	}
}
//实现DDA 画线算法
void CUnitDraw::DrawLine(FCObjImage* _pImg,CPoint& p1,CPoint& p2,int w,COLORREF c)
{

	int dx=p2.x-p1.x;
	int dy=p2.y-p1.y;
	int steps,k;
	float xIncrement,yIncrement,x=p1.x,y=p1.y;
	if(abs(dx)>abs(dy)){ 
		steps=abs(dx);
	}
	else{
		steps=abs(dy);
	}
	xIncrement=dx/(float)steps;
	yIncrement=dy/(float)steps;
	DrawWidth(_pImg,x,y,w,c);
	for(k=0;k<steps;k++){
		x+=xIncrement;
		y+=yIncrement;
		DrawWidth(_pImg,x,y,w,c);
	}
}
void CUnitDraw::FillCircle(FCObjImage* _pImg,CPoint& centre,int r,COLORREF c)
{
	int x,y;
	float d;
	d=1.25-r;
	x=0,y=r;
	if(r==0){
		DrawPixel(_pImg,centre.x,centre.y,c);
		return;
	}
	FillLine(_pImg,centre,x,y,c);
	while(x<=y)
	{ 
		if(d<0)   
			d+=x*2+3;
		else{ 
			d+=(x-y)*2+5; 
			y--;
		}
		x++;
		FillLine(_pImg,centre,x,y,c);
		FillLine(_pImg,centre,(-1)*x,y,c);
		FillLine(_pImg,centre,y,x,c);
		FillLine(_pImg,centre,(-1)*y,x,c);		
	}
}
void CUnitDraw::FillLine(FCObjImage*pImg,CPoint& centre,int start_x,int start_y,COLORREF c)
{
	int end_y=(-1)*start_y;
	int tmp_x=centre.x+start_x;
	for(int i=start_y;i>=end_y;i--){
		DrawPixel(pImg,tmp_x,i+centre.y,c);
	}
}

void CUnitDraw::DrawCircle(FCObjImage* _pImg,CPoint& centre,int r,int w,COLORREF c)
{
	int x,y,x1,y1,x2,y2;
	float d;
	d=1.25-r;
	x=0,y=r;
	DrawWidth(_pImg,x+centre.x,y+centre.y,w,c);
	while(x<=y)
	{ 
		if(d<0)   
			d+=x*2+3;
		else{ 
			d+=(x-y)*2+5; 
			y--;
		}
		x++;
		x1=centre.x+x; y1=centre.y+y; 
		DrawWidth(_pImg,x1,y1,w,c);
		DrawWidth(_pImg,x1,(centre.y<<1)-y1,w,c);
		DrawWidth(_pImg,(centre.x<<1)-x1,y1,w,c);
		DrawWidth(_pImg,(centre.x<<1)-x1,(centre.y<<1)-y1,w,c);
		x2=y1+centre.x-centre.y;
		y2=x1+centre.y-centre.x;
		DrawWidth(_pImg,x2,y2,w,c);
		DrawWidth(_pImg,x2,(centre.y<<1)-y2,w,c);
		DrawWidth(_pImg,(centre.x<<1)-x2,y2,w,c);
		DrawWidth(_pImg,(centre.x<<1)-x2,(centre.y<<1)-y2,w,c);
	}
}
