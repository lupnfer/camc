#include "StdAfx.h"
#include "Hvidicon.h"
#include "ObjImage.h"
#include "CAMCDoc.h"
#include "CAMCView.h"
#include <iostream>
#include <fstream>
#include "highgui.h"
#include "EdgeDetection.h"
#include "BgSub.h"
#include "fcolor.h"
#include "MainFrm.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/legacy/compat.hpp>
#include "PartsDatabase.h"
#include <ctime>
#include <boost/lexical_cast.hpp>  
//定义自己的0
#define  MY_ZERO 0.000000001
#define NUM	10

volatile float intX[NUM];
volatile float intY[NUM];
volatile float intAG[NUM];
volatile float score[NUM];
volatile int ID[NUM];


typedef boost::mutex::scoped_lock scoped_lock;

CHvidicon *CHvidicon::_instance=NULL;

CHvidicon::CHvidicon(void)
{
	//*****初始化相机参数****************
	DeviceNum = 1;
	m_resolution = RES_MODE0;
	m_snapMode = CONTINUATION;
	m_layout = BAYER_GR;//GV400Ux(BAYER_BG)
	m_convertType = BAYER2RGB_PIXELGROUPING;
	for(int i=0;i<4;++i){
		m_lGain[i]=8;
	}
	m_lTintLower = 60;
	m_lTintUpper = 1000;
	m_shutterDelay = 0;
	m_lADCLevel = ADC_LEVEL2;
	m_nMaxWidth = 800;
	m_nMaxHeight = 600;
	m_bSnapping=FALSE;
	m_bOpen=FALSE;
	for(int i=0;i<256;i++)
	{
		m_pLutR[i] = i;
		m_pLutG[i] = i;
		m_pLutB[i] = i;
	}
	m_nLostNum=0;
	m_dwShotCount=0;
	m_dwProcCount=0;
	m_dwStartTime=0;
	m_fpsProc=0.0;
	m_bOptMode=DEFAULT_MODE;
	m_bRtrlMode=DEFAULT_MODE;
	m_rcOutputWindow.SetRect(0,0,0,0);
	m_nHBlanking = 0;
	m_nVBlanking = 0;
	m_snapSpeed=HIGH_SPEED;

	
	m_colorBits=24;
	m_full=FALSE;
	m_pImgRead=NULL;
	m_pMask=NULL;
	m_rgbF1.rgbReserved=255;
	m_rgbF2.rgbReserved=255;
	m_rgbF2.rgbBlue=255;
	m_rgbF2.rgbGreen=m_rgbF2.rgbRed=0;
	m_rgbB.rgbReserved=0;
	m_rgbB.rgbBlue=m_rgbB.rgbGreen=m_rgbB.rgbRed=255;
	m_pDoc=NULL;
	m_bThread=FALSE;
	m_expTime=1;
	m_minVar=256;
	for(int i=0;i<4;++i){
		m_count[i]=0;
	}
	m_bgSub.LoadMeanModel();
	m_partsPos.angle=0;
	m_partsPos.center=cvPoint2D32f(0,0);

	TCHAR buffer[1024];
	GetCurrentDirectory(1024, buffer);
	m_configFile.Format("%s\\config.ini",buffer);
	m_curDir.append(buffer);
	m_curDir.append("\\");
	m_dwLogTime=0;
	m_bLog=0;

	InputConfig();
}
CHvidicon::~CHvidicon(void)
{
	scoped_lock lk(m_mutex);
	if(m_pImageBuffer)
		delete []m_pImageBuffer;
	if(m_pRawBuffer)
		delete []m_pRawBuffer;
	//	关闭数字摄像机，释放数字摄像机内部资源
	if(m_pImgRead)
		cvReleaseImage(&m_pImgRead);
	if(m_pMask)
		cvReleaseImage(&m_pMask);
	HVSTATUS status = STATUS_OK;
	status = EndHVDevice(m_hhv);
	HV_VERIFY(status);
}
CHvidicon* CHvidicon::Instance(){
	if(_instance==NULL){
		_instance=new CHvidicon();
	}
	return _instance;
}
BOOL CHvidicon::BeginDevice(){
	
	HVSTATUS status= BeginHVDevice(1, &m_hhv);
		//	检验函数执行状态，如果失败，则返回错误状态消息框
	HV_VERIFY(status);
	if(status==STATUS_OK){
		InitHVDevice();
		return TRUE;
	}
	return FALSE;
}
void CHvidicon::InitHVDevice()
{
	if(m_hhv == NULL) return;
	
	//get the max size of the output window
	int nBuffSize = 0;
	HVSTATUS status = HVGetDeviceInfo(m_hhv, DESC_RESOLUTION, NULL,&nBuffSize);	
	if( !HV_SUCCESS(status))
		return;

	BYTE *pbContext = new BYTE[nBuffSize];
	DWORD *pdContext = (DWORD *)(pbContext);
	status = HVGetDeviceInfo(m_hhv, DESC_RESOLUTION, pdContext,&nBuffSize);	
	m_nMaxWidth = *(pdContext + 2 * m_resolution);
	m_nMaxHeight = *(pdContext + 2* m_resolution + 1);

	int nMaxWidMd0 = *(pdContext + 2*RES_MODE0);
	int nMaxHeiMd0 = *(pdContext + 2*RES_MODE0 +1);
	delete []pbContext;
	HVSetResolution(m_hhv, m_resolution);
	HVSetSnapMode(m_hhv, m_snapMode);
	HVSetSnapSpeed(m_hhv, m_snapSpeed);
	HVSetBlanking(m_hhv,m_nHBlanking,m_nVBlanking);
	AdjustHBlanking();
	HVSetOutputWindow(m_hhv, 0, 0, m_nMaxWidth, m_nMaxHeight);

	for (int i = 0; i < 4; i++){
		HVAGCControl(m_hhv, RED_CHANNEL + i, m_lGain[i]);
	}

	HVADCControl(m_hhv, ADC_BITS, m_lADCLevel);


	//设置曝光时间
	SetExposureTime(m_nMaxWidth,m_lTintUpper,m_lTintLower);

	//初始化查找表
	//SetLutTable(m_dRatioR,m_dRatioG,m_dRatioB);	

	//allocate the memeor accord to the resolution fo RES_MODE0
	if(m_pImageBuffer)
		delete[] m_pImageBuffer;
	m_pImageBuffer = new BYTE[nMaxWidMd0 * nMaxHeiMd0 * 3];

	if (m_pImageBuffer) {
		FillMemory(m_pImageBuffer, nMaxWidMd0 * nMaxHeiMd0  * 3, 0xff);
	}
	if(m_pRawBuffer)
		delete[] m_pRawBuffer;
	m_pRawBuffer = new BYTE[nMaxWidMd0 * nMaxHeiMd0 ];
	if (m_pRawBuffer) {
		FillMemory(m_pRawBuffer, nMaxWidMd0 * nMaxHeiMd0 , 0xff);
	}
	m_rcOutputWindow.SetRect(0,0,m_nMaxWidth,m_nMaxHeight);
}
void CHvidicon::AdjustHBlanking()
{
	int size = sizeof(HVTYPE);
	HVTYPE type;
	HVGetDeviceInfo(m_hhv,DESC_DEVICE_TYPE, &type, &size);	
	if(type == GV400UMTYPE || type == GV400UCTYPE)
	{
		int rate = 0;
		switch(m_resolution)
		{
		case RES_MODE0:
			rate = 1;
			break;
		case RES_MODE1:
			rate = 2;
			break;
		default:
			break;
		}
		if (m_nHBlanking + m_rcOutputWindow.Width()*rate <= 0x236)
		{
			//取得消隐的边界值
			DWORD pBlankSize[4];
			int nBufSize = 0;
			int nHmin = 0;
			int nHmax = 0;
			HVGetDeviceInfo(m_hhv,DESC_DEVICE_BLANKSIZE,NULL,&nBufSize);	
			HVGetDeviceInfo(m_hhv,DESC_DEVICE_BLANKSIZE,pBlankSize,&nBufSize);	
			nHmin = (int)pBlankSize[0];
			nHmax = (int)pBlankSize[2];
			int tHor = 0x236-m_rcOutputWindow.Width()*rate+1;
			m_nHBlanking = max(nHmin,min(tHor, nHmax));
			HVSetBlanking(m_hhv,m_nHBlanking,m_nVBlanking);
		}			
	}
}
/*
函数:
		SetExposureTime
输入参数:
		int nWindWidth			当前图像宽度		
		int lTintUpper			曝光时间的分子，shutter的取值
		int lTintLower			曝光时间的分母，与shutter的单位相关
								（ms:1000；s:1）
输出参数:
		无						
说明:
		设置曝光时间（其他的参数如摄像机时钟频率，消隐值都取默认值）
*/
void CHvidicon::SetExposureTime(int nWindWidth,long lTintUpper,long lTintLower)
{
	int size = sizeof(HVTYPE);
	HVTYPE type;
	HVGetDeviceInfo(m_hhv,DESC_DEVICE_TYPE, &type, &size);	

	//请参考曝光系数转换公式
	int nOutputWid;
	long lClockFreq; 
	double dExposure = 0.0;
	double dTint = max((double)lTintUpper/(double)lTintLower,MY_ZERO);
	if (type == GV400UMTYPE || type == GV400UCTYPE)
	{
		long rate = 0;
		switch(m_resolution)
		{
		case RES_MODE0:
			rate = 1;
			break;
		case RES_MODE1:
			rate = 2;
			break;
		default:
			return;
		}
		nOutputWid = nWindWidth*rate;
		lClockFreq = (m_snapSpeed == HIGH_SPEED)? 26600000:13300000; 
		long lTb = m_nHBlanking;
		lTb += 0x5e;
		if(lTb <= 0) lTb =0;
		if (dTint* lClockFreq <= ((double)nOutputWid + lTb - 255))
		{
			dExposure = 1;
		}
		dExposure = (dTint* lClockFreq - ((double)nOutputWid + lTb - 255))/((double)nOutputWid + lTb);
		if (dExposure > 32767) 
			dExposure = 32767;
	}
	else
	{
		nOutputWid = nWindWidth;
		lClockFreq = (m_snapSpeed == HIGH_SPEED)? 24000000:12000000; 
		if(type == HV1300UCTYPE || type == HV1301UCTYPE)	
		{
			if(type == HV1300UCTYPE) 
				lClockFreq = 24000000;   //HV1300UCTYPE 暂不支持更改摄像机时钟频率
			long lTb = m_nHBlanking;
			lTb -= 10;	
			if(lTb <= 0) lTb =0;
			dExposure = (dTint* lClockFreq + 180.0)/((double)nOutputWid + 244.0 + lTb);
		}
		else
		{
			if(type == HV2000UCTYPE) 
				lClockFreq = 24000000;   //HV2000UCTYPE 暂不支持更改摄像机时钟频率	

			long lTb = m_nHBlanking;
			lTb += 36;
			if(lTb <= 0) lTb =0;
			dExposure = (dTint* lClockFreq + 180.0)/((double)nOutputWid + 305.0 + lTb) + 1;
		}

		if (dExposure > 16383) 
			dExposure = 16383;
	}
	HVAECControl(m_hhv, AEC_EXPOSURE_TIME, (long)dExposure);
}
void CHvidicon::InputConfig()
{
	
	m_snapMode =(HV_SNAP_MODE)(long)GetPrivateProfileInt("Video Parameter", "SnapMode", 0, m_configFile);
	
	m_lGain[0] =GetPrivateProfileInt("Video Parameter", "Red_Gain", 0, m_configFile);
	m_lGain[1] =GetPrivateProfileInt("Video Parameter", "Green1_Gain", 0, m_configFile);
	m_lGain[2] =GetPrivateProfileInt("Video Parameter", "Green2_Gain", 0, m_configFile);
	m_lGain[3] =GetPrivateProfileInt("Video Parameter", "Blue_Gain", 0, m_configFile);
	m_lTintUpper = GetPrivateProfileInt("Video Parameter", "ExposureTint_Upper", 0, m_configFile);
	m_lTintLower = GetPrivateProfileInt("Video Parameter", "ExposureTint_Lower", 0, m_configFile);
	m_shutterDelay = GetPrivateProfileInt("Video Parameter", "ShutterDelay", 0, m_configFile);
	m_lADCLevel =(HV_ADC_LEVEL) GetPrivateProfileInt("Video Parameter", "ADCLevel", 0, m_configFile);

	pointType=GetPrivateProfileInt("test", "pointType", 0, m_configFile);//定标输出和像素点输出的选择

	CHAR buffer[100];
	GetPrivateProfileString("Shadow Parameter", "H_Threshold","6",buffer,sizeof(buffer),m_configFile);
	float temp;
	sscanf(buffer,"%f",&temp);
	m_bgSub.m_hT=temp;
	GetPrivateProfileString("Shadow Parameter", "S_Threshold","7",buffer,sizeof(buffer),m_configFile);
	sscanf(buffer,"%f",&temp);
	m_bgSub.m_sT=temp;
	GetPrivateProfileString("Shadow Parameter", "V_Low","0.4",buffer,sizeof(buffer),m_configFile);
	sscanf(buffer,"%f",&temp);
	m_bgSub.m_vLow=temp;
	GetPrivateProfileString("Shadow Parameter", "V_High","0.6",buffer,sizeof(buffer),m_configFile);
	sscanf(buffer,"%f",&temp);
	m_bgSub.m_vHi=temp;
	
	

	m_ed.m_ctLineWidth=GetPrivateProfileInt("Contour Parameter", "ct_width",1,m_configFile);
	BYTE r,g,b;
	m_rgbF1.rgbRed=GetPrivateProfileInt("Contour Parameter", "ct_color_r",0,m_configFile);
	m_rgbF1.rgbGreen=GetPrivateProfileInt("Contour Parameter", "ct_color_g",0,m_configFile);
	m_rgbF1.rgbBlue=GetPrivateProfileInt("Contour Parameter", "ct_color_b",0,m_configFile);

	GetPrivateProfileString("Contour Parameter", "min_distance","7",buffer,sizeof(buffer),m_configFile);
	sscanf(buffer,"%f",&temp);
	m_p1=temp;
	GetPrivateProfileString("Contour Parameter", "max_distance","9",buffer,sizeof(buffer),m_configFile);
	sscanf(buffer,"%f",&temp);
	m_p2=temp;
	
	


	//m_ed.m_ctColor=CV_RGB(r,g,b);

	m_bgSub.m_rLow=GetPrivateProfileInt("Preprocess Parameter", "contrast_r_low", 0, m_configFile);
	m_bgSub.m_rHigh=GetPrivateProfileInt("Preprocess Parameter", "contrast_r_high", 255, m_configFile);


	m_bgSub.m_bLow=GetPrivateProfileInt("Preprocess Parameter", "contrast_b_low", 0, m_configFile);
	m_bgSub.m_bHigh=GetPrivateProfileInt("Preprocess Parameter", "contrast_b_high", 255, m_configFile);

	m_bgSub.m_gLow=GetPrivateProfileInt("Preprocess Parameter", "contrast_g_low", 0, m_configFile);
	m_bgSub.m_gHigh=GetPrivateProfileInt("Preprocess Parameter", "contrast_g_high", 255, m_configFile);
	m_bgSub.m_bContrast=GetPrivateProfileInt("Preprocess Parameter", "contrast_flag", 0, m_configFile);
	
	m_minArea=GetPrivateProfileInt("Retrieval Parameter", "min_area",500,m_configFile);
	m_bLog=GetPrivateProfileInt("Retrieval Parameter", "log_flag",0,m_configFile);
	m_logInterval=GetPrivateProfileInt("Retrieval Parameter", "log_interval",1,m_configFile);
	m_bBoundaryFilter=GetPrivateProfileInt("Retrieval Parameter", "Boundary_Filter",1,m_configFile);
	
	


	//multiple=GetPrivateProfileInt("Coeff","multiple",1,m_configFile);
}
void CHvidicon::ResetConfig()
{
	if(m_bSnapping){
		HVCloseSnap(m_hhv);
		InitHVDevice();
		//只定义一个缓冲区,
		HVOpenSnap(m_hhv, SnapCallback, this);
		BYTE * ppBuffer[1];
		ppBuffer[0] = m_pRawBuffer;
		HVStartSnap(m_hhv,ppBuffer,1);
	}
}
int CALLBACK CHvidicon::SnapCallback(HV_SNAP_INFO *pInfo)
{
	//CHvidicon *pHv= (CHvidicon *)(pInfo->pParam);
	//ASSERT(pHv);
	HVSTATUS status = _instance->GetLastStatus();	// 读取摄像机图像采集工作状态
	
	if(!HV_SUCCESS(status)){	//如果函数调用不成功，摄像机停止图像采集。
		 _instance->OnStopShot();
		 AfxMessageBox("摄像机安装不正确！停止采集");
		return 1;
	}
	
	if( _instance->dwLastStatus == 0)	// 当状态值等于0时，表示摄像机图像采集正常，则调用SendMessage显示图像；
	{	
		//ConvertBayer2Rgb(m_pImageBuffer,m_pRawBuffer,m_rcOutputWindow.Width(),m_rcOutputWindow.Height(),m_convertType,m_pLutR,m_pLutG,m_pLutB,true,m_layout);
		_instance->DecodeImage();
		_instance->OutputOperationInfo();
		_instance->m_dwStartTime=::GetTickCount();
	}
	else if(_instance->dwLastStatus == STATUS_FILE_INVALID) // 当状态值等于-18时，表示摄像机发生一次丢帧行为
	{
		_instance->m_nLostNum++;	//丢帧计数
	}
	else if (_instance->dwLastStatus==0xc0000012 || _instance->dwLastStatus==0xc0000011)
	{
		//pDoc->m_nErrorNum++;		//错误计数
	}
	else // 当状态值为其他非0值时，表示摄像机图像采集异常，则调用PostMessage进行处理；
	{
	   /*
	    *	发送自定义消息 WM_SNAP_ERROR 到主窗口，	
	    *	注意：用PostMessage发送消息，不必等待消息处理完，就可以返回。
	    */
		_instance->OnSnapError();
	}		
	return 1;	

}

