// CAMCDoc.cpp :  CCAMCDoc 类的实现
//

#include "stdafx.h"
#include "CAMC.h"
#include "math.h"
#include "CAMCDoc.h"
#include ".\camcdoc.h"
#include "ObjImage.h"
#include "ObjCanvas.h"
#include "ObjLayer.h"
#include "MainFrm.h"
#include "FCSinglePixelProcessBase.h"
#include "LayerCommand.h"
#include "FCCmdSmartLayerBase.h"
#include "NewDlg.h"
#include "FCCmdLayerMerge.h"
#include "layerpropertydlg.h"
#include "DlgBrightnessAdjust.h"
#include "CmdImgBlur_Zoom.h"

#include "DlgCannyEdge.h"
#include "ProgressBar.h"
#include "ObjProgress.h"
#include "FCCmdCanvasRotate.h"
#include "CmdImgRemoveNoise.h"
#include "DlgRemoveNoise.h"
#include "oxo_helper.h"
#include <math.h>



#include "StdDefine.h"
#include "PaintBrush.h"
#include "ZoomIn.h"
#include "CAMCView.h"
#include ".\cmdremovestroke.h"
#include ".\zoomdlg.h"
#include "VBDefine.h"
#include "Shape.h"
#include "VirtualBrush.h"
#include "ColorDisplay.h"
#include "resource.h"
#include "cxcore.h"
#include "highgui.h"
#include "FColor.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "Hvidicon.h"
#include "EdgeDetection.h"
#include "PartsDatabase.h"
#include "ShapeAnalysis.h"
#include "DialogSend.h"
#include "ID_DIALOG_TEST.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NUM	10
extern volatile float intX[NUM];
extern volatile float intY[NUM];
extern volatile float intAG[NUM];
extern volatile float score[NUM];
extern volatile	int ID[NUM];


CSerialPort m_Port; 
float m_comp=0;   //判断数据是否更新


//CCAMCDoc *CCAMCDoc::_ins=NULL;
CCAMCDoc *_ins=NULL;


IMPLEMENT_DYNCREATE(CCAMCDoc, CDocument)

BEGIN_MESSAGE_MAP(CCAMCDoc, CDocument)

	ON_COMMAND(ID_NEWLAYER, OnNewlayer)
	//ON_COMMAND(ID_POPNEWLAYER, OnNewlayer)
	//ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_DELLAYER, OnDellayer)
	ON_COMMAND(ID_MERGESELLAYERS, OnMergesellayers)
	ON_COMMAND(ID_MERGEDOWNLAYER, OnMergedownlayer)
	ON_COMMAND(ID_MERGEALLLAYERS, OnMergealllayers)
	ON_UPDATE_COMMAND_UI(ID_DELLAYER, OnUpdateDellayer)

	ON_UPDATE_COMMAND_UI(ID_MERGEALLLAYERS, OnUpdateMergealllayers)
	ON_COMMAND(ID_IMPORTIMAGE, OnImportimage)
	ON_UPDATE_COMMAND_UI(ID_IMPORTIMAGE, OnUpdateImportimage)
	ON_COMMAND(ID_LAYERPROPERTY, OnLayerproperty)
	ON_UPDATE_COMMAND_UI(ID_MERGEDOWNLAYER, OnUpdateMergedownlayer)
	ON_UPDATE_COMMAND_UI(ID_MERGESELLAYERS, OnUpdateMergesellayers)
	ON_COMMAND(ID_BRIGHTADJUST, OnBrightadjust)
	ON_UPDATE_COMMAND_UI(ID_BRIGHTADJUST, OnUpdateBrightadjust)
	
	
	ON_COMMAND(ID_ZOOMIN, OnZoomin)
	ON_UPDATE_COMMAND_UI(ID_ZOOMIN, OnUpdateZoomin)
	ON_COMMAND(ID_ZOOMOUT, OnZoomout)
	ON_UPDATE_COMMAND_UI(ID_ZOOMOUT, OnUpdateZoomout)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	
	ON_COMMAND(ID_FILE_SAVEVISIBLE, OnFileSavevisible)
	
	
	
	
	ON_COMMAND(ID_FILE_SAVE_AS, &CCAMCDoc::OnFileSaveAs)
	ON_COMMAND(ID_SNOPSHOT, &CCAMCDoc::OnSnopshot)
	ON_COMMAND(ID_CONTINUESHOT, &CCAMCDoc::OnContinueshot)
	ON_COMMAND(ID_STOP_SHOT, &CCAMCDoc::OnStopShot)

	ON_COMMAND(ID_CONTOURDISTILL, &CCAMCDoc::OnContourdistill)

	ON_COMMAND(ID_CONTOUR_OF_BGSUB, &CCAMCDoc::OnContourOfBgsub)
	ON_COMMAND(ID_MODEL_BEGIN, &CCAMCDoc::OnModelBegin)
	ON_COMMAND(ID_MODEL_END, &CCAMCDoc::OnModelEnd)
	ON_COMMAND(ID_BEGIN_CB_MODEL, &CCAMCDoc::OnBeginCbModel)
	ON_COMMAND(ID_END_CB_MODEL, &CCAMCDoc::OnEndCbModel)
	ON_COMMAND(ID_FIND_CB_CONTOUR, &CCAMCDoc::OnFindCbContour)
	ON_COMMAND(ID_ANGLE_DETECTION, &CCAMCDoc::OnAngleDetection)
	ON_COMMAND(ID_UNIT_TEST, &CCAMCDoc::OnUnitTest)
	ON_COMMAND(ID_AUTO_EXP_TIME, &CCAMCDoc::OnAutoExpTime)
	ON_COMMAND(ID_QUERY, &CCAMCDoc::OnQuery)
	ON_COMMAND(ID_INSERT_PARTS, &CCAMCDoc::OnInsertParts)
	ON_COMMAND(ID_INPUT_PARTS_FEATURE, &CCAMCDoc::OnInputPartsFeature)
	ON_COMMAND(ID_PARTS_SINGLE_RETRIEVAL, &CCAMCDoc::OnPartsSingleRetrieval)
	ON_COMMAND(ID_CLEAR_DATABASE, &CCAMCDoc::OnClearDatabase)
	ON_COMMAND(ID_AUTO_CALIBRATION, &CCAMCDoc::OnAutoCalibration)
	ON_COMMAND(ID_UPDATE_DATABASE_TO_FILE, &CCAMCDoc::OnUpdateDatabaseToFile)
	ON_COMMAND(ID_LOAD_TEST_IMAGE, &CCAMCDoc::OnLoadTestImage)
	ON_COMMAND(ID_SAVE_MASK, &CCAMCDoc::OnSaveMask)
	ON_COMMAND(ID_FIND_CORNERS, &CCAMCDoc::OnFindCorners)
	ON_COMMAND(ID_FIND_HOMOGRAPHY, &CCAMCDoc::OnFindHomography)
	ON_COMMAND(ID_SHOW_MEANERROR, &CCAMCDoc::OnShowMeanerror)
	ON_COMMAND(ID_ADJUST_FEATURE, &CCAMCDoc::OnAdjustFeature)
	ON_COMMAND(ID_BEGINKPTRAIN, &CCAMCDoc::OnBeginkptrain)
	ON_COMMAND(ID_DOKPTRAIN, &CCAMCDoc::OnDokptrain)
	ON_COMMAND(ID_ENDKPTRAIN, &CCAMCDoc::OnEndkptrain)
	ON_COMMAND(ID_CPTREALSIZE, &CCAMCDoc::OnCptRealSize)
	ON_COMMAND(ID_CAPURE_PART_MASK, &CCAMCDoc::OnCapurePartMask)
	
	ON_COMMAND(ID_NEW_PART, &CCAMCDoc::OnNewPart)
	ON_COMMAND(ID_SXF_CONTOUR, &CCAMCDoc::OnSxfContour)
	ON_COMMAND(ID_IMAGEDETECTION, &CCAMCDoc::OnImageDetection)
	ON_COMMAND(ID_33069, &CCAMCDoc::On33069)
	ON_COMMAND(ID_FAST, &CCAMCDoc::OnFast)
	ON_COMMAND(ID_StartReg, &CCAMCDoc::OnStartreg)
	ON_COMMAND(ID_NewSend, &CCAMCDoc::OnNewsend)
