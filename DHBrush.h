//////////////////////////////////////////////////////////////////
//																//
//		��; : ʵ�����⻭ˢ����							//
//		���� : [������] / 2005-2-21							//
//		���� : 2005-2-21										//						//							//
//////////////////////////////////////////////////////////////////

#pragma once
#include "objbase.h"

#include <list>
#include "ParentBrush.h"
#include "stdDefine.h"
#define MAXLENGTH  10000  





class CDHBrush :
	public CParentBrush
{
public:
	CDHBrush(void){
		m_r=0;
		m_length=0;
		m_lpSection=new SP[MAXLENGTH];
	}
	~CDHBrush(void){
		if(m_lpSection!=NULL)
			delete[] m_lpSection;
		m_lpSection=NULL;
	}
	void InitSection(int r)
	{
		m_r=r;
		m_length=10000;
		for(int i=0;i<m_length;++i)
		{			
			m_lpSection[i].m_bUse=false;
			m_lpSection[i].m_c=RGB(255,255,255);
		}
	}
public:
	void GenSection(int r);
	void GenCircleSection(int r);
	LPSECTION GetSection(){return m_lpSection;}
	
private:
	void FillLine(int start_x,int start_y);
	
protected:
	LPSECTION m_lpSection;//��ˢ��ֽ�Ų����Ľ��棬����ڽ������ĵ�����
	int       m_length;
};
