// DlgSend.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CAMC.h"
#include "DlgSend.h"

// DlgSend �Ի���

IMPLEMENT_DYNAMIC(DlgSend, CDialog)

DlgSend::DlgSend(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSend::IDD, pParent)
	, m_port(1)
{

}

DlgSend::~DlgSend()
{
}

void DlgSend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_portnum);
}


BEGIN_MESSAGE_MAP(DlgSend, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &DlgSend::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &DlgSend::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// DlgSend ��Ϣ�������

BOOL DlgSend::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_portnum.SetCurSel(0);
	OnCbnSelchangeCombo1();
	
	

	m_ctrlComm.Create(NULL,0,CRect(0,0,0,0),this,IDC_MSCOMM1);
	m_ctrlComm.put_CommPort(m_port);                                  //���ô��ں�
	m_ctrlComm.put_InputMode(1);                                    //�ı����� ���
	m_ctrlComm.put_InBufferSize(2048);                          //�������뻺������С
	m_ctrlComm.put_OutBufferSize(2048);                       //���������������С
	m_ctrlComm.put_Settings("115200,n,8,1");                  //���ò����ʣ����ݸ�ʽ
	if(!m_ctrlComm.get_PortOpen())            //�򿪴���
	{
		m_ctrlComm.put_PortOpen(TRUE);
		AfxMessageBox(_T("�򿪴���"));
	}
	else
		AfxMessageBox("cannot open serial port");
	m_ctrlComm.put_RThreshold(1);                            //���Բ�����
	m_ctrlComm.put_SThreshold(1);
	m_ctrlComm.put_InputLen(0);                                //Ĭ��ÿ�ζ���ȫ������
	m_ctrlComm.get_Input();     















	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void DlgSend::OnBnClickedButton1()
{
	CString m_data;
	
	//m_data=app->m_putx+' '+app->m_puty+' '+app->m_putAG;
	m_ctrlComm.put_Output(COleVariant(m_data));
	AfxMessageBox(m_data);
	

	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void DlgSend::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int num=1;
	num+=m_portnum.GetCurSel();
	m_port=num;

}
