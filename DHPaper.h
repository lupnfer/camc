#pragma once
#include "objbase.h"
#include "objImage.h"
#include "DHStroke.h"
#include "ParentBrush.h"

#include "ParentPaper.h"
#include "DHBrush.h"
#include "GraphicsGems.h"

#define R1  0.36
// �߽ṹ��������
typedef struct Edge{
	int ymax;  // �ߵ����y����
	float x; // �뵱ǰɨ���ߵĽ���x����
	float dx; // ������ֱ��б�ʵĵ���
	struct Edge * pNext; // ָ����һ����
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
