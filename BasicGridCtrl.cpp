//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a sample for BCGControlBar Library Professional Edition
// Copyright (C) 1998-2008 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BasicGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "camc.h"
#include "BasicGridCtrl.h"
#include "PartsDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl

CBasicGridCtrl::CBasicGridCtrl()
{
}

CBasicGridCtrl::~CBasicGridCtrl()
{
}

BEGIN_MESSAGE_MAP(CBasicGridCtrl, CBCGPGridCtrl)
	//{{AFX_MSG_MAP(CBasicGridCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBasicGridCtrl message handlers

int CBasicGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableMarkSortedColumn (FALSE);
	EnableHeader (TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);
	SetSingleSel(TRUE);
	SetWholeRowSel();
	return 0;
}

void CBasicGridCtrl::OnDestroy() 
{
	SaveState (_T("BasicGrid"));
	CBCGPGridCtrl::OnDestroy();
}
void CBasicGridCtrl::OnItemChanged (CBCGPGridItem* pItem, int nRow, int nColumn)
{
	//pItem->get
	PartsInfoList& ptInfoLst=PartsDB->GetPartsInfoList();
	CBCGPGridRow* pRow=GetRow(nRow);
	_variant_t val=pRow->GetItem(0)->GetValue();

	int id=(long)val;
	Iterator it=PartsDB->Find(id);
	if(nColumn==1){
		it->name=(char*)(bstr_t)(pItem->GetValue());	
	}else if (nColumn==2){
		it->angle=(double)pItem->GetValue();
	}else if (nColumn==3){
		it->rectangularity=(double)pItem->GetValue();
	}else if (nColumn==4){
		it->eolngatedness=(double)pItem->GetValue();
	}else if (nColumn==5){
		it->compactness=(double)pItem->GetValue();
	}
}
void CBasicGridCtrl::OnSelChanged(const CBCGPGridRange&range,BOOL bSelect)
{
	CBCGPGridRow* pNewSel=GetCurSel();
	int id=(long)(pNewSel->GetItem(0)->GetValue());
	Iterator it=PartsDB->Find(id);
	if(it==PartsDB->GetPartsInfoList().end())
		return;
	IplImage* pImg=it->pTemplate;
	if(pImg)
		::SendMessage(GetParent()->m_hWnd,WM_SHOW_MESSAGE,0,(LPARAM)pImg);
}
void CBasicGridCtrl::InitGridColumn()
{
	InsertColumn(0,_T("零件ID"),80);
	InsertColumn(1,_T("零件名称"),80);
	InsertColumn(2,_T("零件朝向"),80);
	InsertColumn(3,_T("矩形度"),80);
	InsertColumn(4,_T("细长度"),80);
	InsertColumn(5,_T("紧密度"),80);
}
void CBasicGridCtrl::ShowRecordSet()
{
	PartsInfoList& ptInfoLst=PartsDB->GetPartsInfoList();
	Iterator it=ptInfoLst.begin();
	for(;it!=ptInfoLst.end();++it){
		AddRowBack(*it);
	}
}
void CBasicGridCtrl::AddRowBack(PartsInfo& ptInfo)
{
	CBCGPGridRow* pRow = CreateRow (GetColumnCount ());
	pRow->GetItem (0)->SetValue (ptInfo.id);
	pRow->GetItem (0)->AllowEdit(FALSE);
	_variant_t strName(ptInfo.name.c_str());
	CBCGPGridItem* pItem;
	pRow->GetItem (1)->SetValue (strName);
	pRow->GetItem (2)->SetValue (ptInfo.angle);
	pRow->GetItem (3)->SetValue (ptInfo.rectangularity);
	pRow->GetItem (4)->SetValue (ptInfo.eolngatedness);
	pRow->GetItem (5)->SetValue (ptInfo.compactness);
	AddRow (pRow);
}