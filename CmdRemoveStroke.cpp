#include "StdAfx.h"
#include ".\cmdremovestroke.h"
#include "objLayer.h"


void CCmdRemoveStroke::Implement (FCObjImage & img, FCObjProgress * Percent )
{
	m_pSketch->Remove(m_pStroke);		
	m_pSketch->ReDraw(*m_pLayer,m_pStroke->GetRect(),Percent);
}
void  CCmdRemoveStroke::Undo ()
{
	m_pSketch->Add(m_pStroke);
	CCmdImgEffect::Undo();
}
void  CCmdRemoveStroke::Redo ()
{
	m_pSketch->Remove(m_pStroke);
	CCmdImgEffect::Redo();
}