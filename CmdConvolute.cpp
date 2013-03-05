#include "StdAfx.h"
#include "ObjCanvas.h"
#include "ObjProgress.h"
#include ".\cmdimgeffect.h"
//=================================================================================
// ͼ��������/image convolute (>= 24 bit)
FCCmdImgConvolute::FCCmdImgConvolute (int * nElements, int iBlockLen, int iDivisor, int nOffset)
{
	if (nElements == NULL)
	{
		m_iBlock = 0 ; m_iDivisor = 1 ; m_nOffset = 1 ;
		return ;
	}
	for (int i=0 ; i < FSquare(iBlockLen) ; i++)
		m_Element.push_back (nElements[i]) ;
	m_iBlock = iBlockLen ;
	m_iDivisor = iDivisor ;
	m_nOffset = nOffset ;
}
// ͼ��������/image convolute (>= 24 bit)
void FCCmdImgConvolute::Implement (FCObjImage & img, FCObjProgress * Percent)
{
	if (!img.IsValidImage() || (img.ColorBits() < 24) || (m_iBlock < 3) ||
		(m_iBlock >= img.Width()) || (m_iBlock >= img.Height()) || (m_iDivisor == 0))
		return ;

	// �ȸ��Ʊ߽磬���ڴ���
	int			nLeftTop = m_iBlock/2, // ������չ
				nRightDown = nLeftTop - ((m_iBlock % 2)^1) ; // ������չ
	FCObjImage		Old(img) ; // ����ԭͼ
	Old.ExpandFrame (TRUE, nLeftTop, nLeftTop, nRightDown, nRightDown) ;

	// �������¿�ʼ����
	int			nSpan = img.ColorBits() / 8 ; // 3, 4
	DWORD		dwPitch = Old.GetPitch() + m_iBlock * nSpan ;
	for (int y=0 ; y < img.Height() ; y++)
	{
		BYTE	* pOldStart = Old.GetBits (y) ;
		BYTE	* pWrite = img.GetBits (y) ;
		for (int x=0 ; x < img.Width() ; x++, pOldStart += nSpan, pWrite += nSpan)
		{
			// �����ӿ��
			BYTE	* pCurr = pOldStart ;
			int		dwSumR=0, dwSumG=0, dwSumB=0, i=0 ;
			for (int sY=0 ; sY < m_iBlock ; sY++, pCurr -= dwPitch)
				for (int sX=0 ; sX < m_iBlock ; sX++, pCurr += nSpan, i++)
				{
					if (m_Element[i] == 0)
						continue ;
					if (pCurr[0] != 0)  dwSumB += pCurr[0] * m_Element[i] ;
					if (pCurr[1] != 0)  dwSumG += pCurr[1] * m_Element[i] ;
					if (pCurr[2] != 0)  dwSumR += pCurr[2] * m_Element[i] ;
				}
			// ��������ֵ
			pWrite[0] = FClamp0255 (m_nOffset + dwSumB / m_iDivisor) ;
			pWrite[1] = FClamp0255 (m_nOffset + dwSumG / m_iDivisor) ;
			pWrite[2] = FClamp0255 (m_nOffset + dwSumR / m_iDivisor) ;
		}
		if (Percent != NULL)
			Percent->SetProgress (y * 100 / img.Height()) ;
	}
}
