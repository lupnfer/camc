#pragma once


// CNewDlg �Ի���

class CNewDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewDlg)

public:
	CNewDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNewDlg();

// �Ի�������
	enum { IDD = IDD_NewDlg };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
