#pragma once
#include "parentbrush.h"
#include <vector>
#include "DHStroke.h"
#include "stdDefine.h"

using namespace std;

class CDH2Brush :
	public CParentBrush
{

public:
	CDH2Brush(void){
		m_r = 0;
		m_angle = 90;
		m_angelofdirection = 60;
	}

	~CDH2Brush(void){
		if (!m_VecPoints.empty()) {
			m_VecPoints.clear();
		}
	}
	void InitSection(int r)
	{
		m_VecPoints.clear();
	}
public:
	void GenSection(int r,int angle_xy, int angle_z);

	void SetSlopeAngle(int angle_xy)
	{
		m_angle = angle_xy;
	}
	void SetDirectionAngle(int angle_z)
	{
		m_angelofdirection = angle_z;
	}


	int SlopeAngle(){
		return m_angle;
	}
	int DirectionAngel()
	{
		return m_angelofdirection;
	}
	vector<CPoint>& GetPointVector()
	{
		return m_VecPoints;
	}
private:
	void FillLine(int start_x,int start_y,int angle);

protected:
	vector<CPoint> m_VecPoints;
	int			m_angle;                //压力笔与水平面夹角决定长轴短轴的变化
	int       m_angelofdirection ;      //椭圆方向角
};
