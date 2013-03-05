#pragma once


// FCLayerBar 对话框
#include"IconListBox.h"
#include"ObjCanvas.h"
#include"ObjImage.h"
#include "Resource.h"
class FCLayerBar : public CBCGPDialogBar
{
	DECLARE_DYNAMIC(FCLayerBar)

public:
	FCLayerBar(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~FCLayerBar();
	virtual BOOL Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID);
// 对话框数据
	enum { IDD = IDD_LAYERDLG };
	CIconListBox m_ListBox;
public:
	BOOL InitDialogBar();
	void InitImageList();
	void ClearImageList();
	void Add(FCObjLayer& img);//加入一个图层
	void Add(CString str);
	void Delete(int index);	
	void RePaint();
	void SetCanvas(FCObjCanvas* pObjCanvas){m_pObjCanvas=pObjCanvas;}
	FCObjCanvas* GetCanvas(){ return m_pObjCanvas;}
	void FlushLayerThumbnail(int i);
private:
	CImageList m_imgNormal;
	FCObjCanvas* m_pObjCanvas;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeIconlistbox();
};
