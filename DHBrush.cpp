#include "StdAfx.h"
#include ".\dhbrush.h"

void CDHBrush::GenSection(int r)
{
	InitSection(r);
	int x,y;
	int deltax=3; 
	int deltay=2-r-r; 
	int d=1-r;
	x=0,y=r;
	if(r==0){
		FillLine(x,y);
		return;
	}
	FillLine(x,y);
	while(x<=y)
	{ 
		if(d<0) {
			d+=deltax;
			deltax+=2;
			x++;
		}
		else{ 
			d+=(deltax+deltay);
			deltax+=2; deltay+=2;
			x++;
			y--;
		}
		FillLine(x,y);
		FillLine(y,x);	
	}	
}
void CDHBrush::FillLine(int start_x,int start_y)
{
	for(int i=start_y;i>=0;i--){
		m_lpSection[i*m_r+i+start_x].m_bUse=true;
	}
}
void CDHBrush::GenCircleSection(int r){
	InitSection(r);
	int x,y;
	int deltax=3; 
	int deltay=2-r-r; 
	int d=1-r;
	x=0,y=r;
	if(r==0){
		FillLine(x,y);
		return;
	}
	FillLine(x,y);
	while(x<=y)
	{ 
		if(d<0) {
			d+=deltax;
			deltax+=2;
			x++;
		}
		else{ 
			d+=(deltax+deltay);
			deltax+=2; deltay+=2;
			x++;
			y--;
		}
		m_lpSection[y*r+y+x].m_bUse=true;
		m_lpSection[x*r+x+y].m_bUse=true;
	}		
}
