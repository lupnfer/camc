#include "StdAfx.h"
#include ".\dlgmagicbar.h"
#include "Resource.h"
IMPLEMENT_DYNAMIC(CDlgMagicBar, CDialogBar)
CDlgMagicBar::CDlgMagicBar(CWnd* pParent /*=NULL*/)
: CDialogBar(/*FCLayerBar::IDD, pParent*/)
{
	m_threshold = 30;
}

CDlgMagicBar::~CDlgMagicBar(void)
{
}
void CDlgMagicBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgMagicBar, CDialogBar)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL CDlgMagicBar::Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID)
{
	BOOL bRes= CDialogBar::Create(pParentWnd,nIDTemplate,nStyle,nID );

	InitDialogBar();//在类中添加一个成员函数就可以了

	return bRes;
}

BOOL CDlgMagicBar::InitDialogBar()
{


	UpdateData(FALSE);//这个一定要啊，这样就会有和CDialog一样的数据交换效果了

	CSliderCtrl  *pSlideThreshold = (CSliderCtrl  *)GetDlgItem(IDC_SLIDER_THRESHOLED);
	pSlideThreshold->SetRange(0,200);
	pSlideThreshold->SetPos(30);

	CString str;
	str.Format("%d",pSlideThreshold->GetPos());
	SetDlgItemText(IDC_STATIC_THRESHOLD,str);
	m_threshold = pSlideThreshold->GetPos();

	return TRUE;
}

void CDlgMagicBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl  *pSlideThreshold = (CSliderCtrl  *)GetDlgItem(IDC_SLIDER_THRESHOLED);
	CString str;
	str.Format("%d",pSlideThreshold->GetPos());
	SetDlgItemText(IDC_STATIC_THRESHOLD,str);
	m_threshold = pSlideThreshold->GetPos();
	CDialogBar::OnHScroll(nSBCode, nPos, pScrollBar);
}
