#include "StdAfx.h"
#include ".\cmdselectionset.h"
#include "ObjCanvas.h"
#include "ObjProgress.h"
//=================================================================================
// 设置区域/set region
void  CCmdSelectionSet::Execute (FCObjCanvas & canvas, FCObjProgress * Percent)
{
	m_pCanvas = &canvas ;
	Redo();
}
void  CCmdSelectionSet::Undo ()
{
	this->Redo();
}
void  CCmdSelectionSet::Redo ()
{
	FCObjSelect		temp(m_pCanvas->GetSelection()) ; // 交换保存的region
	m_pCanvas->__SetRegionMember (m_Select) ;
	m_Select = temp ;
}
//=================================================================================