// DialogSend.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CAMC.h"
#include "DialogSend.h"

//extern float intX;
//extern float intY;
//extern float intAG;
//extern float score;
//extern int ID;



float comp=0; //�ж������Ƿ��͸ı�




// DialogSend �Ի���

IMPLEMENT_DYNAMIC(DialogSend, CDialog)

DialogSend::DialogSend(CWnd* pParent /*=NULL*/)
	: CDialog(DialogSend::IDD, pParent)
	, m_port(1)
{

}

DialogSend::~DialogSend()
{
}

void DialogSend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbox);
}


BEGIN_MESSAGE_MAP(DialogSend, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &DialogSend::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &DialogSend::OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &DialogSend::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON3, &DialogSend::OnBnClickedButton3)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DialogSend ��Ϣ�������

void DialogSend::OnBnClickedButton1()
{
	//int multiple;
//	multiple=GetPrivateProfileInt("Retrieval Parameter", "multiple",1,"D:\work\�����������ġϵͳ������v1.0\camc\config.ini");
	CString m_x;
	CString m_data;
	m_data.Format("%s","T0");
	int ax;
	int ay;
	int aag	;
	int as;
	//ax=-(int)(100*intX);
	//ay=-(int)(100*intY);
	//ax=(int)(100*intY);
	//ax=2*ax;
	//ay=(int)(100*intX);
	//ay=2*ay;
	//aag=(int )(100*intAG);
	//if(score!=0){
	//	as=10000-(int)(100*score);
//	}
//	m_x.Format("%06d",ID);
	m_data+=m_x;


//	m_x.Format("%0.3f",intX);
	m_x.Format("%06d",ax);
	m_data+=m_x;
//	m_x.Format("%0.3f",intY);
	m_x.Format("%06d",ay);
	m_data+=m_x;
//	m_x.Format("%0.3f",intAG);
	m_x.Format("%06d",aag);
	m_data+=m_x;

	m_x.Format("%06d",as);
	m_data+=m_x;
	m_data+="\r";

	
	m_Comm.put_Output(COleVariant(m_data));
	
	
//	AfxMessageBox(_T("OK"));

AfxMessageBox(m_data);
/*	CString abc;
	abc.Format("%d",multiple);
	AfxMessageBox(abc);
*/







}

void DialogSend::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	CString m_num;
/*
	m_Comm.Create(NULL,0,CRect(0,0,0,0),this,IDC_MSCOMM1);

	m_Comm.put_CommPort(m_port);                                  //���ô��ں�
	m_Comm.put_InputMode(1);                                    //�ı����� ���
	m_Comm.put_InBufferSize(1024);                          //�������뻺������С
	m_Comm.put_OutBufferSize(1024);                       //���������������С
	m_Comm.put_Settings("115200,n,8,1");                  //���ò����ʣ����ݸ�ʽ
	if(!m_Comm.get_PortOpen())            //�򿪴���
	{
		m_Comm.put_PortOpen(TRUE);
		AfxMessageBox(_T("�򿪴���"));
	//	m_num.Format("%d",m_port);
	//	AfxMessageBox(m_num);

	}
	else
		AfxMessageBox(_T("���ڴ�Error"));

	//	AfxMessageBox("cannot open serial port");
	m_Comm.put_RThreshold(1);                            //���Բ�����
	m_Comm.put_SThreshold(1);
	m_Comm.put_InputLen(0);                                //Ĭ��ÿ�ζ���ȫ������
	m_Comm.get_Input();     

*/
}

void DialogSend::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_port=m_cbox.GetCurSel();
	//m_port++;
	//UpdateData(FALSE);




}

BOOL DialogSend::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//comp=score;
	//m_cbox.SetCurSel(0);
	char   temp[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,temp); 

	CString   strTmp; 
	CString   str,c   ; 
	c.Format( "%s",temp); 
	str=c+"\\config.ini ";
	m_port=GetPrivateProfileInt( "test ", "com",1,str);  
	
	//m_Comm.Create(NULL,0,CRect(0,0,0,0),pWnd,IDC_MSCOMM1);

	m_Comm.Create(NULL,0,CRect(0,0,0,0),this,IDC_MSCOMM1);

	m_Comm.put_CommPort(m_port);                                  //���ô��ں�
	m_Comm.put_InputMode(1);                                    //�ı����� ���
	m_Comm.put_InBufferSize(1024);                          //�������뻺������С
	m_Comm.put_OutBufferSize(1024);                       //���������������С
	m_Comm.put_Settings("115200,n,8,1");                  //���ò����ʣ����ݸ�ʽ
	if(!m_Comm.get_PortOpen())            //�򿪴���
	{
		m_Comm.put_PortOpen(TRUE);
		strTmp.Format("%d",m_port);
	//	AfxMessageBox(_T("�򿪴���:")+strTmp);

	}
	else
		AfxMessageBox(_T("���ڴ�Error"));

	//	AfxMessageBox("cannot open serial port");
	m_Comm.put_RThreshold(1);                            //���Բ�����
	m_Comm.put_SThreshold(1);
	m_Comm.put_InputLen(0);                                //Ĭ��ÿ�ζ���ȫ������
	m_Comm.get_Input();     


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void DialogSend::OnBnClickedButton3()
{
	SetTimer(1,400,NULL);

}


void DialogSend::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//    if ((nIDEvent==1)&&(comp!=score))
	//if(nIDEvent==1)
	{
	//	if (comp!=intX)
	//	{
			OnBnClickedButton1();
//			comp=score;
	//	}
		
	}

	CDialog::OnTimer(nIDEvent);
}
