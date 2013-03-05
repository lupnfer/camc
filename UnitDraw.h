#pragma once
#include "ObjImage.h"
class CUnitDraw
{
public:
	static void DrawWidth(FCObjImage* _pImg,int x,int y,int w,COLORREF c)
	{
		int r=w/2;
		FillCircle(_pImg,CPoint(x,y),r,c);
	}
public:
	static void DrawPixel(FCObjImage* _pImg,int x,int y,COLORREF c);
	static void DrawLine(FCObjImage* _pImg,CPoint& p1,CPoint& p2,int w,COLORREF c);
	static void DrawCircle(FCObjImage* _pImg,CPoint& centre,int r,int w,COLORREF c);
	static void FillCircle(FCObjImage* _pImg,CPoint& centre,int r,COLORREF c);
	static void FillLine(FCObjImage*pImg,CPoint& centre,int start_x,int start_y,COLORREF c);

};