END_MESSAGE_MAP()


// CCAMCDoc 构造/析构

CCAMCDoc::CCAMCDoc()
{
	// TODO: 在此添加一次性构造代码
	int w,h;
	HVidicom->GetCaptureResolution(w,h);
	SIZE          sizeCanvas = {w,h};
    m_fcObjCanvas.SetCanvasDimension (sizeCanvas);
	CMainFrame* frame=(CMainFrame*)AfxGetApp()->m_pMainWnd;
	m_fcLayerBar=&(frame->m_fcLayerBar);
	IsAuto = FALSE;
	m_bgSub.LoadMeanModel();
	m_pImageRead=NULL;
	m_kpTmp.CreateDetectorDescriptorMatcher();
	m_partId=0;

	HVidicom->SetDoc(this);
	
}

CCAMCDoc::~CCAMCDoc()
{	
	m_fcLayerBar=NULL;
}
//==============================================================================
BOOL CCAMCDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	CNewDlg newDlg;
	if(newDlg.DoModal()==IDOK){
		int dpi=newDlg.GetDPI();
		SIZE size;
		size.cx=newDlg.GetWidth();
		size.cy=newDlg.GetHigh();
		m_fcObjCanvas.SetCanvasDimension(size);
		m_fcObjCanvas.SetCanvasResolution(dpi,dpi);
		m_fcLayerBar->SetCanvas(&m_fcObjCanvas);
		OnNewlayer();
		if (size.cx>800||size.cy>600) {
			int zoomrate = FMax(((size.cx/800)),((size.cy/600)));	
			zoomrate=zoomrate*(-1);
			m_fcObjCanvas.SetZoomScale(zoomrate-1);
		}
	//	OnFast();
	}else
		return false;
	return true;
}
//打开一个新的画布
BOOL CCAMCDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	m_fcLayerBar->SetCanvas(&m_fcObjCanvas);
	OnNewlayer();
	FCObjLayer *pLayer = m_fcObjCanvas.GetCurrentLayer();
	if(pLayer==NULL)
		return false;
	if (!pLayer->Load (lpszPathName))
	{
		delete pLayer;
		return false;
	}
	pLayer ->ConvertTo32Bit();
//按照读入图像大小，重新设置画布大小
	SIZE size;
	size.cx=pLayer->Width();
	size.cy=pLayer->Height();
	m_fcObjCanvas.SetCanvasDimension(size);
	m_fcLayerBar->SetCanvas(&m_fcObjCanvas);
	m_fcLayerBar->InitImageList();
//缩放到合适大小
	/*if (size.cx>800||size.cy>600) {
		int zoomrate = FMax(((size.cx/800)),((size.cy/600)));	
		zoomrate=zoomrate*(-1);
		m_fcObjCanvas.SetZoomScale(zoomrate-1);
	}*/
	return true;
}
// CCAMCDoc 序列化

void CCAMCDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CCAMCDoc 诊断

#ifdef _DEBUG
void CCAMCDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCAMCDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCAMCDoc 命令

