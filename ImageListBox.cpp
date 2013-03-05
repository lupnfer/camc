//
// ImageListBox.cpp : implementation of CImageListBox class
//

//
// Author :    GHINI Mauro
//

#include "stdafx.h"

#ifndef __IMAGELISTBOX_H__
#include "ImageListBox.h"
#endif //__IMAGELISTBOX_H__

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdDefine.h"
/////////////////////////////////////////////////////////////////////////////
// ILB_IMAGE_DATA
//
struct ILB_IMAGE_DATA
{
public:
	int m_nImage;
	DWORD m_dwUserData;

	ILB_IMAGE_DATA()
	{
		m_nImage = -1;
		m_dwUserData = 0;
	};
};

/////////////////////////////////////////////////////////////////////////////
// CImageListBox

BEGIN_MESSAGE_MAP(CImageListBox, CListBox)
	//{{AFX_MSG_MAP(CImageListBox)
	ON_WM_CREATE()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(LB_ADDSTRING, OnLBAddString)
	ON_MESSAGE(LB_FINDSTRING, OnLBFindString)
	ON_MESSAGE(LB_FINDSTRINGEXACT, OnLBFindStringExact)
	ON_MESSAGE(LB_GETITEMDATA, OnLBGetItemData)
	ON_MESSAGE(LB_GETTEXT, OnLBGetText)
	ON_MESSAGE(LB_INSERTSTRING, OnLBInsertString)
	ON_MESSAGE(LB_SELECTSTRING, OnLBSelectString)
	ON_MESSAGE(LB_SETITEMDATA, OnLBSetItemData)
	ON_MESSAGE(LB_SETITEMHEIGHT, OnLBSetItemHeight)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CImageListBox::CImageListBox()
	{ m_pImageList = NULL; }

BOOL CImageListBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!(dwStyle & LBS_OWNERDRAWVARIABLE)) //must be one or the other
		dwStyle |= LBS_OWNERDRAWFIXED;
	return CListBox::Create(dwStyle, rect, pParentWnd, nID);
}

CImageList* CImageListBox::SetImageList(CImageList* pImageList)
{
    ASSERT(pImageList != NULL);

    CImageList* pPrevImageList = m_pImageList;
    m_pImageList = pImageList;
    IMAGEINFO iiImg;
    m_pImageList->GetImageInfo(0, &iiImg);
    m_bmSize.cx = iiImg.rcImage.right;
    m_bmSize.cy = iiImg.rcImage.bottom;

    OnLBSetItemHeight(0, 0);

    return pPrevImageList;
}

CImageList* CImageListBox::GetImageList()
{
    return m_pImageList;
}

int CImageListBox::AddItem(int nImageIndex, DWORD dwItemData)
{
    ASSERT(m_pImageList != NULL);
    if (m_pImageList->GetImageCount() < nImageIndex)
        return LB_ERR;

    char szText[] = "";
    WPARAM wParam = 0;
    LRESULT lResult = DefWindowProc(LB_ADDSTRING, wParam, (LPARAM)szText);
    if (lResult == LB_ERR || lResult == LB_ERRSPACE)
        return lResult;

	ILB_IMAGE_DATA* pState = NULL;
	pState = new ILB_IMAGE_DATA;
    pState->m_nImage = nImageIndex;
	pState->m_dwUserData = dwItemData;
    wParam = (WPARAM)lResult;
    LRESULT lRes = DefWindowProc(LB_SETITEMDATA, wParam, (LPARAM)pState);

	if (lRes == LB_ERR && pState != NULL)
		delete pState;

	return lResult;
}

void CImageListBox::MeasureItem(LPMEASUREITEMSTRUCT)
{
	// You must override DrawItem and MeasureItem for LBS_OWNERDRAWVARIABLE
	ASSERT((GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS)) ==
		(LBS_OWNERDRAWFIXED | LBS_HASSTRINGS));
}

