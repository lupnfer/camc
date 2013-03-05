#pragma once


// CLayerPropertyDlg dialog

class CLayerPropertyDlg : public CDialog
{
	DECLARE_DYNAMIC(CLayerPropertyDlg)

public:
	CLayerPropertyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayerPropertyDlg();

// Dialog Data
	enum { IDD = IDD_LAYERPROPERTYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_alpha;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	void AdjustTransparent(void);
	afx_msg void OnBnClickedProcontour();
};
