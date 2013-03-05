#pragma once
#include "objbase.h"

#include "HVDAILT.h"
#include "Raw2Rgb.h"
#include "cxcore.h"
#include "BgSub.h"
#include "EdgeDetection.h"
#include "ShapeAnalysis.h"
#include "BlobResult.h"
#include "PartsDatabase.h"
#include "CameraCalibration.h"


typedef enum tagOPERA_MODE{ 
	DEFAULT_MODE 		= 0,
	CREATE_MEAN_MODEL	= 1,
	CREATE_CB_MODEL		= 2,
	MEAN_BG_DIFF		= 3,
	CB_BG_DIFF          = 4,
	END_MODEL_BG_CREATE = 5,
	END_MODEL_CB_CREATE = 6,
	CANNY_MODE          = 7,
	AUTO_SET_EXPTIME    = 8,
	HUMOMENTS_RETRIEVAL = 9,
	SXF_DIFF            = 10
	
} OPT_MODE;
//数字摄像机类，采用singleton模式，全局一个对象
class CCAMCDoc;
class FCObjImage;

class CHvidicon :
	public FCObject
{
private:
	CHvidicon(void);
public:
	~CHvidicon(void);
	static CHvidicon* _instance;


private:
	int    pointType;		//选择是像素输出 还是定标输出
	int   getHeight;        //图像显示区大小
	int   getWidth;
	int   cameraHeight;     //摄像头像素设置
	int   cameraWidth;
	CvSize getSize;         //截取图像大小 
	CvPoint getOrign;		//截取图像起始位置

	BOOL OverThrow;  //是否颠倒显示图像
	
	BOOL CamSelect;//摄像头选择

	HHV		 m_hhv;
	BYTE	*m_pRawBuffer;			//采集图像原始数据缓冲区////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	DWORD m_dwShotCount;  //平均每秒采集的幁数
	DWORD m_dwProcCount;//平均每秒处理的帧数
	DWORD m_dwStartTime;//开始采集时间
	DWORD m_dwLogTime;
	DWORD m_imagesize;
	CvBox2D m_partsPos;
	float m_fpsProc;

	//颜色查找表
	BYTE m_pLutR[256] ;
	BYTE m_pLutG[256] ;
	BYTE m_pLutB[256] ;	

	int				DeviceNum;
	HV_RESOLUTION	m_resolution;
	HV_SNAP_MODE	m_snapMode;
	HV_BAYER_LAYOUT m_layout;//GV400Ux(BAYER_BG)
	HV_BAYER_CONVERT_TYPE m_convertType;

	long	m_lGain[4];
	long	m_lTintUpper;
	long	m_lTintLower;
	long	m_shutterDelay;
	long	m_lADCLevel;
	int		m_nMaxWidth;
	int		m_nMaxHeight;

	BOOL	m_bSnapping;
	
	BOOL	m_bOpen;

	int		dwLastStatus; 
	DWORD		m_nLostNum;



	CRect       m_rcOutputWindow;
	int m_nHBlanking;
	int m_nVBlanking;
	HV_SNAP_SPEED m_snapSpeed;
public:
	BYTE	*m_pImageBuffer;		//Bayer转换后缓冲区
	int      m_colorBits;
	CString  m_configFile;
public:
	static CHvidicon* Instance();
	BOOL   BeginDevice();
	void   OutputOperationInfo();
public:
	HVSTATUS GetLastStatus();
	void AdjustHBlanking();
	void SetExposureTime(int nWindWidth,long lTintUpper,long lTintLower);
	void InitHVDevice();
	void InputConfig();
	void OnSnapError();
	void ShowImage();
	IplImage* OnSnopshot();
	void OnStopShot();
	virtual void OnStartShot(CCAMCDoc* pDoc) sealed;
	BOOL DecodeImage();
	static int CALLBACK SnapCallback(HV_SNAP_INFO *pInfo);
	int SnapCallback1();
	static void CALLBACK MyTimerProc();

	void ResetConfig();
public:
	OPT_MODE    m_bOptMode;//操作开关，选择执行那个操作
	OPT_MODE    m_bRtrlMode;//检索

	int         m_minArea;
public:
	void PutVedioData(){}//采集视频数据，由回调函数里的线程来实现
	
//轮廓处理
	void OnBeginCreateMeanModel();
	void CreateMeanModel(IplImage* pI);
	void OnEndCreateModel();
	void OnContourOfMeanBgsub();
	IplImage* ContourOfMeanBgsub(IplImage* pI);

	void OnBeginCreateCBModel();
	void CreateCbModel(IplImage* pI);
	IplImage* ContourOfCbBgsub(IplImage* pI);
	void OnContourOfCbBgsub();

	IplImage*  ContourOfCanny(IplImage* pI);
	void OnContourofCanny();

	void ReadImage();//处理线程在互斥区内读取图像
	void GetCaptureResolution(int& w,int& h){w=m_nMaxWidth;h=m_nMaxHeight;}
public:
//实现多线程
	boost::mutex  m_mutex;
	boost::mutex  m_ioMutex;
	boost::mutex  m_stMutex;  //未用到
	boost::condition m_cond;
private:
	CBgSub m_bgSub;
	CEdgeDetection	m_ed;
	CBlobResult     m_br;
	ResultSet		m_rs;
	Blob_vector     m_blobVec;

private:
	
	BOOL  m_full;
	IplImage*  m_pImgRead;
	IplImage*  m_pImgMatch;//用于保存匹配显示的图像
	IplImage*  m_pMask;//用于指向获取的轮廓线描图
	IplImage* m_pCamera;// 自带摄像机图像数据结构
	CvCapture* pCapture;//自带摄像机摄像头

	//IplImage*  m_pMaskRetrl;//保存用于检索的轮廓图像==每5帧轮廓图像 进行 “与” 操作后得到的轮廓进行检索
private:
	BOOL m_bThread;
	int  m_bLog;
	int  m_logInterval;
public:
	void StartProcThread();//启动处理线程
	void Processing();//获取视频数据并处理，单独
	void CloneImageToLayer(IplImage* pImgSrc,FCObjImage* pImgDst,int nlayer=1);
	void ShowRecogntionResult(IplImage* pImg);
	IplImage* CloneImageRead();
	RGBQUAD m_rgbF1;
	RGBQUAD m_rgbF2;
	RGBQUAD m_rgbB;
private:
	CCAMCDoc* m_pDoc; 
	CShapeAnalysis m_spAnalysis;
public:
	//实现自动曝光时间设定
	void AutoSetExposureTime();
	void CalcHistofExpTime(IplImage* pImgRead,double* pHist);//计算特定曝光时间下采集的图像的直方图
	void ShowHist(double* pHist,int nBins);
	void OnAutoSetExposureTime(CCAMCDoc* pDoc);

	void SetDoc(CCAMCDoc* pDoc){m_pDoc=pDoc;}
	
private:
	long m_expTime;
	double m_minVar;
	int  m_count[4];
	string m_curDir;

public:

	void CptBlobs(IplImage* pMask);

	void PartsRetrieval(IplImage* pMask,IplImage* pImg=NULL);
	void OnPartsRetrieval();

	void WriteLogFile();
public:
//用于检测轮廓角点的参数
	double m_p1;
	double m_p2;
	
	int    m_bBoundaryFilter;
	
	

	
	
};

#define HVidicom	(CHvidicon::Instance())