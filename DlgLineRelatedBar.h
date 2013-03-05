#pragma once
#include "afxext.h"
#include "Resource.h"
class CDlgLineRelatedBar :
	public CDialogBar
{

DECLARE_DYNAMIC(CDlgLineRelatedBar)
public:
	CDlgLineRelatedBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLineRelatedBar();
	virtual BOOL Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID);

	// Dialog Data
	enum { IDD = IDD_DIALOG_LINERELATED };
public:
	BOOL InitDialogBar();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDeltaposSpinPenwidth(NMHDR *pNMHDR, LRESULT *pResult);
	int m_penwidth;
	int OnChangePenwidth(void);
	CStatic m_showpenwidth;
	CSpinButtonCtrl m_spinbuttonctrl_penwidth;
};