//==============================================================================
void CCAMCDoc::OnNewlayer()
{
	// TODO: 在此添加命令处理程序代码
	FCObjLayer* layer=new FCObjLayer();
	layer->Create(m_fcObjCanvas.GetCanvasDimension().cx,
					m_fcObjCanvas.GetCanvasDimension().cy,32);
	m_fcObjCanvas.ExecuteEffect (new FCCmdLayerAdd (layer, -1));
	m_fcLayerBar->Add(*layer);

}
//==============================================================================
void CCAMCDoc::OnDellayer()
{
	// TODO: Add your command handler code here
	int nLayerSelectNum = m_fcLayerBar->m_ListBox.GetCurSel();
	if(nLayerSelectNum == 0)
		{
			AfxMessageBox("背景层不能删除！",MB_OK);
			return ;
		}
	FCObjLayer* layer=m_fcObjCanvas.GetLayer(nLayerSelectNum);
	if(layer != NULL){
		m_fcObjCanvas.ExecuteEffect (new FCCmdLayerRemove (layer));
		m_fcLayerBar->InitImageList();
		this->UpdateAllViews(NULL);
		//m_fcLayerBar->Delete(nLayerSelectNum);
	}
	else
		return;
}
//==============================================================================
void CCAMCDoc::OnMergesellayers()
{
	// TODO: Add your command handler code here
	int nTotalNum = m_fcLayerBar->m_ListBox.GetCount();
	int nVisualLayerNum = 0;
	for(int i =0;i <nTotalNum; i++)
	{
		FCObjLayer *pLayer = m_fcObjCanvas.GetLayer(i);
		if(pLayer->GetLayerVisible()==TRUE)
		{
			nVisualLayerNum ++;
		}
}
	int *arIndex = new int [nVisualLayerNum];
	int j=0;
	for(int i = 0; i< nTotalNum; i++)
	{
		FCObjLayer *pLayer = m_fcObjCanvas.GetLayer(i);
		if(pLayer->GetLayerVisible()==TRUE)
		{
			arIndex[j] = i ;
			j++ ;
		}
      }
	m_fcObjCanvas.ExecuteEffect(new FCCmdLayerMerge(arIndex,j));
	for(int k = j-1;k > 0;k -- )
		{
			m_fcLayerBar->Delete(arIndex[k]);
		}
	delete[] arIndex;
	arIndex=NULL;
}
//==============================================================================
void CCAMCDoc::OnMergedownlayer()
{
	// TODO: Add your command handler code here
	int nSeletedNum = m_fcLayerBar->m_ListBox.GetCurSel();
	int nDownNum = nSeletedNum -1;
	int		* arIndex = new int[2] ;
	arIndex[0] = nSeletedNum;
	arIndex[1] = nDownNum;
	m_fcObjCanvas.ExecuteEffect(new FCCmdLayerMerge(arIndex , 2));
	int j;
	for(j = nSeletedNum; j > nDownNum ; j--)
	{
		m_fcLayerBar->Delete(j);
	}
	delete[] arIndex;
	arIndex=NULL;
}
//==============================================================================
void CCAMCDoc::OnMergealllayers()
{
	// TODO: Add your command handler code here
	int     nTotalLayerNum = m_fcObjCanvas.GetLayerNumber();
	int		* arIndex = new int[nTotalLayerNum] ;
	int     i,j ;
	for (i= 0 ; i <nTotalLayerNum ; i++)
		arIndex[i] = i ;
	m_fcObjCanvas.ExecuteEffect (new FCCmdLayerMerge (arIndex , nTotalLayerNum));
	for(j = nTotalLayerNum; j > 0 ; j--)
	{
		m_fcLayerBar->Delete(j);
	}
	delete[] arIndex;
	arIndex=NULL;
}
//==============================================================================
void CCAMCDoc::OnUpdateDellayer(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(m_fcObjCanvas.GetLayerNumber() <= 1)
		pCmdUI->Enable(FALSE);
}
//==============================================================================
void CCAMCDoc::OnUpdateMergealllayers(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(m_fcObjCanvas.GetLayerNumber() <= 1)
		pCmdUI->Enable(FALSE);
}
//==============================================================================
void CCAMCDoc::OnImportimage()
{
	// TODO: Add your command handler code here
	char szFilters[]=
		"位图(*.bmp)|所有文件 (*.*)|*.*||";
	CFileDialog fileDlg (TRUE, "edge", "*.bmp",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters,NULL);
	BOOL bResult=FALSE;
	if (fileDlg.DoModal() == IDOK)
	{
		CString path = fileDlg.GetPathName();
		CString fileName = fileDlg.GetFileName();
		int nLayerSelectNum = m_fcLayerBar->m_ListBox.GetCurSel();
		FCObjLayer* pLayer=m_fcObjCanvas.GetLayer(nLayerSelectNum);
		CPoint point = pLayer->GetGraphObjPos();
		if (!pLayer->Load (fileName))
		{
			delete pLayer;
			return ;
		}
		pLayer ->ConvertTo32Bit();
		pLayer->SetGraphObjPos(point);

		m_fcObjCanvas.ExecuteEffect( new FCCmdLayerAdd (pLayer, -1));		
		m_fcLayerBar->FlushLayerThumbnail(m_fcObjCanvas.GetCurrentLayerIndex());
		UpdateAllViews(NULL);
	}
}
//==============================================================================
void CCAMCDoc::OnUpdateImportimage(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(m_fcObjCanvas.GetLayerNumber() <= 1)
		pCmdUI->Enable(FALSE);
}
//==============================================================================
void CCAMCDoc::OnLayerproperty()
{
	// TODO: Add your command handler code here
	CLayerPropertyDlg dlg;
	dlg.DoModal();
}
//==============================================================================
void CCAMCDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	OnStopShot();
	m_fcLayerBar->SetCanvas(NULL);
	m_fcLayerBar->ClearImageList();
	CDocument::OnCloseDocument();
}
//==============================================================================
void CCAMCDoc::OnUpdateMergedownlayer(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(m_fcObjCanvas.GetLayerNumber() <= 1)
		pCmdUI->Enable(FALSE);

}
//==============================================================================
void CCAMCDoc::OnUpdateMergesellayers(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(m_fcObjCanvas.GetLayerNumber() <= 1)
		pCmdUI->Enable(FALSE);
}



void CCAMCDoc::OnBrightadjust()
{
	// TODO: Add your command handler code here
	CDlgBrightnessAdjust dlg;
	dlg.DoModal();
}

