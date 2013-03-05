#include "StdAfx.h"
#include ".\virtualpaper.h"
#include "ColorDisplay.h"
#include "FColor.h"
#include "ObjImage.h"
#include "ObjSelect.h"
#include "VBDefine.h"

//一笔画完以后，重新设置纸张中水分。

CVirtualPaper::CVirtualPaper(){
	//m_pCurdrawRegion=new FCObjSelect();
	//m_pCurdrawRegion=NULL;
	m_light=255;
	m_pImage=NULL;
	m_isdiffuse=true;
	m_absorbency=0.4;
}
CVirtualPaper::CVirtualPaper(const CVirtualPaper& vp)
{
	this->m_timeslice=vp.m_timeslice;
	this->m_light=vp.m_light;
	this->m_absorbency=vp.m_absorbency;
	this->m_isdiffuse=vp.m_isdiffuse;
	::CopyRect(&m_selRect,&(vp.m_selRect));
	m_ppStruct=vp.m_ppStruct;
	FCObjGraph::operator =(vp);
}
CVirtualPaper::~CVirtualPaper(){
	
}
void CVirtualPaper::ToBeDry()
{
	for(int y=0;y<m_ppStruct.rows();++y){
		PAPERLINE& paperRow=m_ppStruct.getRow(y);
		PAPERLINE::iterator it=paperRow.begin();
		for(;it!=paperRow.end();++it){
			(*it).m_waterQuan=0;
			//(*it).AddNewLayer(pig);//增加一个新的颜料层
		}
	}
}
void CVirtualPaper::Redo()
{
	
	RECT rcPaper,rcDest;
	GetRectInCanvas(&rcPaper);
	if(::IntersectRect(&rcDest,&rcPaper,&m_selRect)==false)
		return;
	for(int y=rcDest.top;y<rcDest.bottom;++y){
		//BYTE* ppline=m_pCurdrawRegion->GetBits(rcDest.left-rcRegion.left,y-rcRegion.top);
		PAPERLINE& paperRow=m_ppStruct.getRow(y-rcPaper.top);
		PAPERLINE::iterator it=paperRow.begin()+rcDest.left-rcPaper.left;
		for(int x=rcDest.left;x<rcDest.right;++it,++x){
			if(IsOnDraw(*it)){
				(*it).Redo();
			}
		}
	}
}
void CVirtualPaper::Undo()
{
	RECT rcPaper,rcDest;
	GetRectInCanvas(&rcPaper);
	if(::IntersectRect(&rcDest,&rcPaper,&m_selRect)==false)
		return;
	for(int y=rcDest.top;y<rcDest.bottom;++y){
		PAPERLINE& paperRow=m_ppStruct.getRow(y-rcPaper.top);
		PAPERLINE::iterator it=paperRow.begin()+rcDest.left-rcPaper.left;
		for(int x=rcDest.left;x<rcDest.right;++it,++x){
			if(IsOnDraw(*it)){
				(*it).Undo();
			}
		}
	}
}
void CVirtualPaper::BeginOneStrokeDraw(Pigment pig)
{
	RECT rcRst,rcPaper={0,0,m_ppStruct.columns(),m_ppStruct.rows()};
	if(::IntersectRect(&rcRst,&m_selRect,&rcPaper)==false)
		return;
	for(int y=rcRst.top;y<rcRst.bottom;++y){
		PAPERLINE& paperRow=m_ppStruct.getRow(y);
		PAPERLINE::iterator it=paperRow.begin()+rcRst.left;
		for(int x=rcRst.left;x<rcRst.right;++it,++x){
			it->m_waterQuan=0;
			it->m_isDiffused=false;
			(*it).AddNewLayer(pig);//增加一个新的颜料层
		}
	}
	//m_pCurdrawRegion->SetGraphObjPos(0,0);
	//m_pCurdrawRegion->Destroy();
}
void CVirtualPaper::EndOneStrokeDraw()
{
	if(m_isdiffuse)
		Diffusion();
	Draw();
}
//实现颜色的绘制：通过纸张上的颜料信息和当前的光照强度来绘制最终的显示颜色
void CVirtualPaper::DrawSection(POINT start,FCObjSelect& mask)
{
	if(m_pImage==NULL&&m_pImage->ColorBits()<24){
		return;
	}
	int w=mask.Width();
	int h=mask.Height();
	int step=m_pImage->ColorBits()/8;
	for(int y=0;y<h;++y){
		BYTE* ppline=mask.GetBits(0,y);
		PAPERLINE& paperLine=m_ppStruct.getRow(y+start.y);
		BYTE*  ppImgLine=m_pImage->GetBits(start.x,y+start.y);
		PAPERLINE::iterator it=paperLine.begin()+start.x;

		for(int x=0;x<w;++x,ppline+=1,++it,ppImgLine+=step){
			if((*ppline)==0xFF){
				BYTE r,g,b;
//				CColorDisplay::DrawColor((*it).m_cbList,r,g,b,m_light);
				RGBQUAD rgb;
				rgb.rgbRed=r;
				rgb.rgbGreen=g;
				rgb.rgbBlue=b;
				FCColor::CopyPixel(ppImgLine,&rgb,3);
			}
		}
	}
}
void CVirtualPaper::Diffusion()
{
	if(m_ppStruct.rows()<=1)
		return;
	RECT rcRegion,rcPaper,rcDest;
	GetRectInCanvas(&rcPaper);
	if(::IntersectRect(&rcDest,&rcPaper,&m_selRect)==false)
		return;
	//记录初始时满足扩散条件的Papel,放到链表中m_ppList中，采用先进先出的原则，每次扩散仅限于8邻域。
//	PAPERLINE ppList;
	//BYTE* pTmpL,pTmpR,pTmpT,pTmpB,pTmpLT,pTmpRT,pTmpLB,pTmpRB;
	int num=0;
	int time=10*sqrt(m_absorbency);
	while(num<time){
		for(int y=rcDest.top;y<rcDest.bottom;++y){
			//BYTE* ppline=m_pCurdrawRegion->GetBits(rcDest.left-rcRegion.left,y-rcRegion.top);
			PAPERLINE& paperRow=m_ppStruct.getRow(y);
			PAPERLINE::iterator it=paperRow.begin()+rcDest.left;
			for(int x=rcDest.left;x<rcDest.right;++x,++it){
				if(IsOnDraw(*it)){
					//if(IsOnDiffusionList(*it))
					//	ppList.push_front((*it));
					//it->m_isDiffused=true;
					DiffusionOfOnePapel(*it);
				}
			}
		}
		num++;
	}
/*	Papel pl;//标志，用来表示进行了一轮扩散
	pl.m_waterQuan=0;
	ppList.push_front(pl);
	int num=0;
	while(ppList.empty()==false&&num<MAX_DIFFUSION_TIME){
		Papel& plTmp=ppList.back();
		ppList.pop_back();
		if(plTmp.m_waterQuan==0){
			//ExpandFrame(1,1,1,1);
			ppList.push_front(pl);
			num++;
			continue;
		}
		DiffusionOfOnePapel(plTmp,ppList);
	}*/
}
void CVirtualPaper::DiffusionOfOnePapel(Papel& pl)
{
	if(IsPapelDiffusion(pl)==FALSE)
		return;
	/**********************************
	//八邻域的编号为：
	0  1  2
	7     3
	6  5  4
	***********************************/
	PAPERLINE::iterator it=m_ppStruct.GetPapel(pl.m_pos.x,pl.m_pos.y);
	PAPERLINE::iterator neighbor[8];
	Get8Neighbors(pl,neighbor);
	DiffusionFunc(it,neighbor);
}
void CVirtualPaper::Get8Neighbors(Papel& pl,PAPERLINE::iterator neighbor[])
{
	PAPERLINE::iterator end=m_ppStruct.GetPapel(-1,-1);
	POINT pos=pl.m_pos;
	PAPERLINE::iterator it=m_ppStruct.GetPapel(pos.x,pos.y);
	RECT rc;
	GetRectInCanvas(&rc);
	neighbor[1]=m_ppStruct.GetPapel(pos.x,pos.y-1);
	neighbor[5]=m_ppStruct.GetPapel(pos.x,pos.y+1);
	if(pos.x<=rc.left||pos.y<=rc.top)
		neighbor[0]=end;
	else neighbor[0]=neighbor[1]-1;
	if(pos.x>=(rc.right-1)||pos.y<=rc.top)
		neighbor[2]=end;
	else neighbor[2]=neighbor[1]+1;
	if(pos.x>=(rc.right-1))
		neighbor[3]=end;
	else {
		neighbor[3]=it+1;
	}
	if(pos.x>=(rc.right-1)||pos.y>=(rc.bottom-1))
		neighbor[4]=end;
	else neighbor[4]=neighbor[5]+1;
	if(pos.x<=rc.left||pos.y>=(rc.bottom-1))
		neighbor[6]=end;
	else neighbor[6]=neighbor[5]-1;
	if(pos.x<=rc.left)
		neighbor[7]=end;
	else neighbor[7]=it-1;
}
void CVirtualPaper::DiffusionFunc(PAPERLINE::iterator center,PAPERLINE::iterator neighbor[])
{
	double Absum=0,Gsum=0,Rsum=0,Ab[8],G[8],R[8];
	PAPERLINE::iterator end=m_ppStruct.GetPapel(-1,-1);
	double Nsum=0.0;
	for(int i=0;i<8;++i){
		if(neighbor[i]==end)
		{
			Ab[i]=0;
			G[i]=0;
			continue;
		}
		Ab[i]=(neighbor[i])->m_absorb;
		Absum+=Ab[i];
		G[i]=UnitFuc((*center).m_waterQuan,(*neighbor[i]).m_waterQuan);
		Gsum+=G[i];
		Nsum+=double((*center).m_npiple[i]);
	}
	for(int i=0;i<8;++i){
		if(neighbor[i]==end)
		{
			R[i]=0;
			continue;
		}
		Ab[i]=Ab[i]/Absum;
		G[i]=G[i]/Gsum;
		double ni=double((*center).m_npiple[i])/Nsum;
		R[i]=ni*Ab[i]*G[i];
		Rsum+=R[i];
	}
	if(Gsum==0||Rsum==0)
		return;
	double wfSum=0,pfSum=0,waterFlow,pigFlow,ratio;
	//计算当前Papel墨水向八邻域扩散墨水总量的比例
	double K,abc=center->m_absorb;
	K=(1-1/exp(Rsum))*(2*abc-abc*abc)/2+0.3;	
	PIGMENTLIST& pgList=center->m_cbList;
	Pigment& pig=pgList.back();
	wfSum=(center->m_waterQuan)*K;
	pfSum=(pig.m_quan)*K;
	(center->m_waterQuan)-=wfSum;
	(pig.m_quan)=pig.m_quan-pfSum;
	//计算每个八邻域因扩散而分别获得的墨水量
	for(int i=0;i<8;++i){
		if(neighbor[i]==end)
		{
			continue;
		}
		R[i]=R[i]/Rsum;
		waterFlow=wfSum*R[i];
		pigFlow=pfSum*R[i];
		(neighbor[i])->AddPigmentOnSameLayer(pigFlow);
		(neighbor[i])->m_waterQuan+=waterFlow;
		//if(((neighbor[i])->m_waterQuan)>(center->m_waterQuan)){
		//	double tmp=3*(((neighbor[i])->m_waterQuan)-(center->m_waterQuan))/5;
		//	(neighbor[i])->m_waterQuan-=tmp;
		//	(center->m_waterQuan)+=tmp;
		//}		
	/*	if(IsOnDiffusionList(*(neighbor[i]))){
			(neighbor[i])->m_isDiffused=true;
			ppList.push_front(*(neighbor[i]));
		}*/
	}
}
BOOL CVirtualPaper::IsPapelDiffusion(Papel& pl)
{
	double diffuse=pl.m_waterQuan;
	if(diffuse>0.1)
		return true;
    return false;
}
void CVirtualPaper::Init(double absorbency,int ppType)
{
	int row=m_ppStruct.rows();
	int col=m_ppStruct.columns();
	if(row<=1||col<=1)
		return;
	m_absorbency=absorbency;
	srand( (unsigned)(absorbency*100000) );
	double RANGE_MAX=0.3;
	double RANGE_MIN=-0.15;
	for(int y=0;y<m_ppStruct.rows();++y){
		PAPERLINE& paperRow=m_ppStruct.getRow(y);
		PAPERLINE::iterator it=paperRow.begin();
		for(int x=0;it!=paperRow.end();++it,++x){
			double range=(((double) rand() /(double) RAND_MAX) * RANGE_MAX + RANGE_MIN);
			(*it).m_absorb=absorbency+range;
			(*it).m_pos.x=x;
			(*it).m_pos.y=y;
		}
	}
	/***********************************
	0 1  2
	7    3
	6 5  4
	***********************************/
	PAPERLINE& paperRow1=m_ppStruct.getRow(0);
	//PAPERLINE& paperRow2=paperRow1;
	PAPERLINE::iterator it=paperRow1.begin();
	PAPERLINE::iterator it3,it4,it5;
	int y=0;
	for(y=1;y<row;++y){		
		PAPERLINE& paperRow2=m_ppStruct.getRow(y);
		int xx=it->m_pos.x;
		int yy=it->m_pos.y;
		it3=it+1;
		xx=it3->m_pos.x;
		it5=paperRow2.begin();
		it4=it5+1;
		for(;it4!=paperRow2.end();++it,++it4,++it3,++it5){
			if(it3==paperRow1.end()){
				(*it).m_npiple[5]=GenNumOfPiple(ppType,5);
				(*it5).m_npiple[1]=(*it).m_npiple[5];
				break;
			}
			(*it).m_npiple[3]=GenNumOfPiple(ppType,3);
			(*it).m_npiple[4]=GenNumOfPiple(ppType,4);
			(*it).m_npiple[5]=GenNumOfPiple(ppType,5);
			(*it3).m_npiple[6]=GenNumOfPiple(ppType,6);

			(*it3).m_npiple[7]=(*it).m_npiple[3];
			(*it5).m_npiple[1]=(*it).m_npiple[5];
			(*it4).m_npiple[0]=(*it).m_npiple[4];
			(*it5).m_npiple[2]=(*it3).m_npiple[6];
		}
		it=paperRow2.begin();

	}
	if(y==row){
		PAPERLINE& paperRow2=m_ppStruct.getRow(y-1);
		it=paperRow2.begin();
		it3=it+1;
		for(;it3!=paperRow2.end();++it3,++it){
			(*it).m_npiple[3]=GenNumOfPiple(ppType,3);
			(*it3).m_npiple[7]=(*it).m_npiple[3];
		}
	}
}

