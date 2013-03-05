#pragma once
#include "Objbase.h"
#include "ObjImage.h"
class FCObjLayer ;

class FCCmdSmartLayerBase :
	public FCCmdArtPrider
{
public :
	FCCmdSmartLayerBase () : m_pLayer(NULL), m_pCanvas(NULL), m_bSaveAll(TRUE) {}
	virtual void  Execute (FCObjCanvas & canvas, FCObjProgress * Percent = NULL) ;
	virtual void  Undo () ;
	virtual void  Redo () ;
	virtual void  Implement (FCObjImage & img, FCObjProgress * Percent = NULL) {}
protected :
	virtual void  QuerySaveRect (RECT * prcSave) const ;
	virtual void  OnPrepareBlockRect (const RECT & rcBlock) {} // �յ�blockλ��ͼ���λ��
	virtual void  OnAfterGrowLayer (int nLeft, int nTop, int nRight, int nBottom) {} ;
public:
	FCObjLayer		* m_pLayer ;
protected :
	FCObjCanvas		* m_pCanvas ;
	FCObjImage		m_Undo ;
	FCObjImage		m_Redo ;
	BOOL			m_bSaveAll ; // �Ƿ񱣴�������ͼ��
	RECT			m_rcSave ; // m_pLayer���������
};
