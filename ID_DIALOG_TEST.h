#pragma once


// ID_DIALOG_TEST �Ի���

class ID_DIALOG_TEST : public CDialog
{
	DECLARE_DYNAMIC(ID_DIALOG_TEST)

public:
	ID_DIALOG_TEST(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ID_DIALOG_TEST();

// �Ի�������
	enum { IDD = IDD_DIALOG_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
