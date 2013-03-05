// CAMCView.cpp : CCAMCView 类的实现
//

#include "stdafx.h"
#include "CAMC.h"

#include "CAMCDoc.h"
#include "CAMCView.h"
#include "Win32_Func.h"
#include ".\camcview.h"
#include "oxo_helper.h"
#include "Dline.h"
#include "Curve.h"
#include "Rectangle.h"
#include "CmdSelectionSet.h"
#include "StdDefine.h"
#include "BsplineCurve.h"
#include "LayerCommand.h"
#include "PaintBrush.h"
#include "CmdImgFillColor.h"
#include "MainFrm.h"

#include "ZoomIn.h"
#include "ZoomOut.h"
#include "Move.h"
#include "Distortion.h"
#include "Rotate.h"
#include "FillColor.h"
#include "DBDlg.h"
#include "CameraCalibration.h"
#include "Hvidicon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCAMCView

IMPLEMENT_DYNCREATE(CCAMCView, CScrollView)

BEGIN_MESSAGE_MAP(CCAMCView, CScrollView)
	//{{AFX_MSG_MAP(CCAMCView)

	


	
    
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)


	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MDIACTIVATE()
	ON_WM_ACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()

		//}}AFX_MSG_MAP
	// Standard printing commands

	ON_COMMAND(ID_VIEW_PROPERTIES, &CCAMCView::OnViewProperties)
	ON_COMMAND(ID_DB_EDIT, &CCAMCView::OnDbEdit)
	ON_COMMAND(ID_SET_ORIGIN_POINT, &CCAMCView::OnSetOriginPoint)
	ON_UPDATE_COMMAND_UI(ID_SET_ORIGIN_POINT, &CCAMCView::OnUpdateSetOriginPoint)
END_MESSAGE_MAP()

// CCAMCView 构造/析构
//==============================================================================
CCAMCView::CCAMCView()
{
	// TODO: 在此处添加构造代码
	m_CurrentTool= DEFAULT;
	m_pCurrentDraw = NULL;
	m_penState=0;	
	m_pBrush=NULL;
	if(m_popMenu.CreatePopupMenu()==0){
		AfxMessageBox("创建下拉菜单失败！");
	}
	m_popMenu.AppendMenu(MF_POPUP,IDM_DELETE_STROKE,"删除笔划");	
	m_pBmpInfo		= (BITMAPINFO *)m_chBmpBuf;
	m_pBmpInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	m_pBmpInfo->bmiHeader.biWidth			= 0;
	m_pBmpInfo->bmiHeader.biHeight			= 0;
	m_pBmpInfo->bmiHeader.biPlanes			= 1;
	m_pBmpInfo->bmiHeader.biBitCount		= 0;
	m_pBmpInfo->bmiHeader.biCompression		= BI_RGB;
	m_pBmpInfo->bmiHeader.biSizeImage		= 0;
	m_pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;
	m_pBmpInfo->bmiHeader.biClrUsed			= 0;
	m_pBmpInfo->bmiHeader.biClrImportant	= 0;

	for (int i = 0; i < 256; i++){
		m_pBmpInfo->bmiColors[i].rgbBlue		= (BYTE)i;
		m_pBmpInfo->bmiColors[i].rgbGreen		= (BYTE)i;
		m_pBmpInfo->bmiColors[i].rgbRed			= (BYTE)i;
		m_pBmpInfo->bmiColors[i].rgbReserved	= 0;	
	}
}
//==============================================================================
CCAMCView::~CCAMCView()
{
}
//==============================================================================
BOOL CCAMCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式
	//cs.lpszClass=AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,0,(HBRUSH)GetStockObject(BLACK_BRUSH),0);
	return CScrollView::PreCreateWindow(cs);
}

