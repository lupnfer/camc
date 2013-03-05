#pragma once
#include "fcimageproc.h"

class CCannyEdge :public FCImageProc
{
public:
	CCannyEdge(void);
	~CCannyEdge(void);
	class Paramer:public FCImageProc::Paramer
		{
			public:
				Paramer(){
					m_deviation=5;
					m_min=0.02;
					m_max=0.5;
					
				}
			public:
				float m_min;
				float m_max;
				float m_deviation;
		};
	virtual void SetParamer(Paramer& paramer);
	virtual void Apply(FCObjImage& imgSrc,FCObjImage& imgDst,FCObjProgress * Percent = NULL);
private:
	Paramer m_para;
};
