#pragma once
#include "objbase.h"
#include "Shape.h"
#include "VirtualPaper.h"
#include "VBDefine.h"
#include "GraphicsGems.h"
/***************************
*���ߣ�������
*ԭʼ������ڣ�2006.9.26
*����޸����ڣ�2006.10.13
*��;��ʵ������ı�ˢ���� 
*���߱����޶����ڣ�
*�������޶��������޸��ߣ��޸����ڣ��޸ĵط����޸�ǰ�����ݣ��޸ĺ����ݣ����õȣ�      
***************************/

struct BrushAttr{
	double  m_water;       //ë����ˮ���Ӻ���	         
	Pigment m_pigment;     //ë����ī���Ӻ���
	int     m_bristleNum;  //���ë�����������ܲ��������ʴ�����������������ʴ��棨�û��趨����������ġ�

	//��ˢ�����Ʋ���
	BOOL    m_isInkReduce; //���Ʊ�ˢ���ƹ������Ƿ����īˮ
	CShape  m_shape;
};
class CVirtualBrush :
	public FCObjGraph
{
public:
	CVirtualBrush(){m_isInkReduce=true;}
	virtual ~CVirtualBrush(void){}
	static CVirtualBrush* Instance();
//protected:
public:
	BrushWork     m_brushWork;//����8λ�ĻҶ�ͼ��mask��������ʾ���ʴ�
	BWLIST        m_bwList;//���Ʋ����Ĵ�da��xiao��ĳ�ֱʴ�ϵ��
	static CVirtualBrush* _instance;
	//--------------------------------------------------------------------------
private:
	//���ǰ�īˮ��Ϊˮ���Ӻ����Ϸ��ӡ�
	double  m_water;       //ë����ˮ���Ӻ���	         
    Pigment m_pigment;     //ë����ī���Ӻ���
	int     m_bristleNum;  //���ë�����������ܲ��������ʴ�����������������ʴ��棨�û��趨����������ġ�

	//��ˢ�����Ʋ���
	BOOL    m_isInkReduce; //���Ʊ�ˢ���ƹ������Ƿ����īˮ
	CShape  m_shape;
protected:
	
public:
	void DrawSection(CVirtualPaper& paper,Point2 p,double press);
	int  GetBrushWork(double press);
	void Customizing(CShape& shape);
	void DipInk(Ink& ik,double brlInkSuck){//����īˮ
		m_pigment.m_pigAttr=ik.m_pigAttr;
		m_pigment.m_quan=(brlInkSuck*ik.m_percentage)*m_bristleNum;
		m_water=m_bristleNum*brlInkSuck-m_pigment.m_quan;
	}
	BOOL IsInkReduce(){return m_isInkReduce;}
	void SetInkReduceOrNot(BOOL isInkReduce){m_isInkReduce=isInkReduce;}
	void Init(CShape& shape,Ink& ik,double brlInkSuck){
		Customizing(shape);
		DipInk(ik,brlInkSuck);
	}
	void BeginOneStrokeDraw(FCObjImage* img,CVirtualPaper& paper,RECT& rcInPaper);
	void EndOneStrokeDraw(CVirtualPaper& paper);
	void GetBrushAttribute(BrushAttr& bAttr){
		bAttr.m_bristleNum=m_bristleNum;
		bAttr.m_isInkReduce=m_isInkReduce;
		bAttr.m_pigment=m_pigment;
		bAttr.m_shape=m_shape;
		bAttr.m_water=m_water;
	}
};
