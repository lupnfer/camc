// DlgBrightnessAdjust.cpp : implementation file
//

#include "stdafx.h"
#include "CAMC.h"
#include "DlgBrightnessAdjust.h"
#include ".\dlgbrightnessadjust.h"
#include "CmdImgEffect.h"
#include "CmdImgBrightness.h"
#include "CmdImgContrast.h"
#include "CAMCDoc.h"
#include "LayerCommand.h"
#include "Win32_Func.h"
#include "oxo_helper.h"
#include "ObjProgress.h"
#include "ObjSelect.h"
// CDlgBrightnessAdjust dialog

IMPLEMENT_DYNAMIC(CDlgBrightnessAdjust, CDialog)
CDlgBrightnessAdjust::CDlgBrightnessAdjust(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBrightnessAdjust::IDD, pParent)
{
	m_pImgThumb = NULL;
}

CDlgBrightnessAdjust::~CDlgBrightnessAdjust()
{
	if(m_pImgThumb != NULL)
		delete m_pImgThumb;
	m_pImgThumb = NULL;

}

void CDlgBrightnessAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgBrightnessAdjust, CDialog)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CDlgBrightnessAdjust message handlers

void CDlgBrightnessAdjust::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	
	CSliderCtrl  *pSlideBright = (CSliderCtrl  *)GetDlgItem(IDC_SLIDER_BRIGHT);
	CSliderCtrl  *pSlideContrast = (CSliderCtrl  *)GetDlgItem(IDC_SLIDER_CONTRAST);
	CString str;
	SetThumbImage();
	FCObjImage * pImage = GetThumbImage();
	CCmdImgBrightness* CmdImgBrightAdjust = new CCmdImgBrightness(0,CHANNEL_RGB);
	CCmdImgContrast* CmdImgContrastAdjust = new CCmdImgContrast(0,CHANNEL_RGB);
	switch(pScrollBar->GetDlgCtrlID())
	{
	case IDC_SLIDER_BRIGHT:
		str.Format("%d",pSlideBright->GetPos()-100);
		SetDlgItemText(IDC_STATIC_BRIGHT,str);
		break;
	case IDC_SLIDER_CONTRAST:
		str.Format("%d",pSlideContrast->GetPos()-100);
		SetDlgItemText(IDC_STATIC_CONTRAST,str);
		break;
	}	

	CmdImgBrightAdjust->SetBrightness(pSlideBright->GetPos()-100);
	CmdImgBrightAdjust->Implement(*pImage,NULL);
	CmdImgContrastAdjust->SetContrast(pSlideContrast->GetPos()-100);
	CmdImgContrastAdjust->Implement(*pImage,NULL);
	delete CmdImgBrightAdjust;
	CmdImgBrightAdjust = NULL;	
	delete CmdImgContrastAdjust;
	CmdImgContrastAdjust = NULL;

	Invalidate();
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

}

BOOL CDlgBrightnessAdjust::OnInitDialog()
{
	CDialog::OnInitDialog();
	CSliderCtrl  *pSlideBright = (CSliderCtrl  *)GetDlgItem(IDC_SLIDER_BRIGHT);
	CSliderCtrl  *pSlideContrast = (CSliderCtrl  *)GetDlgItem(IDC_SLIDER_CONTRAST);
	pSlideBright->SetRange(0,200);
	pSlideBright->SetPos(100);
	pSlideContrast->SetRange(0,200);
	pSlideContrast->SetPos(100);
	CString str;
	str.Format("%d",pSlideBright->GetPos()-100);
	SetDlgItemText(IDC_STATIC_BRIGHT,str);
	str.Format("%d",pSlideContrast->GetPos()-100);
	SetDlgItemText(IDC_STATIC_CONTRAST,str);
	
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBrightnessAdjust::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CWnd *pWndofLittleImage = GetDlgItem(IDC_STATIC_SHOWIMAGE);
	CDC *pDCofLittleImage =pWndofLittleImage->GetDC();
	pWndofLittleImage->Invalidate();
	pWndofLittleImage->UpdateWindow();
	pDCofLittleImage->Rectangle(0,0,200,200);
	CPen *pPenGreen = new CPen;
	pPenGreen->CreatePen(PS_SOLID,1,RGB(0,255,0));
	CGdiObject *pOldPencil = pDCofLittleImage->SelectObject(pPenGreen);
	pDCofLittleImage->MoveTo(0,0);
	pDCofLittleImage->LineTo(0,200);
	pDCofLittleImage->LineTo(200,200);
	pDCofLittleImage->LineTo(200,0);
	pDCofLittleImage->LineTo(0,0);
	
	BITMAPINFO* pBmpInfo=(BITMAPINFO*)FLib_NewImgInfoWithPalette(*m_pImgThumb);
	::StretchDIBits(pDCofLittleImage->m_hDC,						  // hDC
										0,				  // DestX
										0,				   // DestY
										m_pImgThumb->Width(),        // nDestWidth
										m_pImgThumb->Height(),       // nDestHeight
										0,						    // SrcX
										0,							// SrcY
										m_pImgThumb->Width(),         // nStartScan
										m_pImgThumb->Height(),		// nNumScans
										m_pImgThumb->GetMemStart(),   // lpBits
										pBmpInfo,					// lpBitsInfo
										DIB_RGB_COLORS,
										SRCCOPY);  
	delete pBmpInfo;
	delete pPenGreen;
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
}