HVSTATUS CHvidicon::GetLastStatus()
{
	HV_ARG_GET_LAST_STATUS ArgFeature;
	ArgFeature.type = HV_LAST_STATUS_TRANSFER;

	HV_RES_GET_LAST_STATUS ResFeature;

	HVAPI_CONTROL_PARAMETER  p;
	p.pInBuf		 = &ArgFeature;
	p.dwInBufSize	 = sizeof(ArgFeature);	
	p.pOutBuf		 = &ResFeature;
	p.dwOutBufSize	 = sizeof(ResFeature);
	p.pBytesRet		 = NULL;		
	p.code			 = ORD_GET_LAST_STATUS;
	int	dwSize = sizeof(p);

	HVSTATUS status = HVCommand(m_hhv, CMD_HVAPI_CONTROL, &p, &dwSize);
	if(status != STATUS_OK)
	{
		TRACE("HVCommand return status is %d", status);
	}

	dwLastStatus = 0;

	if (HV_SUCCESS(status)) {
		dwLastStatus = ResFeature.status;
	}

	return status;
}
void CHvidicon::OnStopShot()
{
	// TODO: 在此添加命令处理程序代码
	if (m_bSnapping){
		HVSTATUS status =HVCloseSnap(m_hhv);
		HV_VERIFY(status);
		if (HV_SUCCESS(status)) {
			m_bSnapping = FALSE;
			m_bOpen=FALSE;
			//OnSnapChange();
		}
	}
}
/*
	函数:
		OnSnapError
	输入参数:
		WPARAM wParam			没有使用		
		LPARAM lParam			异常状态码
	输出参数:
		LRESULT						
	说明:
		摄像机采集异常错误报告
*/
void CHvidicon::OnSnapError()
{	
	if (m_bSnapping){
		HVSTATUS status=HVCloseSnap(m_hhv);
		if(status!=STATUS_OK)
			AfxMessageBox("发生采集异常，OnSnapError处理异常中无法关闭视频采集！");
		m_bSnapping = FALSE;
		m_bOpen=FALSE;
	}
	if(m_pImageBuffer && m_pRawBuffer){
		HVSTATUS status=STATUS_OK;
		if(!m_bOpen)
			status= HVOpenSnap(m_hhv, SnapCallback, this);
		if (HV_SUCCESS(status)) {
			m_bOpen=TRUE;
			//只定义一个缓冲区,
			BYTE * ppBuffer[1];
			ppBuffer[0] = m_pRawBuffer;
			status = HVStartSnap(m_hhv,ppBuffer, 1);

			if ( ! HV_SUCCESS(status))
			{
				HVCloseSnap(m_hhv);
				m_bOpen=FALSE;
				m_bSnapping = FALSE;
			}
			else{
				m_bSnapping	= TRUE;			
			}
		}
		HV_MESSAGE(status);
	}
}