void CCAMCDoc::OnUpdateBrightadjust(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}



void CCAMCDoc::OnZoomin()
{
	m_fcObjCanvas.SetZoomIn();
	UpdateAllViews(NULL);
}

void CCAMCDoc::OnUpdateZoomin(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	
}

void CCAMCDoc::OnZoomout()
{
	// TODO: Add your command handler code here
	m_fcObjCanvas.SetZoomOut();
	UpdateAllViews(NULL);
}

void CCAMCDoc::OnUpdateZoomout(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void CCAMCDoc::OnFileSave()
{
	// TODO: Add your command handler code here
	//OnMergealllayers();
	boost::mutex::scoped_lock lock(CHvidicon::Instance()->m_ioMutex);
	//FCObjLayer *pLayer = m_fcObjCanvas.GetLayer(0);
	CFileDialog fileDlg(false,".bmp","*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bmp files (*.bmp)|所有文件(*.*)||");
	if(fileDlg.DoModal()==IDOK){
		FCObjImage imgSave;
		m_fcObjCanvas.MakeViewImage(0,0,m_fcObjCanvas.GetCanvasDimension(),FCObjImage::GetBGImage(),&imgSave);
		imgSave.ConvertTo24Bit();
		imgSave.Save(fileDlg.GetFileName());
		//pLayer->Save(fileDlg.GetFileName());
	}	
}


CCAMCDoc * CCAMCDoc::instance()
{
	if (_ins==NULL)
	{
		_ins=new CCAMCDoc;
	}
	return _ins;
}

void CCAMCDoc::Activate()
{
	m_fcLayerBar->SetCanvas(&m_fcObjCanvas);
	m_fcLayerBar->InitImageList();
}




void CCAMCDoc::OnFileSavevisible()
{
	// TODO: Add your command handler code here
	OnMergesellayers();
	FCObjLayer *pLayer = m_fcObjCanvas.GetCurrentLayer();
	CFileDialog fileDlg(false,".bmp","*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bmp files (*.bmp)|所有文件(*.*)||");
	if(fileDlg.DoModal()==IDOK){
		pLayer->Save(fileDlg.GetFileName());
	}

}



CCAMCView* CCAMCDoc::GetView(int i)
{
	int k=0;
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		if(k==i){
			return (CCAMCView*)pView;
		}
		k++;
	}   
}





void CCAMCDoc::OnFileSaveAs()
{
	
	OnFileSave();
}

void CCAMCDoc::OnSnopshot()
{

	CFileDialog fileDlg(false,".bmp","*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bmp files (*.bmp)|所有文件(*.*)||");
	if(fileDlg.DoModal()==IDOK){
		IplImage* pImgRead=CHvidicon::Instance()->OnSnopshot();
		if(pImgRead){

			cvSaveImage(fileDlg.GetFileName().GetBuffer(),pImgRead);	
		}
	}	
}

void CCAMCDoc::OnContinueshot()
{
	FCObjLayer* pLayer=GetLayer(0);           //取得画布一类的
	pLayer->SetAlphaChannelValue(0xFF);//设置alpha通道 和单像素处理过程  不是很明白
	CHvidicon::Instance()->OnStartShot(this);
}

void CCAMCDoc::OnStopShot()
{
	CHvidicon::Instance()->OnStopShot();
	//CHvidicon::Instance()->OnSnapChange(this);
}

void CCAMCDoc::OnContourdistill()
{
	GetLayer(1);	
	CHvidicon::Instance()->OnContourofCanny();
	//pLayer->Load(p)
}


//从背景剪除算法中获取轮廓
void CCAMCDoc::OnContourOfBgsub()
{
	// TODO: 在此添加命令处理程序代码
	CHvidicon::Instance()->OnContourOfMeanBgsub();
}

void CCAMCDoc::OnModelBegin()
{
	// TODO: 在此添加命令处理程序代码
	CHvidicon::Instance()->OnBeginCreateMeanModel();
}

void CCAMCDoc::OnModelEnd()
{
	// TODO: 在此添加命令处理程序代码
	CHvidicon::Instance()->OnEndCreateModel();
}

void CCAMCDoc::OnBeginCbModel()
{
	// TODO: 在此添加命令处理程序代码
	CHvidicon::Instance()->OnBeginCreateCBModel();
}

void CCAMCDoc::OnEndCbModel()
{
	// TODO: 在此添加命令处理程序代码
	CHvidicon::Instance()->OnEndCreateModel();
}

void CCAMCDoc::OnFindCbContour()
{
	// TODO: 在此添加命令处理程序代码
	CHvidicon::Instance()->OnContourOfCbBgsub();
}
//根据轮廓线检查零件的旋转角度
void CCAMCDoc::OnAngleDetection()
{
	// TODO: 在此添加命令处理程序代码
	
}