// CCAMCView 绘制
//==============================================================================
void CCAMCView::OnDraw(CDC* pDC)
{
	CCAMCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CDC MemDC; //首先定义一个显示设备对象
	
	CBitmap MemBitmap;//定义一个位图对象
	CRect rect;
	GetClientRect(&rect);
	FCObjCanvas& objCanvas = pDoc->GetCanvas();
	SIZE rcCanvas=objCanvas.GetCanvasScaledDimension();
	if(rcCanvas.cx>rect.Width()){
		rect.right=rcCanvas.cx;
	}
	if(rcCanvas.cy>rect.Height()){
		rect.bottom=rcCanvas.cy;
	}
	//随后建立与屏幕显示兼容的内存显示设备
	MemDC.CreateCompatibleDC(pDC);
	//这时还不能绘图，因为没有地方画 ^_^
	//下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小
	MemBitmap.CreateCompatibleBitmap(pDC,rect.Width(),rect.Height());

	//将位图选入到内存显示设备中
	//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
	CBitmap *pOldBit=MemDC.SelectObject(&MemBitmap);

	SIZE SizeofLayer ;
	SizeofLayer.cx = rect.Width();
	SizeofLayer.cy = rect.Height(); 
	SetScrollSizes(MM_TEXT,SizeofLayer);
	
	DrawInMemory(&MemDC);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCCOPY);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	//MemDC.SelectObject()
	//CDC::BitBlt()
}

//==============================================================================
//所以绘制的工作都在这个函数里面进行
void CCAMCView::DrawInMemory(CDC* pMemDC)
{
	CCAMCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	FCObjCanvas& objCanvas = pDoc->GetCanvas();
	
	SIZE m_size;
	CRect rcView;
	GetClientRect(&rcView);
	m_size.cx = rcView.Width();
	m_size.cy = rcView.Height();
	CPoint pScroll = this->GetScrollPosition();
	int nScrollX = pScroll.x;
	int nScrollY = pScroll.y;
	FCObjImage ImgView;
	objCanvas.MakeViewImage(nScrollX,nScrollY,m_size,FCObjImage::GetBGImage(),&ImgView);
	m_pBmpInfo->bmiHeader.biWidth	= ImgView.Width();
	m_pBmpInfo->bmiHeader.biHeight	= ImgView.Height();	
	m_pBmpInfo->bmiHeader.biBitCount = ImgView.ColorBits();
	::StretchDIBits(pMemDC->m_hDC,						  // hDC
								 nScrollX,				  // DestX
								 nScrollY,				   // DestY
								 ImgView.Width(),        // nDestWidth
								 ImgView.Height(),       // nDestHeight
								 0,						    // SrcX
								 0,							// SrcY
								 ImgView.Width(),         // nStartScan
								 ImgView.Height(),		// nNumScans
								 ImgView.GetMemStart(),   // lpBits
								 m_pBmpInfo,					// lpBitsInfo
								 DIB_RGB_COLORS,
								 SRCCOPY);  
}
//==============================================================================
void CCAMCView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

}
//==============================================================================
// CCAMCView 打印

BOOL CCAMCView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}
//==============================================================================
void CCAMCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印前添加额外的初始化
}
//==============================================================================
void CCAMCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 打印后添加清除过程
}
//==============================================================================
// CCAMCView 诊断

#ifdef _DEBUG
void CCAMCView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CCAMCView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CCAMCDoc* CCAMCView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCAMCDoc)));
	return (CCAMCDoc*)m_pDocument;
}
#endif //_DEBUG


// CCAMCView 消息处理程序


void CCAMCView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
//	Invalidate();

	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CCAMCView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	//Invalidate();

	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CCAMCView::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CScrollView::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	CCAMCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	pDoc->Activate();
	// TODO: 在此处添加消息处理程序代码
}

void CCAMCView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CScrollView::OnActivate(nState, pWndOther, bMinimized);
	CCAMCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	pDoc->Activate();
	// TODO: 在此处添加消息处理程序代码
}



void CCAMCView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);
	POINT pt={point.x,point.y};
	LPToCP(pt);
	CCAMCDoc* pDoc=GetDocument();
	if(pDoc!=NULL){
		if(m_CurrentTool==CAM_CALIBRATION){
			pDoc->SetOriginPoint(pt);
			m_CurrentTool=DEFAULT;
		}
	}
	CScrollView::OnLButtonDown(nFlags, point);
}

void CCAMCView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);
	if(m_pCurrentDraw!=NULL)
		m_pCurrentDraw->OnLButtonUp(&dc,this,point);
	CScrollView::OnLButtonUp(nFlags, point);
}