//
void CHvidicon::ShowImage()
{
	if(m_pDoc==NULL)
		return;
	CCAMCView* pView=m_pDoc->GetView();
	if(pView==NULL)
		return;
	if(m_pImgRead){
		FCObjLayer* pLayer0=m_pDoc->GetLayer(0);	
		if(pLayer0==NULL)
			return;
		IplImage* pImgShow=AutoStorage->CreateIplImage(pLayer0->Width(),pLayer0->Height(),m_pImgRead->depth,m_pImgRead->nChannels);
		cvResize(m_pImgRead,pImgShow);
		CamCalib->DrawWordCoordiateSystem(pImgShow);
		CloneImageToLayer(pImgShow,pLayer0);//显示采集的零件图像
		AutoStorage->ReleaseIplImage(pImgShow);
	}
	if(m_pMask){
		FCObjLayer* pLayer1=m_pDoc->GetLayer(1);
		if(pLayer1==NULL)
			return;
		IplImage* pImgProc=AutoStorage->CreateIplImage(pLayer1->Width(),pLayer1->Height(),m_pMask->depth,m_pMask->nChannels);	
		cvResize(m_pMask,pImgProc);
		
		CloneImageToLayer(pImgProc,pLayer1,1);//显示获取的轮廓图像
		AutoStorage->ReleaseIplImage(pImgProc);
		AutoStorage->ReleaseIplImage(m_pMask);
		m_pMask=NULL;
	}	
	pView->Draw();	
	//pView->Invalidate();
}
IplImage* CHvidicon::OnSnopshot()
{
	scoped_lock lk(m_mutex);
	IplImage* pImgRead=cvCloneImage(m_pImgRead);
	return pImgRead;
}

