#include "StdAfx.h"
#include "OutputWnd.h"
#include "Hvidicon.h"

COutputWnd::COutputWnd(void)
{
}

COutputWnd::~COutputWnd(void)
{
}
void COutputWnd::OnSelChanged(const CBCGPGridRange&range,BOOL bSelect)
{
	CBCGPGridRow* pNewSel=GetCurSel();
	if(pNewSel==NULL)
		return;
	int id=(long)(pNewSel->GetItem(0)->GetValue());
	Iterator it=PartsDB->Find(id);
	if(it==PartsDB->GetPartsInfoList().end())
		return;
	HVidicom->ShowRecogntionResult(it->pTemplate);
	
}
void COutputWnd::InitGridColumn()
{
	InsertColumn(0,_T("ID"),80);
	InsertColumn(1,_T("����"),80);
	InsertColumn(2,_T("�Ƕ�"),80);
	InsertColumn(3,_T("λ��"),80);
	InsertColumn(4,_T("�����"),80);
}