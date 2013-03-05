// ChildFrm.cpp : CChildFrame ���ʵ��
//
#include "stdafx.h"
#include "CAMC.h"

#include "ChildFrm.h"
#include ".\childfrm.h"
#include "FCLayerBar.h"
#include "CAMCDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CBCGPMDIChildWnd )

BEGIN_MESSAGE_MAP(CChildFrame, CBCGPMDIChildWnd)


	ON_WM_MDIACTIVATE()

	ON_WM_CREATE()
END_MESSAGE_MAP()


// CChildFrame ����/����

CChildFrame::CChildFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
	//this->Create(
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸� CREATESTRUCT cs ���޸Ĵ��������ʽ
	if( !CBCGPMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;
	
	return TRUE;
}


// CChildFrame ���

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG



void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	if(pActivateWnd!=NULL){
		CMDIChildWnd* pWnd=static_cast<CMDIChildWnd*>(pActivateWnd);
		CCAMCDoc* pDoc=(CCAMCDoc*)pWnd->GetActiveDocument();
		pDoc->InitLayerbar();
	}
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	// TODO: �ڴ˴������Ϣ����������
}



int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	HMENU hMenu;

	hMenu = ::GetSystemMenu(this->m_hWnd,FALSE);
	EnableMenuItem(hMenu,SC_CLOSE | MF_BYCOMMAND, MF_GRAYED);


	return 0;
}