void CHvidicon::OutputOperationInfo()
{
	
	CString strTitle;
	float fpsCt=(1000.0)/(::GetTickCount()-m_dwStartTime+1);
	strTitle.Format("采集分辨率：[%d x %d] ，采集速度：%0.2lf(帧/秒)，帧数：%d(帧),处理速度：%0.2lf(帧/秒)，处理帧数：%d",
		m_rcOutputWindow.Width(),m_rcOutputWindow.Height(), fpsCt,m_dwShotCount,m_fpsProc,m_dwProcCount);

	m_dwStartTime=::GetTickCount();
	SetWindowText((AfxGetApp()->m_pMainWnd)->m_hWnd,strTitle);
}
void CHvidicon::OnStartShot(CCAMCDoc* pDoc){
	// TODO: 在此添加命令处理程序代码
	// TODO: Add your command handler code here
	if(pDoc)
		m_pDoc=pDoc;
	HVSTATUS status = STATUS_OK;
	if (m_bSnapping) {
		/*status = HVCloseSnap(m_hhv);
		m_bSnapping = FALSE;
		m_bOpen=FALSE;
		m_shotMode=2;*/
		return;
	}
	else{
		if(m_pImageBuffer && m_pRawBuffer){
			HVSTATUS status=STATUS_OK;
			if(!m_bOpen)
				status= HVOpenSnap(m_hhv, SnapCallback, this);
			if (HV_SUCCESS(status)) {
				m_bOpen=TRUE;
				//只定义一个缓冲区,
				BYTE * ppBuffer[1];
				ppBuffer[0] = m_pRawBuffer;
				status = HVStartSnap(m_hhv,ppBuffer, 1);

				if ( ! HV_SUCCESS(status))
				{
					HVCloseSnap(m_hhv);
					m_bOpen=FALSE;
					m_bSnapping = FALSE;
				}
				else{
					m_dwStartTime=::GetTickCount();
					m_bSnapping	= TRUE;
					if(!m_bThread){
						m_dwShotCount=0;
						StartProcThread();
						m_bThread=TRUE;
					}
				}
			}
			HV_MESSAGE(status);
		}
	}
}
BOOL CHvidicon::DecodeImage()
{
	CSize Size;
	Size.cx  = m_nMaxWidth;
	Size.cy =  m_nMaxHeight;
	//	//每两帧处理一次
	//if(m_convertType != BAYER2RGB_NEIGHBOUR)
	//{
	m_dwShotCount++;	
	{	
		scoped_lock lk(m_mutex);
		ConvertBayer2Rgb(m_pImageBuffer, m_pRawBuffer,Size.cx,Size.cy, m_convertType,m_pLutR,m_pLutG,m_pLutB,TRUE,m_layout);
		//m_rcOutputWindow.SetRect(0,0,m_nMaxWidth,m_nMaxHeight);
		if(!m_full){
			m_full=TRUE;
			m_cond.notify_one();
		}
	}
	return TRUE;
}
void CHvidicon::ReadImage()
{
	{
		scoped_lock lk(m_mutex);
		if(!m_full)
			m_cond.wait(m_mutex);
		if(m_pImgRead==NULL){
			m_pImgRead=cvCreateImage(cvSize(m_rcOutputWindow.Width(),m_rcOutputWindow.Height()),8,3);
			m_pImgRead->origin=1;
		}
		::CopyMemory(m_pImgRead->imageData,m_pImageBuffer,m_pImgRead->imageSize);
		m_full=FALSE;
	}
	if(m_bgSub.m_bContrast==1){
		m_bgSub.EqualizeContrast_3(m_pImgRead);
	}else if(m_bgSub.m_bContrast==2){
		IplImage* pImg=AutoStorage->CreateIplImage(m_pImgRead->width,m_pImgRead->height,m_pImgRead->depth,m_pImgRead->nChannels);
		m_bgSub.EqualizeHist_3(m_pImgRead,pImg);
		cvCopy(pImg,m_pImgRead);
		AutoStorage->ReleaseIplImage(pImg);
	}
}
void CHvidicon::OnBeginCreateMeanModel()
{
	//scoped_lock lk(m_stMutex);
	//ReadImage();
	IplImage* pI=m_bgSub.PrepareImage(m_pImgRead);
	m_bgSub.InitBGStatModel(pI);
	m_bOptMode=CREATE_MEAN_MODEL;
	
}
void CHvidicon::OnBeginCreateCBModel()
{
	m_bgSub.InitCodebook();
	m_bOptMode=CREATE_CB_MODEL;
}
void CHvidicon::OnEndCreateModel()
{
	if(m_bOptMode==CREATE_MEAN_MODEL){
		m_bOptMode=END_MODEL_BG_CREATE;
		//m_bgSub.CreateModelsfromStats();
	}else if(m_bOptMode==CREATE_CB_MODEL){
		m_bOptMode=END_MODEL_CB_CREATE;
		m_bgSub.EndCodeBookModelLearning();
	}
	else
		AfxMessageBox("请先开始背景模型的创建！");
	
}
void CHvidicon::OnContourOfMeanBgsub(){
	m_bOptMode=SXF_DIFF;
}

