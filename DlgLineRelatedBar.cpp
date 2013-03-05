#include "StdAfx.h"
#include ".\dlglinerelatedbar.h"

IMPLEMENT_DYNAMIC(CDlgLineRelatedBar, CDialogBar)
CDlgLineRelatedBar::CDlgLineRelatedBar(CWnd* pParent /*=NULL*/)
: CDialogBar(/*FCLayerBar::IDD, pParent*/)
{

}

CDlgLineRelatedBar::~CDlgLineRelatedBar(void)
{
}
void CDlgLineRelatedBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SHOWPENWIDTH, m_showpenwidth);
	DDX_Control(pDX, IDC_SPIN_PENWIDTH, m_spinbuttonctrl_penwidth);
}

BEGIN_MESSAGE_MAP(CDlgLineRelatedBar, CDialogBar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PENWIDTH, OnDeltaposSpinPenwidth)
END_MESSAGE_MAP()

BOOL CDlgLineRelatedBar::Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID)
{
	BOOL bRes= CDialogBar::Create(pParentWnd,nIDTemplate,nStyle,nID );

	InitDialogBar();//在类中添加一个成员函数就可以了

	return bRes;
}

BOOL CDlgLineRelatedBar::InitDialogBar()
{


	UpdateData(FALSE);//这个一定要啊，这样就会有和CDialog一样的数据交换效果了

	m_spinbuttonctrl_penwidth.SetBase(1);
	m_spinbuttonctrl_penwidth.SetPos(1);
	m_spinbuttonctrl_penwidth.SetRange(1,1024);
	m_penwidth=1;
	return TRUE;
}



void CDlgLineRelatedBar::OnDeltaposSpinPenwidth(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	OnChangePenwidth();
	*pResult = 0;
}


int CDlgLineRelatedBar::OnChangePenwidth(void)
{
	UpdateData();
	CRect rect;
	CPen pen;
	CBrush brush;

	brush.CreateSolidBrush(RGB(255,255,255));
	CDC *pDC = m_showpenwidth.GetDC();
	m_penwidth = m_spinbuttonctrl_penwidth.GetPos();
	if (m_penwidth>30) {
		m_penwidth =30;
	}

	pen.CreatePen(PS_SOLID,	m_penwidth,RGB(0,0,0));
	CPen *oldpen = pDC->SelectObject(&pen);
	m_showpenwidth.GetClientRect(&rect);
	pDC->FillRect(&rect,&brush);
	brush.DeleteObject();
	pDC->MoveTo(rect.left+10,rect.Height()/2);
	pDC->LineTo(rect.right-10,rect.Height()/2);

	pDC->SelectObject(oldpen);
	m_showpenwidth.ReleaseDC(pDC);
	return m_penwidth;
}