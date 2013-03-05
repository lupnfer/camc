// DBDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CAMC.h"
#include "DBDlg.h"
#include "PartsDatabase.h"
#include "CvvImage.h"
// CDBDlg 对话框

IMPLEMENT_DYNAMIC(CDBDlg, CDialog)

CDBDlg::CDBDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CDBDlg::IDD, pParent)
{

}

CDBDlg::~CDBDlg()
{
}

void CDBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_STATIC_DBLIST_LOCATION, m_dbListLocation);
	DDX_Control(pDX, IDC_STATIC_PARTS_PICTURE, m_partsPic);
	
}


BEGIN_MESSAGE_MAP(CDBDlg, CDialog)
	ON_BN_CLICKED(IDNEWPARTS, OnBnNewParts)
	ON_BN_CLICKED(IDINSERTFEATURE, OnBnInsertFeature)
	ON_BN_CLICKED(IDDELETE, OnBnDelete)

	ON_MESSAGE(WM_SHOW_MESSAGE,OnShowMessage)

END_MESSAGE_MAP()


// CDBDlg 消息处理程序


BOOL CDBDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect rectGrid;
	m_dbListLocation.GetClientRect (&rectGrid);
	m_dbListLocation.MapWindowPoints (this, &rectGrid);

	m_dbList.Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectGrid, this, (UINT)-1);
	m_dbList.EnableHeader (TRUE, BCGP_GRID_HEADER_MOVE_ITEMS);

	m_dbList.InitGridColumn();
	m_dbList.ShowRecordSet();
	m_dbList.LoadState (_T("BasicGrid"));
	m_dbList.AdjustLayout ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CDBDlg::OnBnNewParts()
{
	PartsDB->NewParts();
}
void CDBDlg::OnBnInsertFeature()
{
	PartsInfoList ptInfoList;
	PartsDB->Insert(ptInfoList);
	for(Iterator it=ptInfoList.begin();it!=ptInfoList.end();++it)
		m_dbList.AddRowBack(*it);	
}
void CDBDlg::OnBnDelete()
{
	CBCGPGridRow* pRow=m_dbList.GetCurSel();
	if(pRow){
		int id=(long)(pRow->GetItem(0)->GetValue());
		PartsDB->Delete(id);
		m_dbList.RemoveRow(pRow->GetRowId());
	}
	
}
void CDBDlg::ShowImage(IplImage* img)
{
	CDC* pDC=m_partsPic.GetDC();
	HDC hDC=pDC->GetSafeHdc();
	CRect rect;
	m_partsPic.GetClientRect(&rect);
	// 求出图片控件的宽和高
	int rw = rect.right - rect.left;			
	int rh = rect.bottom - rect.top;
	// 读取图片的宽和高
	int iw = img->width;	
	int ih = img->height;
	// 使图片的显示位置正好在控件的正中
	int tx = (int)(rw - iw)/2;	
	int ty = (int)(rh - ih)/2;
	SetRect( rect, tx, ty, tx+iw, ty+ih );
	// 复制图片
	CvvImage cimg;
	cimg.CopyOf( img );
	// 将图片绘制到显示控件的指定区域内	
	cimg.DrawToHDC( hDC, &rect );	

	ReleaseDC( pDC );
}
LRESULT CDBDlg::OnShowMessage(WPARAM wParam, LPARAM lParam)
{
	IplImage* pImg=(IplImage*)lParam;
	ShowImage(pImg);
	return 1;
}