void CCAMCView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);
	POINT pt={point.x,point.y};
	LPToCP(pt);
	if(m_CurrentTool==CAM_CALIBRATION)
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS)); 
	if(CamCalib->IsValid()){
		CvPoint2D32f ptWCS;
		FCObjCanvas& objCanvas = GetCanvas();
		FCObjLayer* pLayer=objCanvas.GetCurrentLayer();
		CamCalib->CptWCSPoint(cvPoint2D32f(pt.x,pLayer->Height()-1-pt.y),ptWCS);
		//CamCalib->CptWCSPoint(cvPoint2D32f(pt.x,pt.y),ptWCS);
		CString strTitle;
		strTitle.Format("当前点图像坐标（%d,%d）；世界坐标：(%0.2f 毫米,%0.2f 毫米)",pt.x ,(pLayer->Height()-1-pt.y),ptWCS.x,ptWCS.y);
		CMainFrame* pMf=(CMainFrame*)(AfxGetApp()->m_pMainWnd);
		pMf->SetStatusBarText(strTitle);
	}
	CScrollView::OnMouseMove(nFlags, point);
}

void CCAMCView::LPToCP(POINT& point)
{
	FCObjCanvas& objCanvas = GetCanvas();
	POINT p=GetCanvasOffset();
	point.x-=p.x;
	point.y-=p.y;
	objCanvas.MapRealPoint(point);
}
void CCAMCView::CPToLP(POINT& point)
{
	FCObjCanvas& objCanvas = GetCanvas();
	objCanvas.MapRealPoint(point);
	POINT p=GetCanvasOffset();
	point.x+=p.x;
	point.y+=p.y;
}

POINT CCAMCView::GetCanvasOffset()
{
	CRect rect;
	GetClientRect(&rect);
	FCObjCanvas& objCanvas = GetCanvas();
	POINT ptoffset = objCanvas.GetViewOffset(rect.Size());
	return ptoffset;
}
FCObjCanvas& CCAMCView::GetCanvas()
{
	CCAMCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	
	return pDoc->GetCanvas();	
}
void CCAMCView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CClientDC dc(this);
	if(m_CurrentTool==SELECT_TOOL){
		ClientToScreen(&point);
		UINT uFlags;
		uFlags = TPM_LEFTALIGN | TPM_RIGHTBUTTON;
		m_popMenu.TrackPopupMenu(uFlags,point.x, point.y,this, NULL);	
	}
	if(m_pCurrentDraw==NULL)
		return;
	if(m_pCurrentDraw->IsClassType()==BSPLINE_TOOL&&m_CurrentTool==BSPLINE_TOOL){
		CBsplineCurve* bs=static_cast<CBsplineCurve*>(m_pCurrentDraw);
		bs->OnRButtonDown(&dc,this,point);
	}
	CScrollView::OnRButtonDown(nFlags, point);
}



int CCAMCView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}


void CCAMCView::SetZoomRateText()
{
	CMainFrame*pMainFrame=(CMainFrame*)::AfxGetMainWnd();
	int zoomrate=GetCanvas().GetZoomScale();
	CString str;
	if ((zoomrate==1)||(zoomrate==-1)) 
	{
		str.Format("原图大小");
	}
	else if (zoomrate<0) 
	{
		str.Format("缩小了%d倍",zoomrate*(-1));
	}
	else
	{
		str.Format("放大了%d倍",zoomrate);
	}
	pMainFrame->SetStatusBarText(str);
}
void CCAMCView::UpdateRectView()
{
	
}
void CCAMCView::Draw(){
	try
	{
		CClientDC dc(this);
		OnDraw(&dc);
	}
	catch (CException* e)
	{
		e->ReportError();
	}
}
void CCAMCView::OnViewProperties()
{
	// TODO: 在此添加命令处理程序代码
}

void CCAMCView::OnDbEdit()
{
	// TODO: 在此添加命令处理程序代码
	CDBDlg dbDlg;
	dbDlg.DoModal();
}

void CCAMCView::OnSetOriginPoint()
{
	// TODO: 在此添加命令处理程序代码
	m_CurrentTool=CAM_CALIBRATION;
}

void CCAMCView::OnUpdateSetOriginPoint(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_CurrentTool==CAM_CALIBRATION);
}
