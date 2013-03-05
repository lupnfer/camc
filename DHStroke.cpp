#include "StdAfx.h"
#include ".\dhstroke.h"
#include "DHPaper.h"


CDHStroke::CDHStroke(void)
{
	m_lpPaper=NULL;
	m_c=RGB(0,0,0);
	m_isErase=FALSE;

	//CShape shape;
	//shape.Init(40,40,0);
	//Ink ik;
	//ik.m_percentage=0.4;
	//ik.m_pigAttr.B_reflectance=0.1;
	//ik.m_pigAttr.R_reflectance=0.1;
	//ik.m_pigAttr.G_reflectance=0.1;
	//m_vb.Init(shape,ik,10);
}

CDHStroke::~CDHStroke(void)
{
}
void CDHStroke::InsertStrokePiece(CDC* pDC,TPosition& position)
{
	int index=m_disCurve.size()-1;
	if(index>=0){
		TPosition p;
		p.m_x=m_disCurve[index].x;
		p.m_y=m_disCurve[index].y;
		p.press=m_pressList[index].press;
		p.m_angle_xy=m_pressList[index].m_angle_xy;
		p.m_angle_z=m_pressList[index].m_angle_z;
		Integer_DDA_Line(pDC,p,position);
	}
	AddPoint(position);
}
//-----------------------------------------------------------
//实现Bresenham画线算法
void CDHStroke::Integer_Bresenham_Line(CDC* pDC,TPosition& p1,TPosition& p2)
{
	int x,y,dx,dy,e;
	dx=abs(p2.m_x-p1.m_x);
	dy=abs(p2.m_y-p1.m_y);
	
	x=p1.m_x; y=p1.m_y;
	if(dx>=dy)
	{
		e=-dx; 
		for(int i=0; i<=dx; i++)
		{
			//drawpixel(x,y)
			TPosition p;
			p.m_x=x;
			p.m_y=y;
			p.press=4;
			m_lpPaper->DrawSection(pDC,p,m_c);
			if(p2.m_x>p1.m_x)
				x=x+1;
			else x=x-1;
			e=e+2*dy;
			if(e>=0)
			{	
				if(p2.m_y>p1.m_y)
					y=y+1;
				else y=y-1;
				e=e-2*dx;
			}
		}
	}else{
		e=-dy;
		for(int i=0; i<=dx; i++)
		{
			//drawpixel(x,y)
			TPosition p;
			p.m_x=x;
			p.m_y=y;
			p.press=4;
			m_lpPaper->DrawSection(pDC,p,m_c);
			if(p2.m_y>p1.m_y)
				y=y+1;
			else y=y-1;
			e=e+2*dx;
			if(e>=0)
			{	
				if(p2.m_x>p1.m_x)
					x=x+1;
				else 
					x=x-1;
				e=e-2*dy;
			}
		}		
	}
}

void CDHStroke::Integer_DDA_Line(CDC* pDC,TPosition& p1,TPosition& p2)
{
	int dx=p2.m_x-p1.m_x;
	int dy=p2.m_y-p1.m_y;
	int steps,k;
	float xIncrement,yIncrement,x=p1.m_x,y=p1.m_y;
	if(abs(dx)>abs(dy)){ 
		steps=abs(dx);
	}
	else{
		steps=abs(dy);
	}
	xIncrement=dx/(float)steps;
	yIncrement=dy/(float)steps;
	TPosition p;
	p.m_x=x;
	p.m_y=y;
	p.press=4;
	m_lpPaper->DrawSection(pDC,p,m_c);
	for(k=0;k<steps;k++){
		x+=xIncrement;
		y+=yIncrement;
		TPosition p;
		p.m_x=x;
		p.m_y=y;
		p.press=4;
		m_lpPaper->DrawSection(pDC,p,m_c);
	}
}

