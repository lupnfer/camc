#pragma once


// CZoomDlg 对话框

class CZoomDlg : public CDialog
{
	DECLARE_DYNAMIC(CZoomDlg)

public:
	CZoomDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CZoomDlg();

// 对话框数据
	enum { IDD = IDD_STROKEWIDTHZOOMDLG };
	float GetZoomRatio();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual void OnCancel();
	CString m_zoomStr;
};
