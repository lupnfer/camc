//////////////////////////////////////////////////////////////////
//																//
//		��; : System Independent								//
//		���� : [Foolish] / 1999-11-6							//
//		���� : 2002-1-15										//
//		��ҳ : http://www.crazy-bit.com/						//
//		���� : crazybit@263.net									//
//								    (c) 1999 - 2002 =USTC= ����	//
//////////////////////////////////////////////////////////////////
#ifndef	 __FOO_SYSINDEPEND_H_
#define	 __FOO_SYSINDEPEND_H_
#include "ObjBase.h"

//===================================================================
	
	/************************************************************/
	/*	��  �� ��λ����											*/
	/*	��  �� ��Pos : �ӵ�λ0�����λ��						*/
	/*	����ֵ ��if 1 , return true								*/
	/*			 if 0 , return false							*/
	/************************************************************/
	template <class T>	bool  fooTestBit (const T & Test, int Pos) ;
	template <class T>	void  fooSetBit (T * Test, int Pos) ;
	template <class T>	int   fooCountNum_1 (const T & Test) ; // ͳ��1�ĸ���
	template <class T>	void  fooClearBit (T * Test, int Pos) ;
	template <class T>	void  fooFlipBit (T * Test, int Pos) ;

	/************************************************************/
	/*	��  �� ��ʹTest������[low,high]							*/
	/************************************************************/
	template <class T>	void  fooBound (T * Test, T low, T high) ;
	template <class T>	 T    fooBound (const T & Test, T low, T high) ;

	/************************************************************/
	/*	��  �� ����ʼ���ṹΪ0									*/
	/*	��  �� ��bIniHead : ��һ��DWORD�Ƿ���Ϊ�ýṹ�Ĵ�С		*/
	/************************************************************/
	template <class T>
	void  fooInitStruct (T * structure, bool bIniHead) ;

	/************************************************************/
	/*	��  �� �����iNum���ṹ��pDest							*/
	/*	��  �� ��iNum Ϊ��� structure �ĸ���					*/
	/************************************************************/
	template <class T>
	void  fooFillStruct (BYTE * pDest, const T & pStructure, int iNum) ;

	/************************************************************/
	/*	��  �� ��pMemָ����ڴ浹ת, ��_strrev					*/
	/*	��  �� ��iNum Ϊ T �ĸ���								*/
	/************************************************************/
	template <class T>
	void  fooMemReverse (T * pMem, int iNum) ;

	/************************************************************/
	/*	��  �� ����������� (����iLow, iHigh)					*/
	/************************************************************/	
	int  fooRandom (int iLow, int iHigh) ;

	/************************************************************/
	/*	��  �� ��λ�ڴ濽��										*/
	/*	��  �� ��ixxxPosΪ��ǰbyte��bitλ��, ����������			*/
	/*			 <8 7 6 5 4 3 2 1> <8(0) 7 6 ......				*/
	/*			 iBitLen ΪҪ���Ƶ�Bit����						*/
	/*	˵  �� ��pDest����Ԥ����0								*/
	/************************************************************/
	void  fooBitMemoryCopy (BYTE * pDest, int iDestPos,
							BYTE * pSrc, int iSrcPos,
							int iBitLen) ;

	/************************************************************/
	/*	��  �� ��ð������										*/
	/*	��  �� ��iNumElementΪ����Ԫ�ظ���						*/
	/*	˵  �� ��Array[]�������С��������						*/
	/************************************************************/
	template <class T>
	void  fooBubbleSort (T Array[], int iNumElement) ;

