#pragma once

#include "BasicGridCtrl.h"
// CDBDlg �Ի���

class CDBDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDBDlg)

public:
	CDBDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDBDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_DB };
	
	CStatic  m_dbListLocation;
	
	CStatic  m_partsPic;


	CBasicGridCtrl m_dbList;
public:
	void  ShowImage(IplImage* pImg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	afx_msg void OnBnNewParts();
	afx_msg void OnBnInsertFeature();
	afx_msg void OnBnDelete();
	afx_msg LRESULT OnShowMessage(WPARAM wParam, LPARAM lParam); 

	DECLARE_MESSAGE_MAP()
};
