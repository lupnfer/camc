#pragma once

#include "CMSComm1.h"
#include "afxwin.h"
// DialogSend �Ի���


class DialogSend : public CDialog
{
	DECLARE_DYNAMIC(DialogSend)

public:
	DialogSend(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DialogSend();

// �Ի�������
	enum { IDD = IDD_DIALOG_SEND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CMSComm m_Comm;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnCbnSelchangeCombo1();
	int m_port;
	CComboBox m_cbox;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
