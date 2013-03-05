#include "StdAfx.h"
#include ".\dh2brush.h"
#include "math.h"


void CDH2Brush::GenSection(int r,int angle_xy, int angle_z)
{
	InitSection(r);
	//由r计算得到椭圆长轴a,和短轴b
	//(1)  根据角度关系即m_angle可以得出a与b的另外一个关系

	int 	Ry = r * sin((angle_z * LIB_PI)/180);
	int 	Rx = 2 * r - Ry;
	int Rx2 = Rx * Rx;
	int Ry2 = Ry * Ry;
	int TwoRx2 = 2 * Rx2;
	int TwoRy2 = 2 * Ry2;
	int p;
	int x = 0;
	int y = Ry;
	int px = 0;
	int py = TwoRx2 * y;
	FillLine(x,y,angle_xy);

	p = (int)(Ry2 -(Rx2 * Ry) + ( 0.25 * Rx2 ) + 0.5);
	while (px<py) {
		x ++;
		px += TwoRy2;
		if (p<0) {
			p += Ry2 + px;
		}
		else
		{
			y --;
			py -= TwoRx2;
			p+= Ry2 + px -py;
		}
		FillLine(x,y,angle_xy);
		FillLine(-x,y,angle_xy);
	}

	p = (int)(Ry2 * (x + 0.5)*(x + 0.5) +Rx2 * (y -1)*(y - 1) - Rx2*Ry2 + 0.5);
	while (y>0) {
		y --;
		py -= TwoRx2;
		if (p>0) {
			p += Rx2 - py;
		}
		else
		{
			x ++;
			px += TwoRy2;
			p += Rx2 - py + px;
		}
		FillLine(x,y,angle_xy);
		FillLine(-x,y,angle_xy);
	}

}
void CDH2Brush::FillLine(int start_x,int start_y,int angle)
{
	int xnew = 0;
	int ynew = 0;
	for(int y=start_y;y >= -start_y; y--){

		xnew = start_x * cos((angle * LIB_PI)/180.0) - y * sin((angle * LIB_PI)/180.0);
		ynew = start_x * sin((angle * LIB_PI)/180.0) + y * cos((angle * LIB_PI)/180.0);
		m_VecPoints.push_back(CPoint(xnew,ynew));
	}

}