void CCAMCDoc::OnUnitTest()
{
	IplImage* pImgRead=CHvidicon::Instance()->CloneImageRead();	
	IplImage* image=NULL;
	if(pImgRead){
		image=m_bgSub.PrepareImage(pImgRead);
	}
	else{
		FCObjLayer layer(*GetCurrentLayer());
		layer.ConvertTo24Bit();
		image=layer.ToIplImage();
	}
	IplImage* pMask =m_bgSub.BackgroundDiffOfMean(image);
	CBlobResult br;
	br.BwLabeling(pMask,NULL,0);
	br.Filter(br,B_EXCLUDE,CBlobGetArea(), B_LESS,100);
	if(br.GetNumBlobs()==0)
		return;
	CBlob* pBlob=br.GetBlob(0);
	cvZero(pMask);
	pBlob->FillBlob(pMask,CV_RGB(255,255,255));
	
	CHAR buffer[1024];
	float p1,p2;
	GetPrivateProfileString("Contour Parameter", "min_distance","7",buffer,sizeof(buffer),HVidicom->m_configFile);
	sscanf(buffer,"%f",&p1);
	GetPrivateProfileString("Contour Parameter", "max_distance","9",buffer,sizeof(buffer),HVidicom->m_configFile);
	sscanf(buffer,"%f",&p2);

	pBlob->FindDominantPoints(p1,p2,p2,150);
	pBlob->DrawDominantPoints(image, CV_RGB(255,0,0));

	

	CvBox2D box=pBlob->GetMinBoundingRect();
	CvPoint2D32f ptICS[4];
	cvBoxPoints(box,ptICS);
	CvPoint2D32f pt2d[4];
	for(int i=0;i<4;++i){
		CamCalib->CptWCSPoint(ptICS[i],pt2d[i]);
	}
	double wTmp=sqrt((double)((pt2d[0].x-pt2d[1].x)*(pt2d[0].x-pt2d[1].x)+(pt2d[0].y-pt2d[1].y)*(pt2d[0].y-pt2d[1].y)));
	double hTmp=sqrt((double)((pt2d[1].x-pt2d[2].x)*(pt2d[1].x-pt2d[2].x)+(pt2d[1].y-pt2d[2].y)*(pt2d[1].y-pt2d[2].y)));
	double w=FMax<double>(wTmp,hTmp);
	double h=FMin<double>(wTmp,hTmp);
	CString strFormat;
	strFormat.Format("该零件的宽：%0.2f，高：%0.2f",w,h);
	AfxMessageBox(strFormat);

	PartsDB->PartsRecognition(pBlob,image);
	cvZero(pMask);
	Pt32fVec dpList;
	//blob.FindDominantPoints(HVidicom->m_p1,HVidicom->m_p2,HVidicom->m_p2);
	CamCalib->ICSpts2WCSpts(pBlob->GetDominantPoints(),dpList);	
	Pt32fVec::iterator it=dpList.begin();
	
	CvPoint pt1={((it->x)+200)*2,((it->y)+200)*2};
	cvDrawCircle(pMask,pt1,3, CV_RGB(255,255,255),CV_FILLED);
	for(++it;it!=dpList.end();++it){
		CvPoint pt2={((it->x)+200)*2,((it->y)+200)*2};
		cvDrawCircle(pMask,pt2,3, CV_RGB(255,255,255),CV_FILLED);
		cvDrawLine(pMask,pt1,pt2,CV_RGB(255,255,255));
		pt1=pt2;
	}
	cvShowImage("asfda",pMask);
	cvWaitKey(0);

	MV_DEL_IMG(pImgRead);
	MV_DEL_IMG(image);
	MV_DEL_IMG(pMask);
}
FCObjLayer* CCAMCDoc::GetCurrentLayer()
{	
	return m_fcObjCanvas.GetCurrentLayer ();
}
FCObjLayer*  CCAMCDoc::GetLayer(int nIndex)
{
	if(nIndex<0)
		return m_fcObjCanvas.GetCurrentLayer ();
	FCObjLayer* pLayer=m_fcObjCanvas.GetLayer(nIndex);	
	while(pLayer==NULL){
		this->OnNewlayer();
		pLayer=m_fcObjCanvas.GetLayer(nIndex);	
	}
	return pLayer;
}

void CCAMCDoc::OnAutoExpTime()
{
	// TODO: 在此添加命令处理程序代码
	CHvidicon::Instance()->OnAutoSetExposureTime(this);
}



void CCAMCDoc::OnQuery()
{
	CHvidicon::Instance()->OnPartsRetrieval();          //零件检测
}

void CCAMCDoc::OnInsertParts()
{
	// TODO: 在此添加命令处理程序代码
	CPartsDatabase::Instance()->NewParts();
//	AfxMessageBox("插入了一个新零件，请点击“输入零件特征”菜单，录入当前零件各种摆放姿势下的特征！");
}

void CCAMCDoc::OnInputPartsFeature()          //插入新零件特征
{
	// TODO: 在此添加命令处理程序代码
	PartsInfoList ptInfoListOut;
	PartsDB->Insert(ptInfoListOut);
	AfxMessageBox("形状特征入库成功！");
}

void CCAMCDoc::OnPartsSingleRetrieval()
{
	//// TODO: 在此添加命令处理程序代码
	IplImage* pImgRead=CHvidicon::Instance()->CloneImageRead();	
	IplImage* pI=m_bgSub.PrepareImage(pImgRead);
	IplImage* pImask =m_bgSub.BackgroundDiffOfMean(pI);
	
	PartsInfo spInfo;
	int id=CPartsDatabase::Instance()->ShapeRetrivalByHOGFeature(pI,pImask,spInfo);
	if(id<0)
		return;
	AutoStorage->ReleaseIplImage(pI);
	//AutoStorage->ReleaseIplImage(pImask);
	cvReleaseImage(&pImgRead);

	cvZero(pImask);
	CPartsDatabase::Instance()->GetIplImage(spInfo,pImask);
	//cvShowImage("asd",pImask);
	FCObjLayer* pLayer=GetLayer(2);
	pLayer->SetAlphaChannelValue(0xFF);
	CvSize sz={pLayer->Width(),pLayer->Height()};
	IplImage* pImgDst=cvCreateImage(sz,pImask->depth,pImask->nChannels);
	cvResize(pImask,pImgDst,CV_INTER_AREA);
	CHvidicon::Instance()->CloneImageToLayer(pImgDst,pLayer);
	AutoStorage->ReleaseIplImage(pImask);
	cvReleaseImage(&pImgDst);
	UpdateAllViews(NULL);
}

void CCAMCDoc::OnClearDatabase()
{
	// TODO: 在此添加命令处理程序代码
	CPartsDatabase::Instance()->ClearDataBase();
	CPartsDatabase::Instance()->Save();
	AfxMessageBox("数据库清空成功！");
}

void CCAMCDoc::OnUpdateDatabaseToFile()
{
	// TODO: 在此添加命令处理程序代码
	CPartsDatabase::Instance()->Save();
}