void CVirtualPaper::SetImage(FCObjImage* img,RECT& rcInPaper){
	m_pImage=img;
	RECT rcSel;
	GetRectInCanvas(&rcSel);
	::IntersectRect(&m_selRect,&rcSel,&rcInPaper);
}
void CVirtualPaper::GetRectInCanvas(RECT* rc)
{
	::SetRect(rc,GetGraphObjPos().x,GetGraphObjPos().y,GetGraphObjPos().x+m_ppStruct.columns(),GetGraphObjPos().y+m_ppStruct.rows());
}

void CVirtualPaper::Draw()
{
	if(m_pImage==NULL&&m_pImage->ColorBits()<24){
		return;
	}
	if(::IsRectEmpty(&m_selRect)){
		return;
	}
	int step=m_pImage->ColorBits()/8;
	//计算m_pCurdrawRegion与
	RECT rcSel,rcRst;	
	for(int y=m_selRect.top;y<m_selRect.bottom;++y){
		PAPERLINE& paperLine=m_ppStruct.getRow(y);
		BYTE*  ppImgLine=m_pImage->GetBits(0,y-m_selRect.top);
		PAPERLINE::iterator it=paperLine.begin()+m_selRect.left;
		for(int x=m_selRect.left;x<m_selRect.right;++x,++it,ppImgLine+=step){
			if(IsOnDraw(*it)){
				BYTE r,g,b;
				CColorDisplay::DrawColor((*it).m_cbList,r,g,b,m_light);
				PIGMENTLIST& pgList=(*it).m_cbList;
				Pigment& p=pgList[0];
				double a=p.m_quan;
				if(a!=0){
					a=6;
				}
				//CColorDisplay::DrawColor()
				RGBQUAD rgb;
				//r=255;
				//b=0;
				//g=0;
				rgb.rgbRed=r;
				rgb.rgbGreen=g;
				rgb.rgbBlue=b;
				rgb.rgbReserved=255;

				FCColor::CopyPixel(ppImgLine,&rgb,step);
			}
		}
	}
}