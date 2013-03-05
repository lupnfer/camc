#pragma once
#include "afxext.h"
#include "Resource.h"
class CDlgMagicBar :
	public CDialogBar
{
	DECLARE_DYNAMIC(CDlgMagicBar)
public:
	CDlgMagicBar(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMagicBar();
	virtual BOOL Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID);

	// Dialog Data
	enum { IDD = IDD_DIALOG_MAGIC };
public:
	BOOL InitDialogBar();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_threshold;
	CStatic m_showthreshold;

};
