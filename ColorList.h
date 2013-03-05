#pragma once
#include "afxwin.h"

class CColorList :
	public CListBox
{
public:
	CColorList(void);
	~CColorList(void);
public:
	void AddColorItem(COLORREF color);
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
protected:
	DECLARE_MESSAGE_MAP()

};