void CCAMCDoc::OnLoadTestImage()
{
	// TODO: 在此添加命令处理程序代码
	char szFilters[]=
		"位图(*.bmp)|所有文件 (*.*)|*.*||";
	CFileDialog fileDlg (TRUE, "edge", "*.bmp",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters,NULL);
	BOOL bResult=FALSE;
	if (fileDlg.DoModal() == IDOK)
	{
		CString path = fileDlg.GetPathName();
		if(m_pImageRead)
			cvReleaseImage(&m_pImageRead);
		m_pImageRead=cvLoadImage(path);

		FCObjLayer* pLayer=GetLayer(0);
		pLayer->SetAlphaChannelValue(0xFF);
		CvSize sz={pLayer->Width(),pLayer->Height()};
		IplImage* pImgDst=cvCreateImage(sz,m_pImageRead->depth,m_pImageRead->nChannels);
		cvResize(m_pImageRead,pImgDst,CV_INTER_AREA);
		CHvidicon::Instance()->CloneImageToLayer(pImgDst,pLayer);
		cvReleaseImage(&pImgDst);
		UpdateAllViews(NULL);
	}
}

void CCAMCDoc::OnSaveMask()
{
	// TODO: 在此添加命令处理程序代码
	IplImage* pImgRead=CHvidicon::Instance()->CloneImageRead();	
	IplImage* pI=m_bgSub.PrepareImage(pImgRead);
	IplImage* pImask =m_bgSub.BackgroundDiffOfMean(pI);

	cvSaveImage("D:\\mask.bmp",pImask);
	AutoStorage->ReleaseIplImage(pI);
	//AutoStorage->ReleaseIplImage(pImask);
	cvReleaseImage(&pImgRead);
	AutoStorage->ReleaseIplImage(pImask);
	UpdateAllViews(NULL);
}

void CCAMCDoc::OnAutoCalibration()
{
	// TODO: 在此添加命令处理程序代码


}

void CCAMCDoc::OnFindCorners()
{
	// TODO: 在此添加命令处理程序代码
	
	/*FCObjLayer layer(*GetLayer(0));
	if(layer.IsValidImage()==FALSE)
		return;
	layer.ConvertTo24Bit();*/
	IplImage* pImgRead=CHvidicon::Instance()->CloneImageRead();	
	IplImage* image=NULL;
	if(pImgRead){
		image=m_bgSub.PrepareImage(pImgRead);
	}
	else{
		FCObjLayer layer(*GetCurrentLayer());
		layer.ConvertTo24Bit();
		image=layer.ToIplImage();
	}
	CvSize board_sz;
	board_sz.width=GetPrivateProfileInt("Calibration Parameter", "board_width", 0, HVidicom->m_configFile);
	board_sz.height=GetPrivateProfileInt("Calibration Parameter", "board_height", 0, HVidicom->m_configFile);
	int board_n=board_sz.width*board_sz.height;	
	CamCalib->FindChessBoardCorners(image,board_sz,CV_CALIB_CB_ADAPTIVE_THRESH|CV_CALIB_CB_FILTER_QUADS,board_n);
	FCObjLayer layer;
	layer.Load(image);
	layer.ConvertTo32Bit();
	OnNewlayer();
	boost::mutex::scoped_lock lock(CHvidicon::Instance()->m_ioMutex);
	FCObjLayer* pLayer=GetCurrentLayer();
	*pLayer=layer;
	AutoStorage->ReleaseIplImage(image);
	AutoStorage->ReleaseIplImage(pImgRead);
	UpdateAllViews(NULL);
}

void CCAMCDoc::OnFindHomography()
{
	// TODO: 在此添加命令处理程序代码
	CHAR buffer[100];
	GetPrivateProfileString("Calibration Parameter", "unit_width","6",buffer,sizeof(buffer),HVidicom->m_configFile);
	float w;
	sscanf(buffer,"%f",&w);
	GetPrivateProfileString("Calibration Parameter", "unit_height","6",buffer,sizeof(buffer),HVidicom->m_configFile);
	float h;
	sscanf(buffer,"%f",&h);
	CamCalib->SetUnitWH(w,h);
	CamCalib->Findhomography();
}
void CCAMCDoc::SetOriginPoint(POINT pt)
{
	FCObjLayer* pLayer=m_fcObjCanvas.GetCurrentLayer();
	if(!pLayer)
		return;
	pLayer->ConvertTo24Bit();
	IplImage* image=pLayer->ToIplImage();
	pt.y=image->height-1-pt.y;
	CamCalib->SetOriginPoint(pt);
	CvPoint pt2D={CamCalib->GetOrigin().x,CamCalib->GetOrigin().y};
	cvCircle(image,pt2D,5,CV_RGB(255,0,0),CV_FILLED);
	pLayer->Load(image);
	pLayer->ConvertTo32Bit();
	pLayer->SetAlphaChannelValue(0xFF);
	OnFindHomography();
	cvReleaseImage(&image);
	UpdateAllViews(NULL);
}

void CCAMCDoc::OnShowMeanerror()
{
	// TODO: 在此添加命令处理程序代码

	std::vector<WCSPoint> errorArray;
	double maxDiff=CamCalib->UnitTest(errorArray);

}

void CCAMCDoc::OnAdjustFeature()
{
	// TODO: 在此添加命令处理程序代码
	
}

void CCAMCDoc::OnBeginkptrain()
{
	m_kpTmp.BeforeTrain();
}

void CCAMCDoc::OnDokptrain()
{
	m_kpTmp.DoTrain();
}

