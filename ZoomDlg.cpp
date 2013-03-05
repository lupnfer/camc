// ZoomDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CAMC.h"
#include "ZoomDlg.h"
#include ".\zoomdlg.h"


// CZoomDlg �Ի���

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


// CZoomDlg ��Ϣ�������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	OnOK();
}



void CZoomDlg::OnCancel()
{
	m_zoomStr="-1";
	CDialog::OnCancel();
}