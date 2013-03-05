#pragma once
#include "objbase.h"
#include "ltiMatrix.h"
#include "ObjImage.h"
#define THRESHOLD  0.1
typedef std::vector<COLORREF> COLORLIST;
struct tagMeanColor{
	LONGLONG m_rValue;
	LONGLONG m_gValue;
	LONGLONG m_bValue;
	long     m_num;
public:
	tagMeanColor(){
		m_rValue=0;
		m_gValue=0;
		m_bValue=0;
		m_num=0;
	}
};
typedef std::vector<tagMeanColor*> MEANCOLORLIST;
class CColorBaseSD :
	public FCObject
{
public:
	CColorBaseSD(FCObjImage* pImg);
	~CColorBaseSD(void);
public:
	tagMeanColor*    GetMeanColorList(int& length);
	FCObjImage* DoWatershedSketchDistill();
	FCObjImage* DoSketchDistill();
	FCObjImage* GetImage(){return m_pImg;}
	void    AddPalette(COLORREF c){ m_palette.push_back(c);}
	void    Release(){ m_palette.clear();}	
protected:
	double  ComputeMinDiffToPalette(BYTE r,BYTE g,BYTE b);
private:
	COLORLIST		m_palette;//保存要提取出的颜色的种类的调色板
	FCObjImage*         m_pImg;
	lti::matrix<int> m_result;};