void CCAMCDoc::OnEndkptrain()
{
	
}
void CCAMCDoc::OnCptRealSize()
{
	IplImage* pImgRead=CHvidicon::Instance()->CloneImageRead();	
	IplImage* image=NULL;
	if(pImgRead){
		image=m_bgSub.PrepareImage(pImgRead);
	}
	else{
		FCObjLayer layer(*GetCurrentLayer());
		layer.ConvertTo24Bit();
		image=layer.ToIplImage();
	}
	IplImage* pMask =m_bgSub.BackgroundDiffOfMean(image);



	CBlobResult br;
	br.BwLabeling(pMask,NULL,0);
	br.Filter(br,B_EXCLUDE,CBlobGetArea(), B_LESS,6000);
	if(br.GetNumBlobs()==0)
		return;
	CBlob* pBlob=br.GetBlob(0);
	cvZero(pMask);
	pBlob->FillBlob(pMask,CV_RGB(255,255,255));

	CHAR buffer[1024];
	float p1,p2;
	GetPrivateProfileString("Contour Parameter", "min_distance","7",buffer,sizeof(buffer),HVidicom->m_configFile);
	sscanf(buffer,"%f",&p1);
	GetPrivateProfileString("Contour Parameter", "max_distance","9",buffer,sizeof(buffer),HVidicom->m_configFile);
	sscanf(buffer,"%f",&p2);

	pBlob->FindDominantPoints(p1,p2,p2,150);
	pBlob->DrawDominantPoints(image, CV_RGB(255,0,0));



	CvBox2D box=pBlob->GetMinBoundingRect();
	CvPoint2D32f ptICS[4];
	cvBoxPoints(box,ptICS);
	CvPoint2D32f pt2d[4];
	for(int i=0;i<4;++i){
		CamCalib->CptWCSPoint(ptICS[i],pt2d[i]);
	}
	double wTmp=sqrt((double)((pt2d[0].x-pt2d[1].x)*(pt2d[0].x-pt2d[1].x)+(pt2d[0].y-pt2d[1].y)*(pt2d[0].y-pt2d[1].y)));
	double hTmp=sqrt((double)((pt2d[1].x-pt2d[2].x)*(pt2d[1].x-pt2d[2].x)+(pt2d[1].y-pt2d[2].y)*(pt2d[1].y-pt2d[2].y)));
	double w=FMax<double>(wTmp,hTmp);
	double h=FMin<double>(wTmp,hTmp);
	CString strFormat;
	strFormat.Format("该零件的宽：%0.2f，高：%0.2f",w,h);
	AfxMessageBox(strFormat);

	PartsDB->PartsRecognition(pBlob,image);
	cvZero(pMask);
	Pt32fVec dpList;
	//blob.FindDominantPoints(HVidicom->m_p1,HVidicom->m_p2,HVidicom->m_p2);
	CamCalib->ICSpts2WCSpts(pBlob->GetDominantPoints(),dpList);	
	Pt32fVec::iterator it=dpList.begin();

	CvPoint pt1={((it->x)+200)*2,((it->y)+200)*2};
	cvDrawCircle(pMask,pt1,3, CV_RGB(255,255,255),CV_FILLED);
	for(++it;it!=dpList.end();++it){
		CvPoint pt2={((it->x)+200)*2,((it->y)+200)*2};
		cvDrawCircle(pMask,pt2,3, CV_RGB(255,255,255),CV_FILLED);
		cvDrawLine(pMask,pt1,pt2,CV_RGB(255,255,255));
		pt1=pt2;
	}
	cvShowImage("角点轮廓图",pMask);
	cvWaitKey(0);

	MV_DEL_IMG(pImgRead);
	MV_DEL_IMG(image);
	MV_DEL_IMG(pMask);

}

void CCAMCDoc::OnCapurePartMask()
{
	// TODO: 在此添加命令处理程序代码
	IplImage* pImgRead=HVidicom->CloneImageRead();	

	if(!pImgRead)
		return;
	IplImage* pI=m_bgSub.PrepareImage(pImgRead);
	IplImage* pMask =m_bgSub.BackgroundDiffOfMean(pI);
	CBlobResult br;
	br.BwLabeling(pMask,NULL,0);
	br.Filter(br,B_EXCLUDE,CBlobGetArea(), B_LESS,1000);
	if(br.GetNumBlobs()==0)
		return;
	CBlob* pBlob=br.GetBlob(0);
	cvZero(pMask);
	pBlob->FillBlob(pMask,CV_RGB(255,255,255));

	static int idx=0;
	
	std::stringstream strMask,strImg;
	strMask<<PartsDB->GetCurrentPath()<<"训练样本图像\\p-m-"<<m_partId<<"-"<<idx<<".bmp";
	strImg<<PartsDB->GetCurrentPath()<<"训练样本图像\\p-i-"<<m_partId<<"-"<<idx<<".bmp";
	cvSaveImage(strMask.str().c_str(),pMask);
	cvSaveImage(strImg.str().c_str(),pI);
	AutoStorage->ReleaseIplImage(pI);
	AutoStorage->ReleaseIplImage(pMask);
	AutoStorage->ReleaseIplImage(pImgRead);
	AfxMessageBox("采集MASK成功！请关闭背投灯获取零件图像！");

	pImgRead=HVidicom->CloneImageRead();	
	if(!pImgRead)
		return;
	pI=m_bgSub.PrepareImage(pImgRead);
	std::stringstream strImgImg;
	strImgImg<<PartsDB->GetCurrentPath()<<"训练样本图像\\p-ii-"<<m_partId<<"-"<<idx<<".bmp";
	cvSaveImage(strImgImg.str().c_str(),pI);
	AutoStorage->ReleaseIplImage(pI);
	AutoStorage->ReleaseIplImage(pMask);
	AutoStorage->ReleaseIplImage(pImgRead);
	++idx;
}