//===================================================================
template <class T>	inline bool  fooTestBit (const T & Test, int Pos) {
	return (Test & (1 << Pos)) ? true : false ;
}
template <class T>	inline int  fooCountNum_1 (const T & Test) {
	register int		iBitLen = sizeof(Test) << 3 ;
	register int		iCount = 0 ;
	T					iCurr = 1 ;
	while (iBitLen-- > 0) {
		if (Test & iCurr)
			iCount++ ;
		iCurr <<= 1 ;
	}
	return iCount ;
}
template <class T>	inline void  fooSetBit (T * Test, int Pos) {
	if (Test != NULL)
		*Test |= 1 << Pos ;
}
template <class T>	inline void  fooClearBit (T * Test, int Pos) {
	if (Test != NULL)
		*Test &= ~(1 << Pos) ;
}
template <class T>	inline void  fooFlipBit (T * Test, int Pos) {
	if (Test != NULL)
		*Test ^= 1 << Pos ;
}
template <class T>	inline void  fooBound (T * Test, T low, T high) {
	if (Test != NULL)
		*Test = FMax (FMin (*Test, high), low) ;
}
template <class T>	inline T  fooBound (const T & Test, T low, T high) {
	return (T) FMax (FMin (Test, high), low) ;
}
template <class T>	inline void  fooInitStruct (T * structure, bool bIniHead) {
	if (structure != NULL) {
		memset (structure, 0, sizeof(T)) ;
		if (bIniHead)	* (DWORD *) structure = sizeof(T) ;
	}
}
template <class T>  inline void  fooFillStruct (BYTE * pDest, const T & structure, int iNum) {
	for (int i=0 ; i < iNum ; i++, pDest += sizeof(T)) {
			memcpy (pDest, &structure, sizeof(T)) ;
		}
}
template <class T>  inline void  fooMemReverse (T * pMem, int iNum) {
	T		temp ;
	if ((pMem != NULL) && (iNum > 1))
		while (iNum > 1) {
			temp = *pMem ;
			*pMem = pMem[--iNum] ;
			pMem[iNum--] = temp ;
			pMem++ ;
		}
}
//===================================================================
template <class T>
inline void  fooBubbleSort (T Array[], int iNumElement) {
	register bool	flag ;
		T			temp ;
	for (int i = iNumElement - 1 ; i > 0 ; i--)
	{
		flag = true ;
		for (int j = 0 ; j < i ; j++)
			if (Array[j] > Array[j + 1])
			{
				temp = Array[j] ;
				Array[j] = Array[j + 1] ;
				Array[j + 1] = temp ;
				flag = false ;
			}
		if (flag)
			break ;
	}
}
//===================================================================
inline int  fooRandom (int iLow, int iHigh) {
	return fooBound (iLow + rand () * (iHigh - iLow) / RAND_MAX, iLow, iHigh) ;
}
//===================================================================
inline void  fooBitMemoryCopy (BYTE * pDest, int iDestPos,
							   BYTE * pSrc, int iSrcPos,
							   int iBitLen)
{
	if (iDestPos == iSrcPos) // S & D����
	{
		if (iSrcPos >= iBitLen) // ����1BYTE
			*pDest |= (*pSrc & (0xFF >> (8 - iSrcPos))) & (0xFF << (iSrcPos - iBitLen)) ; // ��������
		else
		{
			*pDest++ |= *pSrc++ & (0xFF >> (8 - iSrcPos)) ; // ������OR
			iBitLen -= iSrcPos ;
			DWORD	dwNumByte = iBitLen / 8 ;
			if (dwNumByte != 0)
				memcpy (pDest, pSrc, dwNumByte) ; // ��BYTE����
			if ((iBitLen %= 8) != 0) // ʣ��bit����
			{
				pDest += dwNumByte ; pSrc += dwNumByte ;
				*pDest |= *pSrc & (0xFF << (8 - iBitLen)) ;
			}
		}
	}
	else // S & D������
	{
		register DWORD	dwTmp ;
		register BYTE	bySub = abs (iSrcPos - iDestPos) ;
		while ((iBitLen -= 24) > 0)
		{
			dwTmp = * (DWORD *) pSrc ;
			dwTmp |= (dwTmp & (0xFFFFFFFF >> (8 - iSrcPos))) & (0xFFFFFFFF << iSrcPos) ;
			if (iSrcPos > iDestPos)
				* (DWORD *) pDest |= dwTmp >> bySub ;
			else
				* (DWORD *) pDest |= dwTmp << bySub ;
			pSrc += 3 ; pDest += 3 ;
		}
		iBitLen += 24 ;
		while (iBitLen-- > 0)
		{
			if (::fooTestBit (*pSrc, iSrcPos - 1))
				::fooSetBit (pDest, iDestPos - 1) ;
			if (--iSrcPos == 0)
			{	iSrcPos = 8 ; pSrc++ ;	}
			if (--iDestPos == 0)
			{	iDestPos = 8 ; pDest++ ;	}
		}
	}
/*	register BYTE	byTmp;
	while (iBitLen != 0)
		if (iDestPos == iSrcPos)
			if (iSrcPos >= iBitLen) // ����1BYTE
			{
				*pDest |= (*pSrc & (0xFF >> (8 - iSrcPos))) & (0xFF << (iSrcPos - iBitLen)) ;
				return ;
			}
			else
			{
				*pDest++ |= *pSrc++ & (0xFF >> (8 - iSrcPos)) ;
				iBitLen -= iSrcPos ;
				DWORD	dwNumByte = iBitLen / 8 ;
				if (dwNumByte)
					memcpy (pDest, pSrc, dwNumByte) ; // ��BYTE����
				
				iBitLen -= dwNumByte * 8 ;
				pDest += dwNumByte ; pSrc += dwNumByte ;
				*pDest |= *pSrc & (0xFF << (8 - iBitLen)) ;
			}
		else
			if (iSrcPos <= iBitLen)
			{
				if (iDestPos > iSrcPos)
				{
					byTmp = *pSrc++ ;
					byTmp <<= 8 - iSrcPos ;
					byTmp >>= 8 - iDestPos ;
					*pDest |= byTmp ;
					iDestPos -= iSrcPos ;
				}
				else
				{
					byTmp = *pSrc ;
					byTmp <<= 8 - iSrcPos ;
					byTmp >>= 8 - iDestPos ;
					*pDest++ |= byTmp ;
					byTmp = *pSrc++ ;
					byTmp <<= 8 - (iSrcPos - iDestPos) ;
					*pDest |= byTmp ;
					iDestPos = 8 - (iSrcPos - iDestPos) ;
				}
				iSrcPos = 8 ;
				iBitLen -= iSrcPos ;
			}
			else // iSrcPos > iBitLen
			{
				if (iDestPos >= iBitLen)
				{
					byTmp = *pSrc ;
					byTmp <<= 8 - iSrcPos ;
					byTmp >>= 8 - iBitLen ;
					byTmp <<= iDestPos - iBitLen ;
					*pDest |= byTmp ;
				}
				else
				{
					byTmp = *pSrc ;
					byTmp <<= 8 - iSrcPos ;
					byTmp >>= 8 - iDestPos ;
					*pDest++ |= byTmp ;
					byTmp = *pSrc++ ;
					byTmp >>= iSrcPos - iBitLen ;
					byTmp <<= 8 - (iBitLen - iDestPos) ;
					*pDest |= byTmp ;
				}
				iBitLen = 0 ;
			}
*/}	

//===================================================================

#endif
