//////////////////////////////////////////////////////////////////
//																//
//		��; : RLE ѹ���㷨	(PCX, TGAͼ��)						//
//		���� : [Foolish] / 2000-9-6								//
//		���� : 2003-2-18										//
//		��ҳ : http://www.crazy-bit.com/						//
//		���� : crazybit@263.net									//
//									(c) 1999 - 2003 =USTC= ����	//
//////////////////////////////////////////////////////////////////
#ifndef		__FOO_RLE_COMPRESS_H__
#define		__FOO_RLE_COMPRESS_H__
#include "../StdDefine.h"
#ifdef __cplusplus
extern "C" {
#endif

//====================================================================
//	RLE - ѹ���㷨
//====================================================================
//////////////////////////////////////////////////////////////////////
//	��  �� ��RLEѹ / ��ѹ�� ( PCX ͼ�� )							//
//	��  �� ��														//
//	����ֵ ��д��OutBuffer�е��ֽ���								//
//	˵  �� ��ѹ��ʱOutBuffer��������InBuffer��С					//
//////////////////////////////////////////////////////////////////////
int  RLE_PCX_Encode (BYTE * InBuffer, int nInSize, BYTE * OutBuffer) ;
int  RLE_PCX_Decode (BYTE * InBuffer, int nInSize, BYTE * OutBuffer) ;
//////////////////////////////////////////////////////////////////////
//	��  �� ��RLEѹ / ��ѹ�� ( TGA ͼ�� )							//
//	��  �� ��iColorBit Ϊ������ɫλ�� - 8, 16, 24, 32				//
//			 iNumPixel Ϊͼ��ÿ�����ظ���							//
//	����ֵ ��EncodeLine ���ر���� OutBuffer ��ָ��					//
//			 DecodeLine ���ؽ���� InBuffer ��ָ��					//
//	˵  �� ��ѹ��ʱOutBuffer��������InBuffer��С					//
//////////////////////////////////////////////////////////////////////
int  RLE_TGA_Encode (BYTE * InBuffer, int nInSize, BYTE * OutBuffer, int nPack = 8) ;
void  RLE_TGA_Decode (BYTE * InBuffer, int nOriSize, BYTE * OutBuffer, int nPack = 8) ;
BYTE * RLE_TGA_EncodeLine (BYTE * InBuffer, int iColorBit,
						   int iNumPixel,  // iNumPixel��Ϊͼ���
						   BYTE * OutBuffer) ;
BYTE * RLE_TGA_DecodeLine (BYTE * InBuffer, int iColorBit,
						   int iNumPixel,  // iNumPixel��Ϊͼ���
						   BYTE * OutBuffer) ;

//===================================================================

#ifdef __cplusplus
}
#endif
#endif
