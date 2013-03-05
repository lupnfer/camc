// DlgSend.cpp : 实现文件
//

#include "stdafx.h"
#include "CAMC.h"
#include "DlgSend.h"

// DlgSend 对话框

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


// DlgSend 消息处理程序

BOOL DlgSend::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_portnum.SetCurSel(0);
	OnCbnSelchangeCombo1();
	
	

	m_ctrlComm.Create(NULL,0,CRect(0,0,0,0),this,IDC_MSCOMM1);
	m_ctrlComm.put_CommPort(m_port);                                  //设置串口号
	m_ctrlComm.put_InputMode(1);                                    //文本输入 输出
	m_ctrlComm.put_InBufferSize(2048);                          //设置输入缓冲区大小
	m_ctrlComm.put_OutBufferSize(2048);                       //设置输出缓冲区大小
	m_ctrlComm.put_Settings("115200,n,8,1");                  //设置波特率，数据格式
	if(!m_ctrlComm.get_PortOpen())            //打开串口
	{
		m_ctrlComm.put_PortOpen(TRUE);
		AfxMessageBox(_T("打开串口"));
	}
	else
		AfxMessageBox("cannot open serial port");
	m_ctrlComm.put_RThreshold(1);                            //可以不设置
	m_ctrlComm.put_SThreshold(1);
	m_ctrlComm.put_InputLen(0);                                //默认每次读完全部数据
	m_ctrlComm.get_Input();     















	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void DlgSend::OnBnClickedButton1()
{
	CString m_data;
	
	//m_data=app->m_putx+' '+app->m_puty+' '+app->m_putAG;
	m_ctrlComm.put_Output(COleVariant(m_data));
	AfxMessageBox(m_data);
	

	// TODO: 在此添加控件通知处理程序代码
}

void DlgSend::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	int num=1;
	num+=m_portnum.GetCurSel();
	m_port=num;

}