void CHvidicon::OnContourOfCbBgsub()
{
	m_dwProcCount=0;
	if(m_bOptMode==END_MODEL_CB_CREATE){
		m_bOptMode=CB_BG_DIFF;
	}else{
		AfxMessageBox("请先完成背景模型的创建！");
	}
}

void CHvidicon::CreateMeanModel(IplImage* pI)
{	
	m_bgSub.UpdateBGStatModel(pI);
}
IplImage* CHvidicon::ContourOfMeanBgsub(IplImage* pI)
{
	IplImage* pImask =m_bgSub.BackgroundDiffOfStatModel(pI);
	return pImask;
}
void CHvidicon::CreateCbModel(IplImage* pI)
{
	m_bgSub.LearningCodeBookModel(pI);
}
IplImage* CHvidicon::ContourOfCbBgsub(IplImage* pI)
{

	IplImage* pImask = m_bgSub.BackgroundDiffOfCodebook(pI);
	/*cvShowImage("asfs",pImask);
	waitKey(0);*/
	
	return pImask;
}
void CHvidicon::Processing()
{
	while(TRUE){
		Sleep(20);
		DWORD dwStartTime=::GetTickCount();
		ReadImage();
		IplImage* pI=m_bgSub.PrepareImage(m_pImgRead);
		m_pMask=NULL;
		switch(m_bOptMode){
			case CREATE_MEAN_MODEL:
				{
					CreateMeanModel(pI);
					break;
				}
			case MEAN_BG_DIFF:
				{
					m_pMask=ContourOfMeanBgsub(pI);
					break;
				}
			case CREATE_CB_MODEL:
				{
					CreateCbModel(pI);
					break;
				}
			case CB_BG_DIFF:
				{
					m_pMask=ContourOfCbBgsub(pI);
					break;
				}
			case CANNY_MODE:
				{
					m_pMask=ContourOfCanny(pI);
					break;
				}
			case SXF_DIFF:
				{
					m_pMask=m_bgSub.SxfBGDiffModel(pI);
					break;
				}
			default:
				break;
		}
		if(m_pMask){
			m_dwProcCount++;
			CptBlobs(m_pMask);
			if(m_bRtrlMode==HUMOMENTS_RETRIEVAL){
			//	cvSaveImage("D:\\picture\\m_pMask1.bmp",m_pMask);
			//	cvSaveImage("D:\\picture\\pI1.bmp",pI);
				PartsRetrieval(m_pMask,pI);
			}	
			m_fpsProc=1000.0/(::GetTickCount() - dwStartTime + 1);
		}	
		AutoStorage->ReleaseIplImage(pI);
		ShowImage();

	}
}


