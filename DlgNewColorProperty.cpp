// DlgNewColorProperty.cpp : implementation file
//

#include "stdafx.h"
#include "CAMC.h"
#include "DlgNewColorProperty.h"
#include ".\dlgnewcolorproperty.h"


// CDlgNewColorProperty dialog

IMPLEMENT_DYNAMIC(CDlgNewColorProperty, CDialog)
CDlgNewColorProperty::CDlgNewColorProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewColorProperty::IDD, pParent)
	, m_colorname(_T(""))
	, m_red(0)
	, m_green(0)
	, m_blue(0)
{

}
CDlgNewColorProperty::CDlgNewColorProperty(int r,int g, int b,CWnd* pParent): CDialog(CDlgNewColorProperty::IDD, pParent)

{
	m_red = r;
	m_green = g;
	m_blue = b;
}

CDlgNewColorProperty::~CDlgNewColorProperty()
{
}

void CDlgNewColorProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_COLORNAME, m_colorname);

}


BEGIN_MESSAGE_MAP(CDlgNewColorProperty, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgNewColorProperty message handlers

BOOL CDlgNewColorProperty::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_colorname = "ÐÂÑÕÉ«";
	m_red = 11;
	m_green = 12;
	m_blue = 13;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CString CDlgNewColorProperty::GetColorName(void)
{
	
	return m_colorname;
	
}

void CDlgNewColorProperty::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	OnOK();
}

void CDlgNewColorProperty::SetRGBProperty(int rvalue, int gvalue, int bvalue)
{
/*	CString str;
	str.Format("%d",rvalue);
	this->SetDlgItemText(IDC_STATIC_R ,str);
	str.Format("%d",gvalue);
	SetDlgItemText(IDC_STATIC_G ,str);
	str.Format("%d",bvalue);
	SetDlgItemText(IDC_STATIC_B ,str);
*/
}
