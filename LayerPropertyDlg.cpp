// LAYERPROPERTYDLG.cpp : implementation file
//

#include "stdafx.h"
#include "CAMC.h"
#include "LayerPropertyDlg.h"
#include "CAMCDoc.h"
#include "ObjCanvas.h"
#include "ObjLayer.h"
#include "FCSinglePixelProcessBase.h"
#include "LayerCommand.h"
// CLayerPropertyDlg dialog

IMPLEMENT_DYNAMIC(CLayerPropertyDlg, CDialog)
CLayerPropertyDlg::CLayerPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayerPropertyDlg::IDD, pParent)
	, m_alpha(255)
{
}

CLayerPropertyDlg::~CLayerPropertyDlg()
{
}

void CLayerPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLayerPropertyDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_PROCONTOUR, OnBnClickedProcontour)
END_MESSAGE_MAP()


// CLayerPropertyDlg message handlers

void CLayerPropertyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl *pSlide = (CSliderCtrl *)pScrollBar;
	CString str;
	str.Format("%d",pSlide->GetPos());
	m_alpha = pSlide->GetPos();
	UpdateData(TRUE);
	SetDlgItemText(IDC_STATIC_SLIDE,str);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CLayerPropertyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CSliderCtrl  *pSlide = (CSliderCtrl  *)GetDlgItem(IDC_SLDALPHA);
	pSlide->SetRange(0,255);
	pSlide->SetPos(m_alpha);
	CString str;
	str.Format("%d",pSlide->GetPos());
	m_alpha = pSlide->GetPos();
	UpdateData(TRUE);
	SetDlgItemText(IDC_STATIC_SLIDE,str);
	AdjustTransparent();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLayerPropertyDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	AdjustTransparent();
	OnOK();
}

void CLayerPropertyDlg::AdjustTransparent(void)
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());
	FCObjCanvas& pCanvas = pDoc->GetCanvas();
	FCLayerBar *pLayerBar = pDoc ->GetLayerBar();
	int nSelectedNum = pLayerBar->m_ListBox.GetCurSel();
	FCObjLayer *pLayer = pCanvas.GetLayer(nSelectedNum);
	pLayer->SetLayerTransparent(m_alpha);
	pDoc->UpdateAllViews(NULL);
}

void CLayerPropertyDlg::OnBnClickedProcontour()
{
	// TODO: Add your control notification handler code here
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());
	FCObjCanvas& pCanvas = pDoc->GetCanvas();
	FCLayerBar *pLayerBar = pDoc ->GetLayerBar();
	int nSelectedNum = pLayerBar->m_ListBox.GetCurSel();
	FCObjLayer *pLayer = pCanvas.GetLayer(nSelectedNum);
	pCanvas.ExecuteEffect(new FCCmdContourLayerSetProperty(pLayer, m_alpha, TRUE, FALSE));
	pDoc->UpdateAllViews(NULL);
	OnOK();

}
