#include "StdAfx.h"
#include ".\virtualbrush.h"
#include "ltiVector.h"

CVirtualBrush* CVirtualBrush::_instance=NULL;
CVirtualBrush* CVirtualBrush::Instance()
{
	if(_instance==NULL){
		_instance=new CVirtualBrush();
	}
    return _instance;
}
void CVirtualBrush::DrawSection(CVirtualPaper& paper,Point2 p,double press)
{

	int index=GetBrushWork(press);
	if(index==-1)
		return;
	BrushWork& bw=m_bwList[index];
	FCObjSelect& mask=bw.m_mask;
	///***************************************
	//计算当前笔触的外接矩形相对于当前绘制区域（）的位置
	int w=mask.Width();
	int h=mask.Height();
	Point2 pos;
	pos.x=p.x-w/2.0;
	pos.y=p.y-h/2.0;	
	RECT rc,rcRst;
	::SetRect(&rc,pos.x,pos.y,pos.x+w,pos.y+h);
	//计算当前绘制区域的位置（左上点为（0，0）），用来计算笔触的外接矩形与绘制区域的相交矩形,如果没有相交，就返回
	//相交矩形是相对于当前绘制区域的
	RECT ppRc=paper.GetDimension();
	if(::IntersectRect(&rcRst,&rc,&ppRc)==false)
		return;
	//计算相交区域矩形相对于笔触mask的位置rcBlock;
	RECT rcBlock;
	rcBlock.left=rcRst.left-rc.left;
	rcBlock.right=rcRst.right-rc.left;
	rcBlock.bottom=rcRst.bottom-rc.top;
	rcBlock.top=rcRst.top-rc.top;
	//计算相交区域矩形相对于当前绘制区域所在的纸张的位置（就是Canvas上的位置）rcRst.
	RECT& rcInPaper=paper.GetRectInPaper();
	rcRst.left+=rcInPaper.left;
	rcRst.right+=rcInPaper.left;
	rcRst.top+=rcInPaper.top;
	rcRst.bottom+=rcInPaper.top;
	//paper.AddSection(rcRst);
	//***********************************************************************
	//初始化
	PAPERSTRUCT& ppStruct=paper.GetPPStruct();
	double waterSum=0,pigSum=0;
	double waterEach=m_water/m_bristleNum;
	double pigEach=m_pigment.m_quan/m_bristleNum;

//下面代码计算墨水从毛笔流向纸张的量，同时得到毛笔上剩余墨水量（某一个时刻，毛笔上墨水按平均分配到每个刚毛上).
	for(int y=rcRst.top;y<rcRst.bottom;++y){
		PAPERLINE& papelRow=ppStruct.getRow(y);
		PAPERLINE::iterator it=papelRow.begin()+rcRst.left;
		BYTE* ptr=mask.GetBits(rcBlock.left,rcBlock.top+y-rcRst.top);
		for(int x=rcRst.left;x<rcRst.right;++it,++x,ptr=ptr+1){
			if((*ptr)==0xFF){
				double wq=(*it).m_waterQuan;
				if(wq<1){
					//墨水从毛笔流向纸张不但根纸张当前papel的吸收率m)absorb有关，而且根papel中现有水分子大小有关wq,APHLA为比例参数。
					double ratio=((*it).m_absorb)*(1-wq)*(1-wq)*APHLA;
					double flowWater=ratio*waterEach;
					double flowPig=ratio*pigEach;

					//控制是否需要笔刷上墨水量减少效果
					if(IsInkReduce()){
						waterSum+=flowWater;
						pigSum+=flowPig;
					}
					//往papel内流水分子
					(*it).m_waterQuan+=flowWater;
					//往papel内流墨分子
					(*it).AddPigmentOnSameLayer(flowPig);
				}
			}
		}
	}
	//笔刷上剩余的墨水含量
	m_water=m_water-waterSum;
	m_pigment.m_quan=m_pigment.m_quan-pigSum;
}
//定制笔触，并且产生此种笔触的从大到小的一系列笔触mask，用以加快绘制速度。
void CVirtualBrush::Customizing(CShape& shape)
{	
	if(shape.IsValid()==false)
		return;
	m_shape=shape;
	float a_axes=shape.GetA();
	float b_axes=shape.GetB();
	float min=a_axes>b_axes?b_axes:a_axes;
	shape.Init(a_axes,b_axes,shape.GetAngle());
	m_bristleNum=shape.GenShape(m_brushWork);
	m_bwList.push_back(m_brushWork);
	for(int i=1;i<min;++i){
		float a=a_axes*(1-i/min);
		float b=b_axes*(1-i/min);
		shape.Init(a,b,shape.GetAngle());
		BrushWork bw;
		if(shape.GenShape(bw)!=0)
			m_bwList.push_back(bw);
	}	
}
int CVirtualBrush::GetBrushWork(double press)
{
	if(press==0)
		return -1;
	int index=press/2;
	if(press<=3){
		index=1;
	}
	int max_len=m_bwList.size();
	index=max_len-index;
	if(index<0)
		return -1;
	return index;
}
void CVirtualBrush::BeginOneStrokeDraw(FCObjImage* img,CVirtualPaper& paper,RECT& rcInPaper)
{
	paper.SetImage(img,rcInPaper);
	paper.BeginOneStrokeDraw(m_pigment);
}
void CVirtualBrush::EndOneStrokeDraw(CVirtualPaper& paper)
{
	paper.EndOneStrokeDraw();
}