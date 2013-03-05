#pragma once


// CNewDlg 对话框

class CNewDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewDlg)

public:
	CNewDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewDlg();

// 对话框数据
	enum { IDD = IDD_NewDlg };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_width;
	CString m_high;
	CString m_dpi;
	int GetWidth();
	int GetHigh();
	int GetDPI();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