IplImage*  CHvidicon::ContourOfCanny(IplImage* pI)
{
	IplImage* pEdge=m_ed.EdgeDetectionByCanny(pI);
	return pEdge;
}
void CHvidicon::OnContourofCanny()
{
	m_dwProcCount=0;
	m_bOptMode=CANNY_MODE;
}
void CHvidicon::StartProcThread()
{
	boost::thread thrd(boost::bind(&CHvidicon::Processing,this));
}
void CHvidicon::CloneImageToLayer(IplImage* pImgSrc,FCObjImage* pImgDst,int nlayer)
{

	if(pImgSrc==NULL||pImgDst==NULL)
		return;
	scoped_lock lk(m_ioMutex);	
	BYTE* pLyrData=pImgDst->GetBits(pImgDst->Height()-1);
	BYTE* pImgData=(uchar*)(pImgSrc->imageData);
	long imgLen=pImgSrc->width*pImgSrc->height;
	
	if(pImgSrc->nChannels==1){
		for (int i=0;i<imgLen;++i)
		{	
			if((*pImgData)>=60){
				if(nlayer==1)
					FCColor::CopyPixel(pLyrData,&m_rgbF1,4);
				else if(nlayer==2)
					FCColor::CopyPixel(pLyrData,&m_rgbF2,4);

			}else{
				FCColor::CopyPixel(pLyrData,&m_rgbB,4);
			}
			pLyrData+=4;
			pImgData+=1;
		}
	}else if(pImgSrc->nChannels==3){
		for (int i=0;i<imgLen;++i)
		{	
			FCColor::CopyPixel(pLyrData,pImgData,3);
			pLyrData+=4;
			pImgData+=pImgSrc->nChannels;
		}
	}
}
void CHvidicon::CalcHistofExpTime(IplImage* pImgRead,double* pHist)
{	
	if(pHist==NULL||pImgRead==NULL)
		return;
	for(int i=0;i<256;++i)
		pHist[i]=0.0;
	IplImage* pGray=cvCreateImage(cvGetSize(pImgRead),8,1);
	cvCvtColor(pImgRead,pGray,CV_RGB2GRAY);	
	for(int y=0;y<pGray->height;++y){
		BYTE* pData=(BYTE*)(pGray->imageData+y*pGray->widthStep);
		for(int x=0;x<pGray->width;++x){
			int idx=*pData;
			pHist[idx]=pHist[idx]+1;
			pData++;
		}
	}
	double tmp=pGray->width*pGray->height;
	for(int i=0;i<256;++i)
		pHist[i]=pHist[i]/tmp;
}
void CHvidicon::ShowHist(double* pHist,int nBins)
{
	/** 获取直方图统计的最大值，用于动态显示直方图 */
	float max_value=0.0;
	for(int i=0;i<nBins;++i)
	{
		if(pHist[i]>max_value)
			max_value=pHist[i];
	}


	/** 设置直方图显示图像 */
	int height = 240;
	int width=nBins*4;
	IplImage* hist_img = cvCreateImage( cvSize(width,height), 8, 1);
	cvZero( hist_img );

	int bin_w = width / nBins;
	for(int v= 0; v< nBins; v++)
	{
		
		/** 获得直方图中的统计次数，计算显示在图像中的高度 */
		double bin_val = pHist[v];
		int intensity = cvRound(bin_val*height/max_value);

		CvScalar color=cvScalarAll(255);
		cvRectangle( hist_img, cvPoint(v*bin_w,height),
			cvPoint((v+1)*bin_w,height - intensity),
			color, -1, 8, 0 );
	}
	cvNamedWindow( "亮度直方图", 1 );
	cvShowImage( "亮度直方图", hist_img );
	cvWaitKey(0);
}
void CHvidicon::AutoSetExposureTime()
{
	float mean=1.0f/256.0f;
	double minVar=256;
	long   minExpTime=0;
	double pHist[256];
	FCObjLayer* pLayer=m_pDoc->GetLayer(0);
	CCAMCView* pView=m_pDoc->GetView();
	IplImage* pImgRead=NULL;
	IplImage* pImgShow=cvCreateImage(cvSize(pLayer->Width(),pLayer->Height()),8,3);
	for(int expTime=1;expTime<1025;expTime=expTime<<1){
		double var=0.0;
		SetExposureTime(m_rcOutputWindow.Width(),expTime,1000);
		{
			pImgRead=OnSnopshot();
			cvResize(pImgRead,pImgShow);
			scoped_lock lk(m_mutex);
			CloneImageToLayer(pImgShow,pLayer);//显示采集的零件图像
			pView->OnDraw(pView->GetDC());
		}
		CalcHistofExpTime(pImgRead,pHist);
		for(int j=0;j<256;++j){
			double tmp=pHist[j]-mean;
			var=var+tmp*tmp;
		}
		if(minVar>var){
			minVar=var;
			minExpTime=expTime;
		}
	}
	SetExposureTime(m_rcOutputWindow.Width(),minExpTime,1000);
	{
		pImgRead=OnSnopshot();
		cvResize(pImgRead,pImgShow);
		scoped_lock lk(m_mutex);
		CloneImageToLayer(pImgShow,pLayer);//显示采集的零件图像
		pView->OnDraw(pView->GetDC());
		OnStartShot(m_pDoc);
	}
}
void CHvidicon::OnAutoSetExposureTime(CCAMCDoc* pDoc)
{
	m_pDoc=pDoc;
	boost::thread thrd(boost::bind(&CHvidicon::AutoSetExposureTime,this));
}
void CHvidicon::PartsRetrieval(IplImage* pMask,IplImage* pImg){
	m_rs.clear();
	PartsDB->PartsRetrieval(pImg,m_br,m_rs,FS_HOG,pMask);
	ResultSet::iterator it=m_rs.begin();
	for(int i=0;i<m_br.GetNumBlobs()&&it!=m_rs.end();++i,++it){
		CBlob* pBlob=m_br.GetBlob(i);
		CvBox2D pos=pBlob->GetMinBoundingRect();
		CvPoint2D32f ptICS=cvPoint2D32f(pos.center.x,pos.center.y);
		if(CamCalib->IsValid())
		{ 
			//这是对输出坐标的选择 0的话就是定标法 1的话就是像素点法
			if(0==pointType)
			{
				CamCalib->CptWCSPoint(ptICS,it->position);
			}
			else
				if (1==pointType)
				{
					it->position.x=ptICS.x;
					if(OverThrow)
						it->position.y=(float)pMask->height-1-ptICS.y;
					else
						it->position.y=ptICS.y;

				}
		}
		it->angle=m_spAnalysis.GetRealAngle(pos)+it->angle;	
	}
	CMainFrame* pMf=(CMainFrame*)(AfxGetApp()->m_pMainWnd);
	pMf->m_wndWatchBar.ShowResultSet(m_rs);
	if(m_bLog)
		WriteLogFile();
	
}
void CHvidicon::OnPartsRetrieval(){
	if(m_bRtrlMode==HUMOMENTS_RETRIEVAL)
		m_bRtrlMode=DEFAULT_MODE;
	else m_bRtrlMode=HUMOMENTS_RETRIEVAL;
}
IplImage* CHvidicon::CloneImageRead()
{	
	if(m_pImgRead){
		scoped_lock lk(m_mutex);
		IplImage* pImgRead=cvCloneImage(m_pImgRead);
		return pImgRead;
	}
	return NULL;
}

