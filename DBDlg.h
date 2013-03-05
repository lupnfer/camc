#pragma once

#include "BasicGridCtrl.h"
// CDBDlg 对话框

class CDBDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDBDlg)

public:
	CDBDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDBDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_DB };
	
	CStatic  m_dbListLocation;
	
	CStatic  m_partsPic;


	CBasicGridCtrl m_dbList;
public:
	void  ShowImage(IplImage* pImg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg void OnBnNewParts();
	afx_msg void OnBnInsertFeature();
	afx_msg void OnBnDelete();
	afx_msg LRESULT OnShowMessage(WPARAM wParam, LPARAM lParam); 

	DECLARE_MESSAGE_MAP()
};
