// IconListBox.cpp : implementation file
//

#include "stdafx.h"
#include "IconListBox.h"
#include ".\iconlistbox.h"
#include "resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconListBox

CIconListBox::CIconListBox()
{
	// Set the Image List Pointer to NULL - Initialization
	m_pImageList = NULL;
	PopMenu.LoadMenu(IDR_POPMENU); 
	//this->Create()
}

CIconListBox::~CIconListBox()
{
	PopMenu.DestroyMenu();
}


BEGIN_MESSAGE_MAP(CIconListBox, CCheckListBox)
	//{{AFX_MSG_MAP(CIconListBox)
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconListBox message handlers

void CIconListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	
	CDC* pDC    = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (((LONG)(lpDrawItemStruct->itemID) >= 0) &&
		(lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))){

		CRect  rcItem(lpDrawItemStruct->rcItem); // To draw the focus rect.
		CRect  rClient(rcItem.left,rcItem.top,rcItem.right,rcItem.bottom); // Rect to highlight the Item
		CRect  rText(rClient); // Rect To display the Text
		CPoint Pt( rClient.left , rClient.top ); // Point To draw the Image

	// if the Image list exists for the list box
	// adjust the Rect sizes to accomodate the Image for each item.
		int cyItem = GetItemHeight(lpDrawItemStruct->itemID);
		if (m_cyText == 0)
		{
			CClientDC dc(this);
			CFont* pOldFont = dc.SelectObject(GetFont());
			TEXTMETRIC tm;
			VERIFY (dc.GetTextMetrics ( &tm ));
			dc.SelectObject(pOldFont);
			m_cyText = tm.tmHeight;
		}

		if(m_pImageList)
		{
			rClient.left += 32;
			rText.left += 32;
			rText.top += max(0, (cyItem - m_cyText) / 2);
		}
		else
		{
			rText.top += max(0, (cyItem - m_cyText) / 2);
		}


		COLORREF crText;
		CString strText;

		// Image information in the item data.
		int iImg = (int)lpDrawItemStruct->itemData;

		// If item selected, draw the highlight rectangle.
		// Or if item deselected, draw the rectangle using the window color.
		if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
			(lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
		{
			CBrush br(::GetSysColor(COLOR_HIGHLIGHT));
		//CRect r(rcItem.left+10,rcItem.top,rcItem.right,rcItem.bottom);
			pDC->FillRect(&rClient, &br);
		}
		else if (!(lpDrawItemStruct->itemState & ODS_SELECTED) && 
			(lpDrawItemStruct->itemAction & ODA_SELECT)) 
		{
			CBrush br(::GetSysColor(COLOR_WINDOW));
			//CRect r(rcItem.left+10,rcItem.top,rcItem.right,rcItem.bottom);
			pDC->FillRect(&rClient, &br);
		}

		// If the item has focus, draw the focus rect.
		// If the item does not have focus, erase the focus rect.
		if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && 
			(lpDrawItemStruct->itemState & ODS_FOCUS))
		{
			pDC->DrawFocusRect(&rcItem); 
		}
		else if ((lpDrawItemStruct->itemAction & ODA_FOCUS) &&	
			!(lpDrawItemStruct->itemState & ODS_FOCUS))
		{
			pDC->DrawFocusRect(&rcItem); 
		}
	
	// To draw the Text set the background mode to Transparent.
	
	//lpDrawItemStruct->CtlID
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
			crText = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		else if (lpDrawItemStruct->itemState & ODS_DISABLED)
			crText = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
		else
			crText = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
   
	// Get the item text.
		GetText(lpDrawItemStruct->itemID, strText);
		int iBkMode = pDC->SetBkMode(TRANSPARENT);
		// Setup the text format.
		UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
		if (GetStyle() & LBS_USETABSTOPS)
			nFormat |= DT_EXPANDTABS;


	// if the ImageList is Existing and there is an associated Image
	// for the Item, draw the Image.


		if(m_pImageList && (iImg != -1 )){
			m_pImageList->Draw(pDC,iImg,Pt,ILD_NORMAL);
			CPen myPen;
			myPen.CreatePen(PS_SOLID,2,RGB(255,0,0)); 
			pDC->SetROP2(R2_NOTXORPEN);
			CPen* oldPen=pDC->SelectObject(&myPen);
			pDC->Rectangle(Pt.x,Pt.y,Pt.x+32,Pt.y+32);
			pDC->SelectObject(oldPen);
		}
	
	//Draw the Text
		pDC->DrawText(strText, -1, &rText, nFormat | DT_CALCRECT);
		pDC->DrawText(strText, -1, &rText, nFormat);	
		pDC->SetTextColor(crText); 
		pDC->SetBkMode(iBkMode);
	}
}

void CIconListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	lpMeasureItemStruct->itemHeight=32;
	lpMeasureItemStruct->itemWidth  =32;
}
int CIconListBox::CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	return 0;
}
int CIconListBox::AddString(LPCTSTR lpszItem)
{
	int iRet = CListBox::AddString(lpszItem);
	if (iRet >= 0)
		SetItemData(iRet, -1);
	return iRet;
}

int CIconListBox::AddString(LPCTSTR lpszItem, int iImg)
{
	int iRet = CListBox::AddString(lpszItem);
	if (iRet >= 0){
		SetItemData(iRet, iImg);
		SetCheck(iImg,1);
		SetCurSel(iImg);
	}
	return iRet;
}

int CIconListBox::InsertString(int iIndex, LPCTSTR lpszItem)
{
	int iRet = CListBox::InsertString(iIndex,lpszItem);
	if (iRet >= 0){
		SetItemData(iRet, iIndex);
		SetCheck(iIndex,1);
		SetCurSel(iIndex);
	}
	return iRet;
}

int CIconListBox::InsertString(int iIndex, LPCTSTR lpszItem, int iImg)
{
	int iRet = CListBox::InsertString(iIndex,lpszItem);
	if (iRet >= 0)
		SetItemData(iRet, iImg);
	return iRet;
}

void CIconListBox::SetItemImage(int iIndex, int iImg)
{
	SetItemData(iIndex, iImg);
	RedrawWindow();
}

BOOL CIconListBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle = LBS_MULTIPLESEL|LBS_EXTENDEDSEL;
	return CCheckListBox::Create(dwStyle, rect, pParentWnd, nID);
}

void CIconListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint pt; 
	pt= point ; 
	this->ClientToScreen(&pt);
	// show the menu (returns, when menu is closed again!) 
	CMenu* pMenu = PopMenu.GetSubMenu (0); 
	pMenu->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		pt.x, pt.y, this); 
	
	//this->ScreenToClient(&pt);
	CCheckListBox::OnRButtonDown(nFlags, point);
}
