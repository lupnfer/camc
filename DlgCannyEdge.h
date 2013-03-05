#pragma once


// CDlgCannyEdge dialog

class CDlgCannyEdge : public CDialog
{
	DECLARE_DYNAMIC(CDlgCannyEdge)

public:
	CDlgCannyEdge(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCannyEdge();

// Dialog Data
	enum { IDD = IDD_CANNYEDGEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float GetMinThreshold(void);
	float GetMaxThreshold(void);
	float GetDeviation(void);
	virtual BOOL OnInitDialog();

protected:
	CString m_deviation;
	CString m_max;
	CString m_min;
};