void CCAMCDoc::OnNewPart()
{
	// TODO: 在此添加命令处理程序代码
	m_partId++;

	IplImage* pImgRead=CHvidicon::Instance()->CloneImageRead();	
	IplImage* image=NULL;
	if(pImgRead){
		image=m_bgSub.PrepareImage(pImgRead);
	}
	else{
		FCObjLayer layer(*GetCurrentLayer());
		layer.ConvertTo24Bit();
		image=layer.ToIplImage();
	}
	IplImage* pMask =m_bgSub.BackgroundDiffOfMean(image);



	CBlobResult br;
	br.BwLabeling(pMask,NULL,0);
	br.Filter(br,B_EXCLUDE,CBlobGetArea(), B_LESS,6000);
	if(br.GetNumBlobs()==0)
		return;
	CBlob* pBlob=br.GetBlob(0);
	cvZero(pMask);
	pBlob->FillBlob(pMask,CV_RGB(255,255,255));
	PartsInfo ptInfo;
	PartsDB->CptKeyPointsDescriptors(image,pBlob,ptInfo);

	MV_DEL_IMG(pImgRead);
	MV_DEL_IMG(image);
	MV_DEL_IMG(pMask);
}

void CCAMCDoc::OnSxfContour()
{
	// TODO: 在此添加命令处理程序代码
	CHvidicon::Instance()->OnContourOfMeanBgsub();
}

void CCAMCDoc::OnImageDetection()
{
	// TODO: 在此添加命令处理程序代码
	FCObjLayer layer(*GetCurrentLayer());
	layer.ConvertTo24Bit();
	IplImage* image=layer.ToIplImage();

	IplImage* pMask =m_bgSub.SxfBGDiffModel(image);
	HVidicom->SetDoc(this);
	HVidicom->CptBlobs(pMask);
	HVidicom->PartsRetrieval(pMask,image);

	if(pMask){
		FCObjLayer* pLayer1=GetLayer(1);
		if(pLayer1==NULL)
			return;
		IplImage* pImgProc=AutoStorage->CreateIplImage(pLayer1->Width(),pLayer1->Height(),pMask->depth,pMask->nChannels);	
		cvResize(pMask,pImgProc);

		HVidicom->CloneImageToLayer(pImgProc,pLayer1,1);//显示获取的轮廓图像
		AutoStorage->ReleaseIplImage(pImgProc);
		AutoStorage->ReleaseIplImage(pMask);
	}	
	cvReleaseImage(&image);
	UpdateAllViews(NULL);
}

void CCAMCDoc::On33069()
{
	// TODO: 在此添加命令处理程序代码
//	ID_DIALOG_TEST dlg;
//	dlg.DoModal();
	DialogSend Dlg;
	Dlg.DoModal();
/*	if (Dlg.DoModal()==IDOK)
	{
	}else
		AfxMessageBox(_T("Error"));
*/
	
}

void CCAMCDoc::OnFast()
{
	// TODO: 在此添加命令处理程序代码
	OnContinueshot( );
	OnSxfContour( );
	OnQuery( );
	OnNewsend();//类发送
	//On33069( ); dialog发送

}

void CCAMCDoc::OnStartreg()
{	
	// TODO: 在此添加命令处理程序代码

	CString strPath; 
	GetModuleFileName(NULL,strPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
//	strPath.ReleaseBuffer(); 

	CString str; 
	HKEY hRegKey; 
	BOOL bResult; 
	str=_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"); 
	if(RegOpenKey(HKEY_LOCAL_MACHINE, str, &hRegKey) != ERROR_SUCCESS) 
		bResult=FALSE; 
	else 
	{ 
		_splitpath(strPath.GetBuffer(0),NULL,NULL,str.GetBufferSetLength(MAX_PATH+1),NULL);
		strPath.ReleaseBuffer(); 
		str.ReleaseBuffer(); 
		if(::RegSetValueEx( hRegKey, 
			str, 
			0, 
			REG_SZ, 
			(CONST BYTE *)strPath.GetBuffer(0), 
			strPath.GetLength() ) != ERROR_SUCCESS) 
		{
			bResult=FALSE;
			AfxMessageBox("注册表注册 失败！！！！！！！");
		}
		else
		{
			bResult=TRUE;
			AfxMessageBox("注册表注册成功");
		}
		strPath.ReleaseBuffer(); 
	} 


	

}

void CCAMCDoc::OnNewsend()
{
	// TODO: 在此添加命令处理程序代码
	char   temp[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,temp); 

	CString   strTmp; 
	CString   str,c   ; 
	c.Format( "%s",temp); 
	str=c+"\\config.ini ";
	m_nCom=GetPrivateProfileInt( "test ", "com",1,str);  
	m_nBaud=115200;
	m_cParity='N';
	m_nDatabits=8;
	m_nStopbits=1;
	m_dwCommEvents = EV_RXFLAG | EV_RXCHAR;
	if (m_Port.InitPort(NULL, m_nCom, m_nBaud,m_cParity,m_nDatabits,m_nStopbits,m_dwCommEvents,512))
	{
		m_Port.StartMonitoring();
	//	AfxMessageBox("串口打开");
		//"当前状态：串口打开，无奇偶校验，8数据位，1停止位");
	}
	else
	{
		AfxMessageBox("没有发现此串口");
	}

	SetTimer(NULL,1,500,(TIMERPROC)MyTimerProcs);

	


}

void  CCAMCDoc::MyTimerProcs()
{
	CString m_x;
	CString m_data;
//	m_data.Format("%s","T0");
	int ax;
	int ay;
	int aag	;
	int as;
	int i;
	//ax=-(int)(100*intX);
	//ay=-(int)(100*intY);
	for (i=1;i<NUM;i++)
	{
		if(ID[i]==0)
			break;
	m_data.Format("%s","T0");
	ax=(int)(100*intY[i]);
	ax=ax;
	ay=(int)(100*intX[i]);
	ay=ay;
	aag=(int )(100*intAG[i]);
	if(score[i]!=0){
		as=10000-(int)(100*score[i]);
	}
	m_x.Format("%06d",ID[i]);
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
	
	if(m_Port.m_hComm==NULL)
		AfxMessageBox("串口没有打开，请打开串口");
	else
//		if (m_comp!=as)
		{
			m_Port.WriteToPort((LPCTSTR)m_data);	//发送数据        直接发送CString
			m_comp=as;
		//	AfxMessageBox(m_data);
		}
		
	}

}