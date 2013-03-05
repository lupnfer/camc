// CAMCView.cpp : CCAMCView ���ʵ��
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

// CCAMCView ����/����
//==============================================================================
CCAMCView::CCAMCView()
{
	// TODO: �ڴ˴���ӹ������
	m_CurrentTool= DEFAULT;
	m_pCurrentDraw = NULL;
	m_penState=0;	
	m_pBrush=NULL;
	if(m_popMenu.CreatePopupMenu()==0){
		AfxMessageBox("���������˵�ʧ�ܣ�");
	}
	m_popMenu.AppendMenu(MF_POPUP,IDM_DELETE_STROKE,"ɾ���ʻ�");	
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
	// TODO: �ڴ˴�ͨ���޸� CREATESTRUCT cs ���޸Ĵ������
	// ��ʽ
	//cs.lpszClass=AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,0,(HBRUSH)GetStockObject(BLACK_BRUSH),0);
	return CScrollView::PreCreateWindow(cs);
}

// CCAMCView ����
//==============================================================================
void CCAMCView::OnDraw(CDC* pDC)
{
	CCAMCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CDC MemDC; //���ȶ���һ����ʾ�豸����
	
	CBitmap MemBitmap;//����һ��λͼ����
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
	//���������Ļ��ʾ���ݵ��ڴ���ʾ�豸
	MemDC.CreateCompatibleDC(pDC);
	//��ʱ�����ܻ�ͼ����Ϊû�еط��� ^_^
	//���潨��һ������Ļ��ʾ���ݵ�λͼ������λͼ�Ĵ�С������ô��ڵĴ�С
	MemBitmap.CreateCompatibleBitmap(pDC,rect.Width(),rect.Height());

	//��λͼѡ�뵽�ڴ���ʾ�豸��
	//ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ��
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
//���Ի��ƵĹ���������������������
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
	// TODO: �������ͼ�ĺϼƴ�С
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

}
//==============================================================================
// CCAMCView ��ӡ

BOOL CCAMCView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}
//==============================================================================
void CCAMCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��ӡǰ��Ӷ���ĳ�ʼ��
}
//==============================================================================
void CCAMCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��ӡ������������
}
//==============================================================================
// CCAMCView ���

#ifdef _DEBUG
void CCAMCView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CCAMCView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CCAMCDoc* CCAMCView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCAMCDoc)));
	return (CCAMCDoc*)m_pDocument;
}
#endif //_DEBUG


// CCAMCView ��Ϣ�������


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
	// TODO: �ڴ˴������Ϣ����������
}

void CCAMCView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CScrollView::OnActivate(nState, pWndOther, bMinimized);
	CCAMCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	pDoc->Activate();
	// TODO: �ڴ˴������Ϣ����������
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
		strTitle.Format("��ǰ��ͼ�����꣨%d,%d�����������꣺(%0.2f ����,%0.2f ����)",pt.x ,(pLayer->Height()-1-pt.y),ptWCS.x,ptWCS.y);
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}


void CCAMCView::SetZoomRateText()
{
	CMainFrame*pMainFrame=(CMainFrame*)::AfxGetMainWnd();
	int zoomrate=GetCanvas().GetZoomScale();
	CString str;
	if ((zoomrate==1)||(zoomrate==-1)) 
	{
		str.Format("ԭͼ��С");
	}
	else if (zoomrate<0) 
	{
		str.Format("��С��%d��",zoomrate*(-1));
	}
	else
	{
		str.Format("�Ŵ���%d��",zoomrate);
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
	// TODO: �ڴ���������������
}

void CCAMCView::OnDbEdit()
{
	// TODO: �ڴ���������������
	CDBDlg dbDlg;
	dbDlg.DoModal();
}

void CCAMCView::OnSetOriginPoint()
{
	// TODO: �ڴ���������������
	m_CurrentTool=CAM_CALIBRATION;
}

void CCAMCView::OnUpdateSetOriginPoint(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_CurrentTool==CAM_CALIBRATION);
}
