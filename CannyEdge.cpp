#include "StdAfx.h"
#include ".\cannyedge.h"

#include "ProgressBar.h"
CCannyEdge::CCannyEdge(void)
{
}

CCannyEdge::~CCannyEdge(void)
{
}

void CCannyEdge::SetParamer(Paramer& paramer)
{
	m_para.m_min = paramer.m_min;
	m_para.m_max = paramer.m_max;
	m_para.m_deviation = paramer.m_deviation;

}

void CCannyEdge::Apply(FCObjImage& imgSrc,FCObjImage& imgDst,FCObjProgress * Percent)
{
	
}

