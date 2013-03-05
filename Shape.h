#pragma once
#include "objbase.h"
#include "objSelect.h"
#include "VBDefine.h"
/***************************
*���ߣ�������
*ԭʼ������ڣ�2006.9.26
*��;����ˢ��״�Ķ��壬����ʵ�ָ��ַ����״�ı�ˢ 
*���߱����޶����ڣ�
*�������޶��������޸��ߣ��޸����ڣ��޸ĵط����޸�ǰ�����ݣ��޸ĺ����ݣ����õȣ�      
***************************/

struct BrushWork{
	FCObjSelect m_mask;
	int         m_maskNum;
};
typedef std::vector<BrushWork> BWLIST;//�������ϵĺ����б�
class CShape :
	public FCObject
{
public:
	CShape();
	CShape(const CShape& shape){
		m_a=shape.m_a;
		m_b=shape.m_b;
		m_angle=shape.m_angle;
	}
	virtual ~CShape(void);
protected:
	float m_a;//��Բ�ĺ���
	float m_b;//��Բ������
	int m_angle;

public:
	void Init(float a,float b,int angle){
		m_a=a;
		m_b=b;
		m_angle=angle;
	}
	float GetA(){return m_a;}
	float GetB(){return m_b;}
	float GetAngle(){return m_angle;}
	virtual int  GenShape(BrushWork& bw);//�����ʴ���������ë������
	virtual bool  IsValid(){	if(m_a>0&&m_b>0) return true; return false;	}

};
