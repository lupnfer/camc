#pragma once
#include "objbase.h"
#include "objImage.h"
#include "DHStroke.h"
#include "ParentBrush.h"

#include "ParentPaper.h"
#include "DHBrush.h"
#include "GraphicsGems.h"

#define R1  0.36
// 边结构数据类型
typedef struct Edge{
	int ymax;  // 边的最大y坐标
	float x; // 与当前扫描线的交点x坐标
	float dx; // 边所在直线斜率的倒数
	struct Edge * pNext; // 指向下一条边
}Edge, * LPEdge;

class  CParentBrush;
struct BezierPressCurve;

class CDHPaper :
	public CParentPaper
{
public:
	void DrawSection(CDC *pDC,TPosition& p,COLORREF c);
	void DrawSection(FCObjImage& img,TPosition& p,BYTE alpha);
	static  CDHPaper* Instance();
	static void FillPolygon(LPPOINT lpPoints,int nCount,FCObjImage& img, COLORREF nColor);
protected:
	static    CDHPaper*  _instance;
};
