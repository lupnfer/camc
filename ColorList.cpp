#include "StdAfx.h"
#include ".\colorlist.h"
#define COLOR_ITEM_HEIGHT   100

BEGIN_MESSAGE_MAP(CColorList, CListBox)
	//{{AFX_MSG_MAP(CColorList)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
CColorList::CColorList(void)
{
}

CColorList::~CColorList(void)
{
}


void CColorList::AddColorItem(COLORREF color)
{
	// add a listbox item

	AddString((LPCTSTR) color);
	// Listbox does not have the LBS_HASSTRINGS style, so the
	//  normal listbox string is used to store an RGB color
}
void CColorList::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	// all items are of fixed size
	// must use LBS_OWNERDRAWVARIABLE for this to work
	lpMIS->itemHeight = COLOR_ITEM_HEIGHT;
}
void CColorList::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	COLORREF cr = (COLORREF)lpDIS->itemData; // RGB in item data

	if (lpDIS->itemAction & ODA_DRAWENTIRE)
	{
		// Paint the color item in the color requested
		CBrush br(cr);
		pDC->FillRect(&lpDIS->rcItem, &br);
	}

	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		// item has been selected - hilite frame
		COLORREF crHilite = RGB(255-GetRValue(cr),
			255-GetGValue(cr), 255-GetBValue(cr));
		CBrush br(crHilite);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}

	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
	{
		// Item has been de-selected -- remove frame
		CBrush br(cr);
		pDC->FrameRect(&lpDIS->rcItem, &br);
	}

}