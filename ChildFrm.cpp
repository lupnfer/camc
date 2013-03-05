// ChildFrm.cpp : CChildFrame 类的实现
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


// CChildFrame 构造/析构

CChildFrame::CChildFrame()
{
	// TODO: 在此添加成员初始化代码
	//this->Create(
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CBCGPMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;
	
	return TRUE;
}


// CChildFrame 诊断

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
	// TODO: 在此处添加消息处理程序代码
}



int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	HMENU hMenu;

	hMenu = ::GetSystemMenu(this->m_hWnd,FALSE);
	EnableMenuItem(hMenu,SC_CLOSE | MF_BYCOMMAND, MF_GRAYED);


	return 0;
}
