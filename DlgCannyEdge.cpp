// DlgCannyEdge.cpp : implementation file
//

#include "stdafx.h"
#include "CAMC.h"
#include "DlgCannyEdge.h"
#include ".\dlgcannyedge.h"


// CDlgCannyEdge dialog

IMPLEMENT_DYNAMIC(CDlgCannyEdge, CDialog)
CDlgCannyEdge::CDlgCannyEdge(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCannyEdge::IDD, pParent)
	, m_deviation(_T(""))
	, m_max(_T(""))
	, m_min(_T(""))
{
}

CDlgCannyEdge::~CDlgCannyEdge()
{
}

void CDlgCannyEdge::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MINEDIT, m_min);
	DDX_Text(pDX, IDC_MAXEDIT, m_max);
	DDX_Text(pDX, IDC_DEVIATIONEDIT, m_deviation);

}


BEGIN_MESSAGE_MAP(CDlgCannyEdge, CDialog)
END_MESSAGE_MAP()


// CDlgCannyEdge message handlers

float CDlgCannyEdge::GetMinThreshold(void)
{
	LPTSTR lpsz = new TCHAR[m_min.GetLength()+1];
	_tcscpy(lpsz,m_min); 
	float min=atof(lpsz);
	delete[] lpsz;
	return min;

}

float CDlgCannyEdge::GetMaxThreshold(void)
{
	LPTSTR lpsz = new TCHAR[m_max.GetLength()+1];
	_tcscpy(lpsz,m_max); 
	float max=atof(lpsz);
	delete[] lpsz;
	return max;

}

float CDlgCannyEdge::GetDeviation(void)
{
	LPTSTR lpsz = new TCHAR[m_deviation.GetLength()+1];
	_tcscpy(lpsz,m_deviation); 
	float deviation=atof(lpsz);
	delete[] lpsz;
	return deviation;

}

BOOL CDlgCannyEdge::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_deviation="5";
	m_min="0.007";
	m_max="0.05";
	UpdateData(false);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
