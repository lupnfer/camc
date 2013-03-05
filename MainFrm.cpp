// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "CAMC.h"

#include "MainFrm.h"
#include ".\mainfrm.h"
#include "CAMCDoc.h"
#include "ObjLayer.h"
#include "PartsDatabase.h"
#include "Hvidicon.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_LAYERBAR, OnLayerbar)
	ON_WM_MDIACTIVATE()

	ON_COMMAND(ID_PREVIEWDLG, OnPreviewdlg)
	ON_COMMAND(ID_COLORDBMANAGER, OnColordbmanager)

	ON_COMMAND(ID_VIEW_OUTPUT, &CMainFrame::OnViewOutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, &CMainFrame::OnUpdateViewOutput)
	ON_UPDATE_COMMAND_UI(ID_LAYERBAR, &CMainFrame::OnUpdateLayerbar)
	ON_COMMAND(ID_IMPORT_PARAMER, &CMainFrame::OnImportParamer)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	//theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2005);
	CHvidicon::Instance()->BeginDevice();
	CPartsDatabase::Instance();
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	//-------------------
	// Create status bar:
	//-------------------
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetPaneStyle (0, SBPS_STRETCH);

	//------------------------------
	// Update status bar indicators:
	//------------------------------
	m_wndStatusBar.SendMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE);

	
	//CreateOutputWnd();
	
	// TODO: 如果不需要工具栏可停靠，则删除这三行
	
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	

	CreateLayerBar();

	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_fcLayerBar);

	if (!CreateDockingBars ())
	{
		return -1;
	}
	m_wndPropertiesBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndWatchBar.EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndWatchBar);
	CBCGPDockingControlBar* pTabbedBar = NULL; 
	m_wndPropertiesBar.AttachToTabWnd (&m_wndWatchBar, BCGP_DM_SHOW, TRUE, &pTabbedBar);
	
	


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序
//------------------------------------------
//显示图层工具栏
void CMainFrame::OnLayerbar()
{
	// TODO: 在此添加命令处理程序代码
	m_fcLayerBar.ShowControlBar(!m_fcLayerBar.IsVisible(),FALSE,TRUE);

}
void CMainFrame::CreateLayerBar()
{
	if(!m_fcLayerBar.Create(this,IDD_LAYERDLG,WS_CHILD|WS_VISIBLE|CBRS_RIGHT
						|CBRS_FLYBY | CBRS_SIZE_DYNAMIC,IDD_LAYERDLG))
	{		  
		TRACE0("Failed to create dialogbar\n");
		return;
	}
	m_fcLayerBar.EnableDocking(CBRS_ALIGN_ANY);
	
}


void CMainFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIFrameWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	// TODO: 在此处添加消息处理程序代码
}




BOOL CMainFrame::CreateDockingBars()
{
	
	//////////////////////////////////New Output////////////////////////////////////////
	if (!m_wndOutputView.Create (_T("Output"), this, CRect (0, 0, 200, 100),
		TRUE, 
		ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create output bar\n");
		return FALSE;      // fail to create
	}
	if (!m_wndWatchBar.Create(_T("识别结果"),this, CRect(0,0,200,100),TRUE, ID_VIEW_WATCH,WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create watch bar\n");
		return FALSE;      // fail to create
	}
	
	if (!m_wndPropertiesBar.Create (_T("参数配置"), this, CRect (0, 0, 300, 200),
		TRUE, 
		ID_VIEW_PROPERTIES,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties Bar\n");
		return FALSE;		// fail to create
	}
	return TRUE;

}

void CMainFrame::OnPreviewdlg()
{
	

}

CDocument* CMainFrame::GetActiveDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	this->m_fcLayerBar.InitImageList();
	return CMDIFrameWnd::GetActiveDocument();
	
}

void CMainFrame::OnColordbmanager()
{

}
void CMainFrame::CreateBar()
{
}


void  CMainFrame::SetStatusBarText(CString str)
{
	this->m_wndStatusBar.SetPaneText(0,str);
}

void  CMainFrame::OnClose()
{
	//CPartsDatabase::Instance()->Save();
	HVidicom->OnStopShot();
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnViewOutput()
{
	// TODO: 在此添加命令处理程序代码
	m_wndWatchBar.ShowControlBar (!m_wndWatchBar.IsVisible (), FALSE, TRUE);
}

void CMainFrame::OnUpdateViewOutput(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck (m_wndWatchBar.IsVisible ());
}

void CMainFrame::OnUpdateLayerbar(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck (m_fcLayerBar.IsVisible ());
}

void CMainFrame::OnImportParamer()
{
	// TODO: 在此添加命令处理程序代码
	HVidicom->ResetConfig();
}
