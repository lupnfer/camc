// FCLayerBar.cpp : 实现文件
//

#include "stdafx.h"
#include "CAMC.h"
#include "CAMCDoc.h"
#include "FCLayerBar.h"
#include "FClayerbar.h"
#include "IconListBox.h"
#include "ObjLayer.h"
#include "LayerCommand.h"
#include "Win32_Func.h"
#include ".\fclayerbar.h"
#include "Resource.h"
// FCLayerBar 对话框

IMPLEMENT_DYNAMIC(FCLayerBar, CBCGPDialogBar)
FCLayerBar::FCLayerBar(CWnd* pParent /*=NULL*/)
	: CBCGPDialogBar(/*FCLayerBar::IDD, pParent*/)
{
	m_pObjCanvas=NULL;
	HIMAGELIST hList = ImageList_Create(32,32, ILC_COLOR24|ILC_MASK,0,20);
	m_imgNormal.Attach(hList);
	m_ListBox.SetImageList(&m_imgNormal);
}

FCLayerBar::~FCLayerBar()
{
	//m_ListBox.CloseWindow();
}

void FCLayerBar::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ICONLISTBOX, m_ListBox);
}
BOOL FCLayerBar::Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID)
{
 // TODO: Add your specialized code here and/or call the base class
	BOOL bRes= CBCGPDialogBar::Create(pParentWnd,nIDTemplate,nStyle,nID );
	//
	//BOOL bRes= CBCGPDialogBar::Create(pParentWnd,nIDTemplate,nStyle,nID );
	InitDialogBar();//在类中添加一个成员函数就可以了

	return bRes;
}
BOOL FCLayerBar::InitDialogBar()
{
	UpdateData(FALSE);//这个一定要啊，这样就会有和CDialog一样的数据交换效果了

	return TRUE;
}
void FCLayerBar::InitImageList()
{
		
	if(m_pObjCanvas==NULL)
		return;
	int num = 0;
	num = m_pObjCanvas->GetLayerNumber();
	FCObjImage* pImgThumb=new FCObjImage();
	
	ClearImageList();
	for(int i=0;i<num;++i){
		FCObjLayer* layer=m_pObjCanvas->GetLayer(i);
		m_pObjCanvas->MakeLayerThumbnail(pImgThumb,32,32,*layer,FCObjImage::GetBGImage());
		CBitmap* bitmap=FLib_CreateBitmapHandle(*pImgThumb);
		m_imgNormal.Add(bitmap,RGB(0,0,0));
		
		CString str;
		if(i == 0)
			str.Format("背景层");	
		else
			str.Format("图层%d",i);
		m_ListBox.AddString(str,i);
		bitmap->DeleteObject();
	}
	delete pImgThumb;

}
//刷新图层i的缩略图
void FCLayerBar::FlushLayerThumbnail(int i)
{		
		FCObjImage* pImgThumb=new FCObjImage();
		FCObjLayer* layer=m_pObjCanvas->GetLayer(i);
		if(layer==NULL)
			return;
		m_imgNormal.Remove(i);
		m_pObjCanvas->MakeLayerThumbnail(pImgThumb,32,32,*layer,FCObjImage::GetBGImage());
		CBitmap* bitmap=FLib_CreateBitmapHandle(*pImgThumb);
		m_imgNormal.Add(bitmap,RGB(0,0,0));
		m_ListBox.Invalidate(FALSE);
		bitmap->DeleteObject();	
		delete pImgThumb;
}
void FCLayerBar::Add(FCObjLayer& layer)//加入一个图层
{
	
	FCObjImage* pImgThumb=new FCObjImage();
	m_pObjCanvas->MakeLayerThumbnail(pImgThumb,32,32,layer,
											FCObjImage::GetBGImage());
	
	int num=m_pObjCanvas->GetLayerNumber();
	CString str;
	CBitmap* bitmap=FLib_CreateBitmapHandle(*pImgThumb);
	m_imgNormal.Add(bitmap,RGB(0,0,0));
	if(num-1 == 0)
		str.Format("background");	
	else
		str.Format("图层%d",num-1);
	m_ListBox.InsertString(num-1,str);
	delete pImgThumb;
	bitmap->DeleteObject();
}
void FCLayerBar::Add(CString str)
{
	m_ListBox.AddString(str);
}
void FCLayerBar::Delete(int index)
{
	/*
	int nTotelItem = m_ListBox.GetCount();
	for(int i = nTotelItem; i >=index ;i--)
	{
		m_ListBox.DeleteString(i);
     }
	for(int i = index+1; i <= nTotelItem -1 ;i++)
	{
		int j = i -1;
		CString str;
		str.Format("图层%d",i-1);
		m_ListBox.AddString(str,i-1);
	}
	if(index  == nTotelItem-1)
		{
			m_ListBox.SetCurSel(index -1);
		}
	else
		{
			m_ListBox.SetCurSel(index);
		}	
	RePaint();
	*/
	InitImageList();

}
void FCLayerBar::ClearImageList()
{
	int count=m_imgNormal.GetImageCount();
	for(int i=count;i>=0;i--){
		m_imgNormal.Remove(i);
	}
	count=m_ListBox.GetCount();
	for(int i=count;i>=0;i--){
		m_ListBox.DeleteString(i);
	}
}
BEGIN_MESSAGE_MAP(FCLayerBar, CBCGPDialogBar)
	ON_LBN_SELCHANGE(IDC_ICONLISTBOX, OnLbnSelchangeIconlistbox)
END_MESSAGE_MAP()


void FCLayerBar::OnLbnSelchangeIconlistbox()
{
	// TODO: Add your control notification handler code here
	int nSelectIndex = m_ListBox.GetCurSel();
	FCObjLayer* layer=m_pObjCanvas->GetLayer(nSelectIndex);
	
	if(m_ListBox.GetCheck(nSelectIndex))
	{
		m_ListBox.SetCheck(nSelectIndex,1);
		layer->SetLayerVisible(TRUE);			
	}
	else
	{
		m_ListBox.SetCheck(nSelectIndex,0);
		layer->SetLayerVisible(FALSE);
	}

	RePaint();

}
void FCLayerBar::RePaint()
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CCAMCDoc* pDoc =(CCAMCDoc*)(pChild->GetActiveDocument());
	pDoc->UpdateAllViews(NULL);

}