// 设置缩略图
void CDlgBrightnessAdjust::SetThumbImage()
{
	// 保存原图
	
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());
	FCObjCanvas& canvas = pDoc->GetCanvas();
	m_pImgThumb=new FCObjImage();

	int i = pDoc->GetLayerBar()->m_ListBox.GetCurSel();
	FCObjLayer* m_pLayer = pDoc->GetCanvas().GetLayer(i);
	FCObjImage m_Undo = * static_cast<FCObjImage *>(m_pLayer) ;

	//FCObjLayer* layer=m_pObjCanvas.GetCurrentLayer();
	if ((m_pLayer == NULL) || !m_pLayer->IsValidImage())
	{
		m_pLayer = NULL ;
		FAssert(FALSE) ; return ;
	}

	if (!canvas.HasSelected()) // 整图
	{
		//m_pImgThumb(*m_pLayer);
		canvas.MakeLayerThumbnail(m_pImgThumb,200,200,*m_pLayer,FCObjImage::GetBGImage());
	}
	else
	{
		// 先挖出一个rect处理
		RECT		rcSel ;
		canvas.GetSelectLayerRect (*m_pLayer, &rcSel) ;
		if (IsRectEmpty (&rcSel))
		{
			m_pLayer = NULL ; canvas = NULL ;
			FAssert(FALSE) ; return ;
		}

		// 保存原区域
		m_pLayer->GetSubBlock (&m_Undo, rcSel) ;

		// 把区域之外alpha置0，这个很重要，否则会影响如“box blur”等需要alpha参与的算法
		int				x, y ;
		FCObjImage		block(m_Undo), imgSelection ;
		canvas.GetSelectLayerMask (*m_pLayer, &imgSelection) ;
		for (y=0 ; y < block.Height() ; y++)
		{
			BYTE	* pBlock = block.GetBits(y),
				* pSel = imgSelection.GetBits(y) ;
			for (x=0 ; x < block.Width() ; x++, pBlock+=4, pSel++)
				if (*pSel == 0)
					pBlock[3] = 0 ;
		}


		if ((block.Width()<=200)&&(block.Height()<=200)) {
			m_pImgThumb->Create(block.Width(),block.Height(),32);
			for(int y = 0; y<block.Height();y++)
				for(int x = 0;x<block.Width();x++)
				{
					DWORD pixel = block.GetPixelData1(x,y);
					m_pImgThumb->SetPixelData(x,y,pixel);
				}
		}
		else
		{
			int nmax = FMax(block.Width(),block.Height());
			float ratio = nmax /200.0 ;
			int newwidth = block.Width()/ratio;
			int newheight = block.Height()/ratio;


			/*float xrate = block.Width()/200.0;
			float yrate = block.Height()/200.0;
			int nWidth=200;//(int)(block.Width()/xrate);
			int nHight=200;//(int)(block.Height()/yrate);
			m_pImgThumb->Create(nWidth,nHight,32);*/


			m_pImgThumb->Create(newwidth,newheight,32);

			for(int y = 0; y<block.Height()-1;y++)
				for(int x = 0;x<block.Width()-1;x++)
				{
					int xx = x / ratio;
					int yy = y / ratio;
					DWORD pixel = block.GetPixelData1(x,y);
					m_pImgThumb->SetPixelData(xx,yy,pixel);
				}

		}
	}

}

// 获得缩略图
FCObjImage* CDlgBrightnessAdjust::GetThumbImage(void)
{
	return m_pImgThumb;
}

int CDlgBrightnessAdjust::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetThumbImage();
	return 0;
}

void CDlgBrightnessAdjust::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::OnCancel();
}

void CDlgBrightnessAdjust::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	CSliderCtrl  *pSlideBright = (CSliderCtrl  *)GetDlgItem(IDC_SLIDER_BRIGHT);
	CSliderCtrl  *pSlideContrast = (CSliderCtrl  *)GetDlgItem(IDC_SLIDER_CONTRAST);
	int nBright = pSlideBright->GetPos()-100;
	int nContrast = pSlideContrast->GetPos()-100;
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());
	FCObjCanvas& m_pObjCanvas = pDoc->GetCanvas();

	FCObjProgress percent;
	m_pObjCanvas.ExecuteEffect(new CCmdImgBrightness(nBright,CHANNEL_RGB),&percent );
	m_pObjCanvas.ExecuteEffect(new CCmdImgContrast(nContrast,CHANNEL_RGB) ,&percent);
	pDoc->SetModifiedFlag(TRUE);
	pDoc->UpdateAllViews(NULL);
	CDialog::OnOK();

}
