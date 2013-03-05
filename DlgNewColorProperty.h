#pragma once


// CDlgNewColorProperty dialog

class CDlgNewColorProperty : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewColorProperty)

public:
	CDlgNewColorProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNewColorProperty();
	CDlgNewColorProperty(int r,int g, int b,CWnd* pParent = NULL);
// Dialog Data
	enum { IDD = IDD_DLGNEWCOLORPROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_colorname;
	virtual BOOL OnInitDialog();
	CString GetColorName(void);
	afx_msg void OnBnClickedOk();
	void SetRGBProperty(int rvalue, int gvalue, int bvalue);
	// 红色分量
	int m_red;
	// 绿色分量
	int m_green;
	// 蓝色分量
	int m_blue;
};
