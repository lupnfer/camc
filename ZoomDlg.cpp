// ZoomDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CAMC.h"
#include "ZoomDlg.h"
#include ".\zoomdlg.h"


// CZoomDlg 对话框

IMPLEMENT_DYNAMIC(CZoomDlg, CDialog)
CZoomDlg::CZoomDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZoomDlg::IDD, pParent)
	, m_zoomStr(_T(""))
{

}

CZoomDlg::~CZoomDlg()
{
}

void CZoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_zoomStr);
}


BEGIN_MESSAGE_MAP(CZoomDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CZoomDlg 消息处理程序
float CZoomDlg::GetZoomRatio()
{
	LPTSTR lpsz = new TCHAR[m_zoomStr.GetLength()+1];
	_tcscpy(lpsz,m_zoomStr); 
	float zoomrate=atof(lpsz);
	delete[] lpsz;
	return zoomrate;
}
void CZoomDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	
	OnOK();
}



void CZoomDlg::OnCancel()
{
	m_zoomStr="-1";
	CDialog::OnCancel();
}