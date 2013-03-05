#include "stdafx.h"
#include "Rle.h"

//===================================================================
int  RLE_PCX_Decode (BYTE * InBuffer, int nInSize, BYTE * OutBuffer)
{
	const BYTE    * pOutStart = OutBuffer ;
	while (nInSize-- > 0)
	{
		const BYTE    byData = *InBuffer++ ; // read byte and move ptr to next
		if ( (byData & 0xC0) == 0xC0 ) // Data >= 0xC0
		{
			const BYTE    cNum = byData & 0x3F ; // repeat current byte Num
			::memset (OutBuffer, *InBuffer++, cNum) ; // memset func will check "Num" =? 0
			OutBuffer += cNum ;
			nInSize-- ;
		}
		else
			*OutBuffer++ = byData ;
	}
	return (OutBuffer - pOutStart) ;
}
//===================================================================
int  RLE_PCX_Encode (BYTE * InBuffer, int nInSize, BYTE * OutBuffer)
{
	const BYTE    * pOutStart = OutBuffer ;
	while (nInSize-- > 0)
	{
              BYTE    cCount = 1 ;
        const BYTE    byData = *InBuffer++ ;
		while ( (cCount < 0x3F) && (nInSize != 0) )
			if (*InBuffer != byData) // 统计重复字节个数
				break ;
			else
			{
				cCount++ ; InBuffer++ ; nInSize-- ;
			}

		if (cCount == 1) // 无重复像素
        {
			if ( (byData & 0xC0) == 0xC0 ) // Data >= 0xC0
			{
				*OutBuffer++ = 0xC1 ;
				*OutBuffer++ = byData ;
			}
			else
				*OutBuffer++ = byData ; // Data < 0xC0, 直接写字节
        }
		else // 重复像素
		{
			*OutBuffer++ = 0xC0 | cCount ;
			*OutBuffer++ = byData ;
		}
	}
	return (OutBuffer - pOutStart) ;
}
//===================================================================
BYTE * RLE_TGA_DecodeLine (BYTE * InBuffer, int iColorBit, int iNumPixel, BYTE * OutBuffer)
{
	iColorBit /= 8 ; // 转换为字节数 1,2,3,4
	while (iNumPixel > 0)
	{
		const BYTE    byData = *InBuffer++ ; // Next Byte
		if (byData & 0x80) // Data >= 0x80
		{
			int    Num = (byData & 0x7F) + 1 ; // number of repeat pixel
			iNumPixel -= Num ;
			for (int i = 0 ; i < Num ; i++, OutBuffer += iColorBit)
				::memcpy (OutBuffer, InBuffer, iColorBit) ;
			InBuffer += iColorBit ;
		}
		else // 直接copy Num个像素
		{
			int    Num = byData + 1 ; // non-repeat pixel
			iNumPixel -= Num ;
			Num *= iColorBit ; // 这里转为字节数
			::memcpy (OutBuffer, InBuffer, Num) ;
			OutBuffer += Num ;
			InBuffer += Num ;
		}
	} // End of while
	return InBuffer ;
}
//===================================================================
BYTE * RLE_TGA_EncodeLine (BYTE * InBuffer, int iColorBit, int iNumPixel, BYTE * OutBuffer)
{
	iColorBit /= 8 ; // 转换为字节数 1,2,3,4
	while (iNumPixel > 0)
	{
		const BYTE    * pBak = InBuffer ; // 记数指针
		DWORD         Data = 0, Next = 0, Count = 1 ;
		::memcpy (&Data, InBuffer, iColorBit) ; // 第一个像素
		InBuffer += iColorBit ; iNumPixel-- ;
		while ((Count < 0x80) && (iNumPixel > 0)) // 统计重复像素
		{
			::memcpy (&Next, InBuffer, iColorBit) ; // 下一个像素
			if (Next != Data)
				break ;
			InBuffer += iColorBit ; iNumPixel-- ; Count++ ;
		}

		if (Count == 1) // 无重复像素
		{
			while ((Count < 0x80) && (iNumPixel > 0)) // 统计不重复像素
			{
				Count++ ; Data = Next ;
				InBuffer += iColorBit ; iNumPixel-- ;
				::memcpy (&Next, InBuffer, iColorBit) ; // 下一个像素
				if (Data == Next)
					break ;
			}
			// 直接copy不重复像素
			*OutBuffer++ = (BYTE)(Count - 1) ;
			Count = InBuffer - pBak ; // Count->临时变量
			::memcpy (OutBuffer, pBak, Count) ;
			OutBuffer += Count ;
		}
		else // 重复像素
		{
			*OutBuffer++ = 0x80 | (BYTE)--Count ;
			::memcpy (OutBuffer, &Data, iColorBit) ;
			OutBuffer += iColorBit ;
		}		
	} // End of while
	return OutBuffer ;
}
//===================================================================
int  RLE_TGA_Encode (BYTE * InBuffer, int nInSize, BYTE * OutBuffer, int nPack)
{
    if (((nInSize * 8) % nPack) != 0)
        return 0 ;
    return ::RLE_TGA_EncodeLine (InBuffer, nPack, nInSize * 8 / nPack, OutBuffer) - OutBuffer ;
}
void  RLE_TGA_Decode (BYTE * InBuffer, int nOriSize, BYTE * OutBuffer, int nPack)
{
    ::RLE_TGA_DecodeLine (InBuffer, nPack, nOriSize * 8 / nPack, OutBuffer) ;
}
//===================================================================
