#pragma once


// FCLayerBar �Ի���
#include"IconListBox.h"
#include"ObjCanvas.h"
#include"ObjImage.h"
#include "Resource.h"
class FCLayerBar : public CBCGPDialogBar
{
	DECLARE_DYNAMIC(FCLayerBar)

public:
	FCLayerBar(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~FCLayerBar();
	virtual BOOL Create(CWnd* pParentWnd,UINT nIDTemplate,UINT nStyle,UINT nID);
// �Ի�������
	enum { IDD = IDD_LAYERDLG };
	CIconListBox m_ListBox;
public:
	BOOL InitDialogBar();
	void InitImageList();
	void ClearImageList();
	void Add(FCObjLayer& img);//����һ��ͼ��
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeIconlistbox();
};
