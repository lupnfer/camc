// ID_DIALOG_TEST.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CAMC.h"
#include "ID_DIALOG_TEST.h"


// ID_DIALOG_TEST �Ի���

IMPLEMENT_DYNAMIC(ID_DIALOG_TEST, CDialog)

ID_DIALOG_TEST::ID_DIALOG_TEST(CWnd* pParent /*=NULL*/)
	: CDialog(ID_DIALOG_TEST::IDD, pParent)
{

}

ID_DIALOG_TEST::~ID_DIALOG_TEST()
{
}

void ID_DIALOG_TEST::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ID_DIALOG_TEST, CDialog)
END_MESSAGE_MAP()


// ID_DIALOG_TEST ��Ϣ�������
