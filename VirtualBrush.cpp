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
	//���㵱ǰ�ʴ�����Ӿ�������ڵ�ǰ�������򣨣���λ��
	int w=mask.Width();
	int h=mask.Height();
	Point2 pos;
	pos.x=p.x-w/2.0;
	pos.y=p.y-h/2.0;	
	RECT rc,rcRst;
	::SetRect(&rc,pos.x,pos.y,pos.x+w,pos.y+h);
	//���㵱ǰ���������λ�ã����ϵ�Ϊ��0��0��������������ʴ�����Ӿ��������������ཻ����,���û���ཻ���ͷ���
	//�ཻ����������ڵ�ǰ���������
	RECT ppRc=paper.GetDimension();
	if(::IntersectRect(&rcRst,&rc,&ppRc)==false)
		return;
	//�����ཻ�����������ڱʴ�mask��λ��rcBlock;
	RECT rcBlock;
	rcBlock.left=rcRst.left-rc.left;
	rcBlock.right=rcRst.right-rc.left;
	rcBlock.bottom=rcRst.bottom-rc.top;
	rcBlock.top=rcRst.top-rc.top;
	//�����ཻ�����������ڵ�ǰ�����������ڵ�ֽ�ŵ�λ�ã�����Canvas�ϵ�λ�ã�rcRst.
	RECT& rcInPaper=paper.GetRectInPaper();
	rcRst.left+=rcInPaper.left;
	rcRst.right+=rcInPaper.left;
	rcRst.top+=rcInPaper.top;
	rcRst.bottom+=rcInPaper.top;
	//paper.AddSection(rcRst);
	//***********************************************************************
	//��ʼ��
	PAPERSTRUCT& ppStruct=paper.GetPPStruct();
	double waterSum=0,pigSum=0;
	double waterEach=m_water/m_bristleNum;
	double pigEach=m_pigment.m_quan/m_bristleNum;

//����������īˮ��ë������ֽ�ŵ�����ͬʱ�õ�ë����ʣ��īˮ����ĳһ��ʱ�̣�ë����īˮ��ƽ�����䵽ÿ����ë��).
	for(int y=rcRst.top;y<rcRst.bottom;++y){
		PAPERLINE& papelRow=ppStruct.getRow(y);
		PAPERLINE::iterator it=papelRow.begin()+rcRst.left;
		BYTE* ptr=mask.GetBits(rcBlock.left,rcBlock.top+y-rcRst.top);
		for(int x=rcRst.left;x<rcRst.right;++it,++x,ptr=ptr+1){
			if((*ptr)==0xFF){
				double wq=(*it).m_waterQuan;
				if(wq<1){
					//īˮ��ë������ֽ�Ų�����ֽ�ŵ�ǰpapel��������m)absorb�йأ����Ҹ�papel������ˮ���Ӵ�С�й�wq,APHLAΪ����������
					double ratio=((*it).m_absorb)*(1-wq)*(1-wq)*APHLA;
					double flowWater=ratio*waterEach;
					double flowPig=ratio*pigEach;

					//�����Ƿ���Ҫ��ˢ��īˮ������Ч��
					if(IsInkReduce()){
						waterSum+=flowWater;
						pigSum+=flowPig;
					}
					//��papel����ˮ����
					(*it).m_waterQuan+=flowWater;
					//��papel����ī����
					(*it).AddPigmentOnSameLayer(flowPig);
				}
			}
		}
	}
	//��ˢ��ʣ���īˮ����
	m_water=m_water-waterSum;
	m_pigment.m_quan=m_pigment.m_quan-pigSum;
}
//���Ʊʴ������Ҳ������ֱʴ��ĴӴ�С��һϵ�бʴ�mask�����Լӿ�����ٶȡ�
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