void CImageListBox::PreDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	DRAWITEMSTRUCT drawItem;
	memcpy(&drawItem, lpDrawItemStruct, sizeof(DRAWITEMSTRUCT));

	if ((((LONG)drawItem.itemID) >= 0) &&
	   ((drawItem.itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) != 0))
	{
		int cyItem = GetItemHeight(drawItem.itemID);

		CDC* pDC = CDC::FromHandle(drawItem.hDC);
		COLORREF newBkColor = GetSysColor(COLOR_WINDOW);

		BOOL fDisabled = !IsWindowEnabled();
		if ((drawItem.itemState & ODS_SELECTED) && !fDisabled)
			newBkColor = GetSysColor(COLOR_HIGHLIGHT);

		COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

		CDC bitmapDC;
		if (bitmapDC.CreateCompatibleDC(pDC))
		{
            ILB_IMAGE_DATA* pState = (ILB_IMAGE_DATA*)lpDrawItemStruct->itemData;

			CRect rectImage = drawItem.rcItem;
			rectImage.left += 1 + FMax<LONG>(0, (rectImage.right - m_bmSize.cx) / 2);
			rectImage.top += 1 + FMax<LONG>(0, (cyItem - m_bmSize.cy) / 2);
			rectImage.right = rectImage.left + m_bmSize.cx;
			rectImage.bottom = rectImage.top + m_bmSize.cy;

			CBrush brush(newBkColor);
			pDC->FillRect(&(drawItem.rcItem), &brush);

            POINT pt;
            pt.x = rectImage.left;
            pt.y = rectImage.top;
            m_pImageList->Draw(pDC, pState->m_nImage, pt, ILD_NORMAL);
		}
		pDC->SetBkColor(oldBkColor);
	}

	if ((drawItem.itemAction & ODA_FOCUS) != 0)
    {
        CDC* pDC = CDC::FromHandle(drawItem.hDC);
		pDC->DrawFocusRect(&(drawItem.rcItem));
    }
}

void CImageListBox::PreMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	int cyItem = CalcMinimumItemHeight();

	MEASUREITEMSTRUCT measureItem;
	memcpy(&measureItem, lpMeasureItemStruct, sizeof(MEASUREITEMSTRUCT));

	measureItem.itemHeight = cyItem;
	measureItem.itemWidth  = (UINT)-1;

	// WINBUG: Windows95 and Windows NT disagree on what this value
	// should be.  According to the docs, they are both wrong
	if (GetStyle() & LBS_OWNERDRAWVARIABLE)
	{
		LRESULT lResult = DefWindowProc(LB_GETITEMDATA, measureItem.itemID, 0);
		if (lResult != LB_ERR)
			measureItem.itemData = (UINT)lResult;
		else
			measureItem.itemData = 0;

		// WINBUG: This is only done in the LBS_OWNERDRAWVARIABLE case
		// because Windows 95 does not initialize itemData to zero in the
		// case of LBS_OWNERDRAWFIXED list boxes (it is stack garbage).
		if (measureItem.itemData != 0)
		{
			ILB_IMAGE_DATA* pState = (ILB_IMAGE_DATA*)measureItem.itemData;
			measureItem.itemData = pState->m_dwUserData;
		}
	}

	MeasureItem(&measureItem);

	lpMeasureItemStruct->itemHeight = max(measureItem.itemHeight,(UINT) cyItem);
	lpMeasureItemStruct->itemWidth = measureItem.itemWidth;
}

int CImageListBox::PreCompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	COMPAREITEMSTRUCT compareItem;
	memcpy(&compareItem, lpCompareItemStruct, sizeof(COMPAREITEMSTRUCT));

	if (compareItem.itemData1 != 0)
	{
		ILB_IMAGE_DATA* pState = (ILB_IMAGE_DATA*)compareItem.itemData1;
		compareItem.itemData1 = pState->m_dwUserData;
	}
	if (compareItem.itemData2 != 0)
	{
		ILB_IMAGE_DATA* pState = (ILB_IMAGE_DATA*)compareItem.itemData2;
		compareItem.itemData2 = pState->m_dwUserData;
	}
	return CompareItem(&compareItem);
}

