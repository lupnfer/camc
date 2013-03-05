// CAMCDoc.h :  CCAMCDoc 类的接口
//


#pragma once
#include "ObjCanvas.h"
#include "FCLayerBar.h"
#include "ObjImage.h"
#include "ProgressBar.h"
#include "stetch.h"
#include "FreeImage.h"
#include "HVDAILT.h"
#include "Raw2Rgb.h"
#include "BgSub.h"
#include "CameraCalibration.h"
#include "KeyPointTemplate.h"
#include "SerialPort.h"

typedef struct tagVecelement{
	double num;
	int nindex;
}Vecelement;
typedef std::vector<Vecelement> VecelementList;
class CCAMCView;
class CCAMCDoc : public CDocument
{
protected: // 仅从序列化创建
	CCAMCDoc();
	DECLARE_DYNCREATE(CCAMCDoc)

// 属性
private:	
	FCObjCanvas			m_fcObjCanvas;//画布变量，用来管理图层
	FCLayerBar*			m_fcLayerBar;

	CKeyPointTemplate   m_kpTmp;

public:
	CProgressBar m_progressbar;
	BOOL         IsAuto;
	VecelementList m_vecpoint;

	IplImage*    m_pImageRead;
	

	int m_nBaud;       //波特率
	int m_nCom;         //串口号
	char m_cParity;    //校验
	int m_nDatabits;    //数据位
	int m_nStopbits;    //停止位
	DWORD m_dwCommEvents;
	

private:
	int          m_partId;
public:
	void Activate();
	void    InitLayerbar(){
		m_fcLayerBar->SetCanvas(&m_fcObjCanvas);
		m_fcLayerBar->InitImageList();
	}
	CCAMCView* GetView(int i=0);
// 操作
public:
	FCObjCanvas& GetCanvas(){return m_fcObjCanvas;}
	FCLayerBar*  GetLayerBar(){return m_fcLayerBar;}
	FCObjLayer*  GetCurrentLayer();
	FCObjLayer*  GetLayer(int nIndex=-1);

// 重写
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
	void InvalidateAllViews(CRect rc);

	void SetOriginPoint(POINT pt);
protected:

//******摄像机参数设置*************************************
private:

	CBgSub m_bgSub;
	
public:
	static void MyTimerProcs();
	
public:
	virtual ~CCAMCDoc();
//	static CCAMCDoc *_ins;
	static CCAMCDoc *instance();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnNewlayer();
	afx_msg void OnDellayer();
	afx_msg void OnMergesellayers();
	afx_msg void OnMergedownlayer();
	afx_msg void OnMergealllayers();
	afx_msg void OnUpdateDellayer(CCmdUI *pCmdUI);

	afx_msg void OnUpdateMergealllayers(CCmdUI *pCmdUI);
	afx_msg void OnImportimage();
	afx_msg void OnUpdateImportimage(CCmdUI *pCmdUI);
	afx_msg void OnLayerproperty();
	virtual void OnCloseDocument();
	afx_msg void OnUpdateMergedownlayer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMergesellayers(CCmdUI *pCmdUI);
	afx_msg void OnBrightadjust();
	afx_msg void OnUpdateBrightadjust(CCmdUI *pCmdUI);

	afx_msg void OnUpdateRotateCanvas(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRotateLayer(CCmdUI *pCmdUI);
	afx_msg void OnZoomin();
	afx_msg void OnUpdateZoomin(CCmdUI *pCmdUI);
	afx_msg void OnZoomout();
	afx_msg void OnUpdateZoomout(CCmdUI *pCmdUI);
	afx_msg void OnFileSave();
	
	
	
	afx_msg void OnColorsamping();
	afx_msg void OnObjectpick();
	afx_msg void OnFileSavevisible();
	
	afx_msg void OnFileSaveAs();
public:
	
	afx_msg void OnSnopshot();
	afx_msg void OnContinueshot();
	afx_msg void OnStopShot();

	afx_msg void OnContourdistill();
	afx_msg void OnContourOfBgsub();
	afx_msg void OnModelBegin();
	afx_msg void OnModelEnd();
	afx_msg void OnBeginCbModel();
	afx_msg void OnEndCbModel();
	afx_msg void OnFindCbContour();
	afx_msg void OnAngleDetection();
	afx_msg void OnUnitTest();
	afx_msg void OnAutoExpTime();
	afx_msg void OnQuery();
	afx_msg void OnInsertParts();
	afx_msg void OnInputPartsFeature();
	afx_msg void OnPartsSingleRetrieval();
	afx_msg void OnClearDatabase();
	afx_msg void OnAutoCalibration();
	afx_msg void OnUpdateDatabaseToFile();
	afx_msg void OnLoadTestImage();
	afx_msg void OnSaveMask();
	afx_msg void OnFindCorners();
	afx_msg void OnFindHomography();
	afx_msg void OnShowMeanerror();
	afx_msg void OnAdjustFeature();
	afx_msg void OnBeginkptrain();
	afx_msg void OnDokptrain();
	afx_msg void OnEndkptrain();
	afx_msg void OnCptRealSize();
	afx_msg void OnCapurePartMask();

	afx_msg void OnNewPart();
	afx_msg void OnSxfContour();
	afx_msg void OnImageDetection();
	afx_msg void On33069();
	afx_msg void OnFast();
	afx_msg void OnStartreg();
	afx_msg void OnNewsend();
};



#define CAMCDoc (CCAMCDoc::instance())



extern CCAMCDoc *_ins;
