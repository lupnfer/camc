#pragma once


// CZoomDlg �Ի���

class CZoomDlg : public CDialog
{
	DECLARE_DYNAMIC(CZoomDlg)

public:
	CZoomDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CZoomDlg();

// �Ի�������
	enum { IDD = IDD_STROKEWIDTHZOOMDLG };
	float GetZoomRatio();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual void OnCancel();
	CString m_zoomStr;
};
