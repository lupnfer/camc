#pragma once
#include "afxext.h"
#include "Resource.h"
class CDlgPenBar :
	public CDialogBar
{

DECLARE_DYNAMIC(CDlgPenBar)

public:
	CDlgPenBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPenBar();
	virtual BOOL Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID);

	// Dialog Data
	enum { IDD = IDD_DIALOG_PEN };
public:
	BOOL InitDialogBar();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioCircle();
	afx_msg void OnBnClickedRadioEllipse();
	afx_msg void OnDeltaposSpinPenwidth(NMHDR *pNMHDR, LRESULT *pResult);
	int m_penwidth;
	bool m_Iscircle;
	CButton m_radio_circle;
	int OnChangePenwidth(void);
	CStatic m_showpenwidth;
	CSpinButtonCtrl m_spinbuttonctrl_penwidth;
};
