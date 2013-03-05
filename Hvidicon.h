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
//����������࣬����singletonģʽ��ȫ��һ������
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
	int    pointType;		//ѡ����������� ���Ƕ������
	int   getHeight;        //ͼ����ʾ����С
	int   getWidth;
	int   cameraHeight;     //����ͷ��������
	int   cameraWidth;
	CvSize getSize;         //��ȡͼ���С 
	CvPoint getOrign;		//��ȡͼ����ʼλ��

	BOOL OverThrow;  //�Ƿ�ߵ���ʾͼ��
	
	BOOL CamSelect;//����ͷѡ��

	HHV		 m_hhv;
	BYTE	*m_pRawBuffer;			//�ɼ�ͼ��ԭʼ���ݻ�����////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	DWORD m_dwShotCount;  //ƽ��ÿ��ɼ��Ď���
	DWORD m_dwProcCount;//ƽ��ÿ�봦���֡��
	DWORD m_dwStartTime;//��ʼ�ɼ�ʱ��
	DWORD m_dwLogTime;
	DWORD m_imagesize;
	CvBox2D m_partsPos;
	float m_fpsProc;

	//��ɫ���ұ�
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
	BYTE	*m_pImageBuffer;		//Bayerת���󻺳���
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
	OPT_MODE    m_bOptMode;//�������أ�ѡ��ִ���Ǹ�����
	OPT_MODE    m_bRtrlMode;//����

	int         m_minArea;
public:
	void PutVedioData(){}//�ɼ���Ƶ���ݣ��ɻص���������߳���ʵ��
	
//��������
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

	void ReadImage();//�����߳��ڻ������ڶ�ȡͼ��
	void GetCaptureResolution(int& w,int& h){w=m_nMaxWidth;h=m_nMaxHeight;}
public:
//ʵ�ֶ��߳�
	boost::mutex  m_mutex;
	boost::mutex  m_ioMutex;
	boost::mutex  m_stMutex;  //δ�õ�
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
	IplImage*  m_pImgMatch;//���ڱ���ƥ����ʾ��ͼ��
	IplImage*  m_pMask;//����ָ���ȡ����������ͼ
	IplImage* m_pCamera;// �Դ������ͼ�����ݽṹ
	CvCapture* pCapture;//�Դ����������ͷ

	//IplImage*  m_pMaskRetrl;//�������ڼ���������ͼ��==ÿ5֡����ͼ�� ���� ���롱 ������õ����������м���
private:
	BOOL m_bThread;
	int  m_bLog;
	int  m_logInterval;
public:
	void StartProcThread();//���������߳�
	void Processing();//��ȡ��Ƶ���ݲ���������
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
	//ʵ���Զ��ع�ʱ���趨
	void AutoSetExposureTime();
	void CalcHistofExpTime(IplImage* pImgRead,double* pHist);//�����ض��ع�ʱ���²ɼ���ͼ���ֱ��ͼ
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
//���ڼ�������ǵ�Ĳ���
	double m_p1;
	double m_p2;
	
	int    m_bBoundaryFilter;
	
	

	
	
};

#define HVidicom	(CHvidicon::Instance())