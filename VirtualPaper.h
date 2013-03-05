#pragma once

#include "objbase.h"
#include "VBDefine.h"

/***************************
*作者：刘建明
*原始完成日期：2006.9.26
*用途：实现虚拟的纸张功能，一个Cavas对应一个paper. 
*作者本人修订日期：
*其他人修订（包括修改者，修改日期，修改地方，修改前的内容，修改后内容，作用等）      
***************************/
class FCObjSelect;
class FCObjImage;

const int MAX_DIFFUSION_TIME=10;
const int DIFFUSE_THD=0.2;
class CVirtualPaper :
	public FCObjGraph
{
public:
	CVirtualPaper();
public:
	CVirtualPaper(const CVirtualPaper& vp);
	void SetImage(FCObjImage* img,RECT& rcInPaper);
	virtual ~CVirtualPaper(void);
	void Create(int nWidth,int nHight){
		m_ppStruct.resize(nHight,nWidth);
	}
	//static CVirtualPaper* Instance(FCObjImage* img,double absorbency=0.0,int ppType=-1);
	void ToPaperRect(FCObjGraph& graph,RECT& rcDst){
		RECT rcTmp;
		int x_offset=graph.GetGraphObjPos().x-GetGraphObjPos().x;
		int y_offset=graph.GetGraphObjPos().y-GetGraphObjPos().y;
		rcDst.left+=x_offset;
		rcDst.right+=x_offset;
		rcDst.bottom+=y_offset;
		rcDst.top+=y_offset;
	}
private:
	int m_timeslice;
	PAPERSTRUCT m_ppStruct;//纸张的数据结构
	double      m_absorbency;//纸张的吸收能力（0，1）
//	RECT        m_dimension;//纸张大小
	BYTE        m_light;
	BOOL        m_isdiffuse;
	RECT        m_selRect;//选中的绘制区域
	FCObjImage* m_pImage;
	//static CVirtualPaper* _instance;
protected:
	//FCObjSelect* m_pCurdrawRegion;//当前的绘制区域
private:
	int GenNumOfPiple(int ppType,int index){
		int numOfPiple=0;
		srand( (unsigned)time( NULL ) );
		int RANGE_MIN=0;
		int RANGE=2;
		if(ppType==RICEPAPER){
			switch(index){
				case 0:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+3); break;
				case 1:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+4); break;
				case 2:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+3); break;
				case 3:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+4); break;
				case 4:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+3); break;
				case 5:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+4); break;
				case 6:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+3); break;
				case 7:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+4); break;
			}
		}else if(ppType==SILKPAPER){
			switch(index){
				case 0:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE)+2; break;
				case 1:numOfPiple=0; break;
				case 2:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+2); break;
				case 3:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+4); break;
				case 4:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+2); break;
				case 5:numOfPiple=0; break;
				case 6:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+2); break;
				case 7:numOfPiple=(((double) rand() /(double) RAND_MAX) *RANGE+4); break;
			}
		}
		return numOfPiple;
	}
public:
	void Init(double absorbency,int ppType);
	void ToBeDry();
	PAPERSTRUCT& GetPPStruct(){return m_ppStruct;}
	void Diffusion();
	void DrawSection(POINT start,FCObjSelect& mask);
	//void AddSection(RECT& rc);
	void Draw();
	void Redo();
	void Undo();
	void BeginOneStrokeDraw(Pigment pig);
	void EndOneStrokeDraw();
	void GetRectInCanvas(RECT* rc);
	RECT& GetRectInPaper(){return m_selRect;}
	RECT  GetDimension(){RECT rc;rc.left=0;rc.top=0;rc.right=::RECTWIDTH(m_selRect);rc.bottom=::RECTHEIGHT(m_selRect);return rc;}
	
private:
	void   DiffusionOfOnePapel(Papel& pl);
	//double DiffuseFunc(double ))
	BOOL   IsPapelDiffusion(Papel& pl);
	double UnitFuc(double w,double wk){
		double r=w-wk;
		if(r<=0)
			return 0;
		return r;
	}
	void DiffusionFunc(PAPERLINE::iterator center,PAPERLINE::iterator neighbor[]);
	BOOL IsOnDraw(Papel& pl){
		if(pl.m_waterQuan==0)
			return false;
		return true;
	}
	BOOL IsOnDiffusionList(Papel& pl){
		if(pl.m_isDiffused)
			return false;
		PAPERLINE::iterator neighbor[8];
		Get8Neighbors(pl,neighbor);
		PAPERLINE::iterator end=m_ppStruct.GetPapel(-1,-1);
		for(int i=0;i<8;++i){
			if(neighbor[i]!=end){
				if((neighbor[i])->m_waterQuan==0.0)
					return true;
			}
		}
		return false;
	}
	void Get8Neighbors(Papel& pl,PAPERLINE::iterator neighbor[]);
	void ExpandFrame(int xl,int xr,int yt,int yb){
		m_selRect.left-=xl;
		m_selRect.right+=xr;
		m_selRect.top-=yt;
		m_selRect.bottom+=yb;
		RECT rc,rcDst;
		GetRectInCanvas(&rc);
		::IntersectRect(&rcDst,&m_selRect,&rc);
		::CopyRect(&m_selRect,&rcDst);
	}
};
