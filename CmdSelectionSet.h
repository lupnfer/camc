#pragma once
#include "cmdselectionbase.h"
#include "ObjSelect.h"
class CCmdSelectionSet :
	public CCmdSelectionBase
{
public :
	CCmdSelectionSet(const FCObjSelect & newSelect) : m_Select(newSelect) {}
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
protected :
	FCObjSelect		m_Select ;
};
