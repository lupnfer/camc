//
// ImageListBox.h : interface of the CImageListBox class
//

//
// Author :    GHINI Mauro
//

#ifndef __IMAGELISTBOX_H__
#define __IMAGELISTBOX_H__

/////////////////////////////////////////////////////////////////////////////
// CImageListBox window

class CImageListBox : public CListBox
{
	DECLARE_DYNAMIC(CImageListBox)

// Constructors
public:
	CImageListBox();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
    CImageList* SetImageList(CImageList* pImageList);
    CImageList* GetImageList();
    int AddItem(int nImageIndex, DWORD dwItemData = 0);

// Overridables
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

// Implementation
protected:

    void PreDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void PreMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	int PreCompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	void PreDeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);

	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);

#ifdef _DEBUG
	virtual void PreSubclassWindow();
#endif

	int CalcMinimumItemHeight();

    CImageList* m_pImageList;
    CSize m_bmSize;

	// Message map functions
protected:
	//{{AFX_MSG(CImageListBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBAddString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBFindString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBFindStringExact(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBGetItemData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBInsertString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBSelectString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBSetItemData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBSetItemHeight(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif //__IMAGELISTBOX_H__
