//////////////////////////////////////////////////////////////////
//																//
//		用途 : System Independent								//
//		创建 : [Foolish] / 1999-11-6							//
//		更新 : 2002-1-15										//
//		主页 : http://www.crazy-bit.com/						//
//		邮箱 : crazybit@263.net									//
//								    (c) 1999 - 2002 =USTC= 付黎	//
//////////////////////////////////////////////////////////////////
#ifndef	 __FOO_SYSINDEPEND_H_
#define	 __FOO_SYSINDEPEND_H_
#include "ObjBase.h"

//===================================================================
	
	/************************************************************/
	/*	功  能 ：位操作											*/
	/*	参  数 ：Pos : 从低位0算起的位置						*/
	/*	返回值 ：if 1 , return true								*/
	/*			 if 0 , return false							*/
	/************************************************************/
	template <class T>	bool  fooTestBit (const T & Test, int Pos) ;
	template <class T>	void  fooSetBit (T * Test, int Pos) ;
	template <class T>	int   fooCountNum_1 (const T & Test) ; // 统计1的个数
	template <class T>	void  fooClearBit (T * Test, int Pos) ;
	template <class T>	void  fooFlipBit (T * Test, int Pos) ;

	/************************************************************/
	/*	功  能 ：使Test饱和在[low,high]							*/
	/************************************************************/
	template <class T>	void  fooBound (T * Test, T low, T high) ;
	template <class T>	 T    fooBound (const T & Test, T low, T high) ;

	/************************************************************/
	/*	功  能 ：初始化结构为0									*/
	/*	参  数 ：bIniHead : 第一个DWORD是否设为该结构的大小		*/
	/************************************************************/
	template <class T>
	void  fooInitStruct (T * structure, bool bIniHead) ;

	/************************************************************/
	/*	功  能 ：填充iNum个结构到pDest							*/
	/*	参  数 ：iNum 为填充 structure 的个数					*/
	/************************************************************/
	template <class T>
	void  fooFillStruct (BYTE * pDest, const T & pStructure, int iNum) ;

	/************************************************************/
	/*	功  能 ：pMem指向的内存倒转, 类_strrev					*/
	/*	参  数 ：iNum 为 T 的个数								*/
	/************************************************************/
	template <class T>
	void  fooMemReverse (T * pMem, int iNum) ;

	/************************************************************/
	/*	功  能 ：产生随机数 (包括iLow, iHigh)					*/
	/************************************************************/	
	int  fooRandom (int iLow, int iHigh) ;

	/************************************************************/
	/*	功  能 ：位内存拷贝										*/
	/*	参  数 ：ixxxPos为当前byte的bit位置, 按如下排列			*/
	/*			 <8 7 6 5 4 3 2 1> <8(0) 7 6 ......				*/
	/*			 iBitLen 为要复制的Bit长度						*/
	/*	说  明 ：pDest必须预先置0								*/
	/************************************************************/
	void  fooBitMemoryCopy (BYTE * pDest, int iDestPos,
							BYTE * pSrc, int iSrcPos,
							int iBitLen) ;

	/************************************************************/
	/*	功  能 ：冒泡排序										*/
	/*	参  数 ：iNumElement为数组元素个数						*/
	/*	说  明 ：Array[]输出按由小到大排列						*/
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
	if (iDestPos == iSrcPos) // S & D对齐
	{
		if (iSrcPos >= iBitLen) // 不足1BYTE
			*pDest |= (*pSrc & (0xFF >> (8 - iSrcPos))) & (0xFF << (iSrcPos - iBitLen)) ; // 左右清零
		else
		{
			*pDest++ |= *pSrc++ & (0xFF >> (8 - iSrcPos)) ; // 左清零OR
			iBitLen -= iSrcPos ;
			DWORD	dwNumByte = iBitLen / 8 ;
			if (dwNumByte != 0)
				memcpy (pDest, pSrc, dwNumByte) ; // 整BYTE拷贝
			if ((iBitLen %= 8) != 0) // 剩余bit拷贝
			{
				pDest += dwNumByte ; pSrc += dwNumByte ;
				*pDest |= *pSrc & (0xFF << (8 - iBitLen)) ;
			}
		}
	}
	else // S & D不对齐
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
			if (iSrcPos >= iBitLen) // 不足1BYTE
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
					memcpy (pDest, pSrc, dwNumByte) ; // 整BYTE拷贝
				
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
