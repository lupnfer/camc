// NewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CAMC.h"
#include "NewDlg.h"
#include "Hvidicon.h"

// CNewDlg 对话框

IMPLEMENT_DYNAMIC(CNewDlg, CDialog)
CNewDlg::CNewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewDlg::IDD, pParent)
{
	int w,h;
	HVidicom->GetCaptureResolution(w,h);
	if (w==0&&h==0)
	{
		w=640;
		h=512;
	}
	m_width.Format("%d",w/2);
	m_high.Format("%d",h/2);
	m_dpi=_T("72");

}

CNewDlg::~CNewDlg()
{
}

void CNewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_width);
	DDX_Text(pDX, IDC_EDIT2, m_high);
	DDX_Text(pDX, IDC_EDIT3, m_dpi);
}
int CNewDlg::GetWidth()
{
	LPTSTR lpsz = new TCHAR[m_width.GetLength()+1];
	_tcscpy(lpsz,m_width); 
	int threshold=atof(lpsz);
	delete[] lpsz;
	return threshold;
}
int CNewDlg::GetHigh()
{
	LPTSTR lpsz = new TCHAR[m_high.GetLength()+1];
	_tcscpy(lpsz,m_high); 
	int threshold=atof(lpsz);
	delete[] lpsz;
	return threshold;
}
int CNewDlg::GetDPI()
{
	LPTSTR lpsz = new TCHAR[m_dpi.GetLength()+1];
	_tcscpy(lpsz,m_dpi); 
	int threshold=atof(lpsz);
	delete[] lpsz;
	return threshold;
}

BEGIN_MESSAGE_MAP(CNewDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CNewDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CNewDlg 消息处理程序

void CNewDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

BOOL CNewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	OnBnClickedOk();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
