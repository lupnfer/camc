#pragma once
#include "cmscomm.h"
#include "afxwin.h"


// DlgSend �Ի���

class DlgSend : public CDialog
{
	DECLARE_DYNAMIC(DlgSend)

public:
	DlgSend(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgSend();

// �Ի�������
	enum { IDD = IDD_DIALOG_SEND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
	afx_msg void OnBnClickedButton1();
	CMSComm m_ctrlComm;
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox m_portnum;
	int m_port;
};