void CImageListBox::PreDeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	DELETEITEMSTRUCT deleteItem;
	memcpy(&deleteItem, lpDeleteItemStruct, sizeof(DELETEITEMSTRUCT));

	// WINBUG: The following if block is required because Windows NT
	// version 3.51 does not properly fill out the LPDELETEITEMSTRUCT.
	if (deleteItem.itemData == 0)
	{
		LRESULT lResult = DefWindowProc(LB_GETITEMDATA, deleteItem.itemID, 0);
		if (lResult != LB_ERR)
			deleteItem.itemData = (UINT)lResult;
	}

	if (deleteItem.itemData != 0)
	{
		ILB_IMAGE_DATA* pState = (ILB_IMAGE_DATA*)deleteItem.itemData;
		deleteItem.itemData = pState->m_dwUserData;
		delete pState;
	}
	DeleteItem(&deleteItem);
}

BOOL CImageListBox::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam,
	LRESULT* pResult)
{
	switch (message)
	{
	case WM_DRAWITEM:
		ASSERT(pResult == NULL);       // no return value expected
		PreDrawItem((LPDRAWITEMSTRUCT)lParam);
		break;
	case WM_MEASUREITEM:
		ASSERT(pResult == NULL);       // no return value expected
		PreMeasureItem((LPMEASUREITEMSTRUCT)lParam);
		break;
	case WM_COMPAREITEM:
		ASSERT(pResult != NULL);       // return value expected
		*pResult = PreCompareItem((LPCOMPAREITEMSTRUCT)lParam);
		break;
	case WM_DELETEITEM:
		ASSERT(pResult == NULL);       // no return value expected
		PreDeleteItem((LPDELETEITEMSTRUCT)lParam);
		break;
	default:
		return CListBox::OnChildNotify(message, wParam, lParam, pResult);
	}
	return TRUE;
}

#ifdef _DEBUG
void CImageListBox::PreSubclassWindow()
{
	CListBox::PreSubclassWindow();

	// CImageListBoxes must be owner drawn
	ASSERT(GetStyle() & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE));
}
#endif

int CImageListBox::CalcMinimumItemHeight()
{
	return m_bmSize.cy + 3;
}

int CImageListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ((GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS))
		== (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS))
		SetItemHeight(0, CalcMinimumItemHeight());

	return 0;
}

LRESULT CImageListBox::OnSetFont(WPARAM , LPARAM)
{
	Default();
	return 0;
}

LRESULT CImageListBox::OnLBAddString(WPARAM wParam, LPARAM lParam)
{
	return LB_ERR;
}

LRESULT CImageListBox::OnLBFindString(WPARAM wParam, LPARAM lParam)
{
	return LB_ERR;
}

LRESULT CImageListBox::OnLBFindStringExact(WPARAM wParam, LPARAM lParam)
{
	return LB_ERR;
}

LRESULT CImageListBox::OnLBGetItemData(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc(LB_GETITEMDATA, wParam, lParam);

	if (lResult != LB_ERR)
	{
		ILB_IMAGE_DATA* pState = (ILB_IMAGE_DATA*)lResult;

		if (pState == NULL)
			return 0; // default

		lResult = pState->m_dwUserData;
	}
	return lResult;
}

LRESULT CImageListBox::OnLBSetItemData(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = DefWindowProc(LB_GETITEMDATA, wParam, 0);

	if (lResult != LB_ERR)
	{
		ILB_IMAGE_DATA* pState = (ILB_IMAGE_DATA*)lResult;

		if (pState == NULL)
			pState = new ILB_IMAGE_DATA;

		pState->m_dwUserData = lParam;
		lResult = DefWindowProc(LB_SETITEMDATA, wParam, (LPARAM)pState);

		if (lResult == LB_ERR)
			delete pState;
	}
	return lResult;
}

LRESULT CImageListBox::OnLBGetText(WPARAM wParam, LPARAM lParam)
{
	return LB_ERR;
}

LRESULT CImageListBox::OnLBInsertString(WPARAM wParam, LPARAM lParam)
{
	return LB_ERR;
}

LRESULT CImageListBox::OnLBSelectString(WPARAM wParam, LPARAM lParam)
{
	return LB_ERR;
}

LRESULT CImageListBox::OnLBSetItemHeight(WPARAM wParam, LPARAM lParam)
{
	int nHeight = max(CalcMinimumItemHeight(),(int)LOWORD(lParam));
	return DefWindowProc(LB_SETITEMHEIGHT, wParam, MAKELPARAM(nHeight,0));
}

IMPLEMENT_DYNAMIC(CImageListBox, CListBox)


/////////////////////////////////////////////////////////////////////////////
