#pragma once


// ID_DIALOG_TEST 对话框

class ID_DIALOG_TEST : public CDialog
{
	DECLARE_DYNAMIC(ID_DIALOG_TEST)

public:
	ID_DIALOG_TEST(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ID_DIALOG_TEST();

// 对话框数据
	enum { IDD = IDD_DIALOG_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
