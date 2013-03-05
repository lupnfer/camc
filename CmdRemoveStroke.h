#pragma once
#include "cmdimgeffect.h"
#include "stetch.h"
#include "bcstroke.h"
#include "objCanvas.h"

class CCmdRemoveStroke :
	public CCmdImgEffect
{
public:
	CCmdRemoveStroke(CSketch* pSketch,CBCStroke* pStroke){
		m_pSketch=pSketch;
		m_pStroke=pStroke;
	}
	virtual ~CCmdRemoveStroke(void){}
public:
	virtual void  Undo () ;
	virtual void  Redo () ;
	virtual void Implement (FCObjImage & img, FCObjProgress * Percent=NULL);
protected:
	CSketch*  m_pSketch;
	CBCStroke* m_pStroke;
};
