// DlgRemoveNoise.cpp : implementation file
//

#include "stdafx.h"
#include "CAMC.h"
#include "DlgRemoveNoise.h"
#include ".\dlgremovenoise.h"


// CDlgRemoveNoise dialog

IMPLEMENT_DYNAMIC(CDlgRemoveNoise, CDialog)
CDlgRemoveNoise::CDlgRemoveNoise(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRemoveNoise::IDD, pParent)
	, m_radioone(_T(""))
	, m_radiotwo(_T(""))
{
}

CDlgRemoveNoise::~CDlgRemoveNoise()
{
}

void CDlgRemoveNoise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RADIOONE, m_radioone);
	DDX_Text(pDX, IDC_RADIOTWO, m_radiotwo);
}


BEGIN_MESSAGE_MAP(CDlgRemoveNoise, CDialog)
END_MESSAGE_MAP()


// CDlgRemoveNoise message handlers

BOOL CDlgRemoveNoise::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_radioone="3";
	m_radiotwo="5";
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgRemoveNoise::GetMaxRadio(void)
{
	LPTSTR lpsz = new TCHAR[m_radiotwo.GetLength()+1];
	_tcscpy(lpsz,m_radiotwo); 
	float min=atoi(lpsz);
	delete[] lpsz;
	return min;

}

int CDlgRemoveNoise::GetMinRadio(void)
{
	LPTSTR lpsz = new TCHAR[m_radioone.GetLength()+1];
	_tcscpy(lpsz,m_radioone); 
	float min=atoi(lpsz);
	delete[] lpsz;
	return min;

}

void CDlgRemoveNoise::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}