/*
 *用于得到二值图像中Blob
 *
 */
void CHvidicon::CptBlobs(IplImage* pMask)
{
	
	
	m_br.BwLabeling(pMask,NULL,0);
	m_br.Filter(m_br,B_EXCLUDE,CBlobGetArea(), B_LESS,m_minArea);
	cvZero(pMask);
	for (int i = 0; i < m_br.GetNumBlobs(); i++ )
	{
		CBlob* pBlob=m_br.GetBlob(i);
		pBlob->FillBlob( pMask, CV_CVX_WHITE);
		//pBlob->DrawBlobContour(pMask, CV_CVX_WHITE);
		//pBlob->FindDominantPoints(m_p1,m_p2,m_p2,150);
		//pBlob->DrawDominantPoints(pMask, CV_CVX_WHITE);
		CvBox2D pos=pBlob->GetMinBoundingRect();
		m_spAnalysis.DrawMinBoundingRect(pos,pMask);

		//cvShowImage("saf",pMask);
		//cvWaitKey(0);
	}
}
void CHvidicon::ShowRecogntionResult(IplImage* pImg)
{
	if(pImg==NULL)
		return;
	IplImage* pShow=AutoStorage->CreateIplImage(pImg->width*2,pImg->height*2,8,1);
	cvResize(pImg,pShow);
	
	FCObjImage objImg;
	objImg.Load(pShow);	
	objImg.ConvertTo32Bit();
	FCObjLayer* pLayer=m_pDoc->GetLayer(2);
	int x=(pLayer->Width()-objImg.Width())/10;
	int y=(pLayer->Height()-objImg.Height())/10;
	{
		scoped_lock lk(m_ioMutex);	
		pLayer->CoverBlock(objImg,x,y);
	}
	m_pDoc->UpdateAllViews(NULL);
	AutoStorage->ReleaseIplImage(pShow);
}
void CHvidicon::WriteLogFile()
{
	static int idx=0;	
	if((::GetTickCount()-m_dwLogTime)<m_logInterval*1000){
		return;
	}
	CString dir;
	dir.Format("%soutput",m_curDir.c_str());
	::CreateDirectory(dir,NULL);
	string logName("output\\log-");
	using boost::lexical_cast;
	try{
		CTime T =CTime::GetCurrentTime();
		logName.append(boost::lexical_cast<string>(T.GetYear()));
		logName.append("-");
		logName.append(boost::lexical_cast<string>(T.GetMonth()));
		logName.append("-");
		logName.append(boost::lexical_cast<string>(T.GetDay()));
		logName.append("-");
		logName.append(boost::lexical_cast<string>(T.GetHour()));
		logName.append("-");
		logName.append(boost::lexical_cast<string>(T.GetMinute()));
		logName.append("-");
		logName.append(boost::lexical_cast<string>(T.GetSecond()));
		logName.append(".txt");
	}catch(boost::bad_lexical_cast& e)
	{
		return;
	}
//	FILE* file = fopen(logName.c_str(), "w" ); 
//	if(!file)
//		return;
//	time_t ltime;
//	time(&ltime);

//	fprintf(file,"%s\n",ctime(&ltime));
	
	ResultSet::iterator it=m_rs.begin();
	if (it!=m_rs.end())
	{
		*ID=it->id;                              //参数传递
		*intX=it->position.x;           
		*intY=it->position.y;
		*intAG=it->angle;
		*score=it->diff;
	}

	if(it==m_rs.end()){
		*ID=0;
		ID[1]=0;                             //参数传递
		intX[1]=0.0;        
		intY[1]=0.0;
		intAG[1]=0.0;
		score[1]=0.0;
	}else
	{
		int i;
		for(i=1;it!=m_rs.end();++it,i++)
		{
//		fprintf(file,"%d, %s, %0.2f,(%0.2f,%0.2f),%0.2f\n",it->id,it->name.c_str(),it->angle,it->position.x,it->position.y,it->diff);
		//提取最大的值
		
			ID[i]=it->id;
			intX[i]=it->position.x;
			intY[i]=it->position.y;
			intAG[i]=it->angle;
			score[i]=it->diff;
			if (i>=NUM)
				break;
		}
	}
//	fprintf(file,"------------------------------------------------\n");
	m_dwLogTime=::GetTickCount();
//	fclose(file);
	idx++;
}
	