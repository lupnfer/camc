//////////////////////////////////////////////////////////////////
//																//
//		��; : ����0�ľ�̬Huffmanѹ���㷨						//
//		���� : [Foolish] / 2001-3-6								//
//		���� : 2002-1-12										//
//		��ҳ : http://www.crazy-bit.com/					//
//		���� : crazybit@263.net									//
//									(c) 1999 - 2002 =USTC= ����	//
//////////////////////////////////////////////////////////////////
#ifndef		__FOO_HUFF_COMPRESS_H__
#define		__FOO_HUFF_COMPRESS_H__
#include "../StdDefine.h"

//	Ҷ�ڵ�child_0 == 0xFFFF
typedef struct tagHUFF_NODE
{
	BYTE	active ;  // �Ƿ�Ϊ��ڵ�
	BYTE	number ;  // ֻ��Ҷ�ڵ����
	DWORD	count ;	  // ͳ�Ƽ���
	WORD	child_0 ; // 0-bit ֧
	WORD	child_1 ; // 1-bit ֧
	WORD	reserve ; // filled, DWORD����
} HUFF_NODE ;	// Total = 12-Byte

//  ӳ�����
typedef struct tagHUFF_CODE
{
	DWORD	code ;	// 32Bit - Code
	DWORD	bit_length ;
} HUFF_CODE ;

//===================================================================
//	Huffman - ѹ���㷨
//===================================================================
////////////////////////////////////////////////////////////
//	��  �� ��ͳ����DataBuffer��0x00--0xFFÿ���ַ����ֵĸ���
//	��  �� ��ͳ�Ƽ������� Count_Array[256] ��
//	����ֵ ��
//	˵  �� ��
////////////////////////////////////////////////////////////
void  Huff_Count (BYTE * pDataBuffer, DWORD dwBufferLength,
									  DWORD Count_Array[256]) ; // <-- ͳ�Ƽ���
////////////////////////////////////////////////////////////
//	��  �� ���ҳ�������С��������
//	��  �� ��min �з��������е�λ��
//	����ֵ ��Return false ���޻�ڵ�
//	˵  �� ��
////////////////////////////////////////////////////////////
bool  __fooSearchMin  (HUFF_NODE Node_Array[513], int * min) ;
bool  __fooSearchMin2 (HUFF_NODE Node_Array[513], int * min_1, int * min_2) ;
////////////////////////////////////////////////////////////
//	��  �� ������ huffman ��
//	��  �� ��
//	����ֵ ������ root ��λ��
//	˵  �� ��
////////////////////////////////////////////////////////////
DWORD  Huff_Build_Tree (DWORD		Count_Array[256], 
						HUFF_NODE	Node_Array [512]) ; // <--��
////////////////////////////////////////////////////////////
//	��  �� ������ӳ���
//	��  �� ��
//	����ֵ ��
//	˵  �� ��
////////////////////////////////////////////////////////////
void  Huff_Create_Table (HUFF_NODE	Node_Array[512],  // <--Huffman��
						 HUFF_CODE	Code_Array[256],  // <--ӳ������ڴ�
						 DWORD		code_walk,  // init == 0
						 WORD		bit_walk,	// init == 0
						 DWORD		root) ;		// ����λ��
////////////////////////////////////////////////////////////
//	��  �� ��Huffman ѹ��
//	��  �� ��
//	����ֵ ������д��OutBuffer���ֽ���
//	˵  �� ��ʹ��ǰ����� OutBuffer �� 0 
////////////////////////////////////////////////////////////
DWORD  Huff_Encode (BYTE * InBuffer, DWORD dwInSize, BYTE * OutBuffer,
					BYTE * WriteBit = NULL) ;
////////////////////////////////////////////////////////////
//	��  �� ��Huffman ��ѹ��
//	��  �� ��dwOutLengthΪѹ��ǰ����ĳ���
//	����ֵ ��
//	˵  �� ��ʹ��ǰ����� OutBuffer �� 0 
////////////////////////////////////////////////////////////
void  Huff_Decode (BYTE * InBuffer, DWORD Count_Array[256],
				   BYTE * OutBuffer,
				   DWORD  dwOutLength) ;	// ѹ��ǰ����ĳ��� (�ļ���С)

//===================================================================
//	Implement
//===================================================================
inline void  Huff_Count (BYTE * DataBuffer, DWORD BufferLength, DWORD Count_Array[256]) {
	::memset (Count_Array, 0, 256 * sizeof(DWORD)) ;
	for (DWORD count = 0 ; count < BufferLength ; count++)
		Count_Array[DataBuffer[count]]++ ;
	return ;
}
//	Temporary : search count��͵������ڵ�, ����Ϊ���
inline bool  __fooSearchMin2 (HUFF_NODE Node_Array[512], int * min_1, int * min_2) {
	bool	result = __fooSearchMin (Node_Array, min_1) ;
	result &= __fooSearchMin (Node_Array, min_2) ;
	return result ;
}
//	Temporary : search count ��͵Ľڵ�, ����Ϊ���
//				Return false ���޻�ڵ�
inline bool  __fooSearchMin (HUFF_NODE Node_Array[512], int * min) {
	int		iIndex, result = -1 ;
	DWORD	count ;
	count = 0xFFFFFFFF ;
	for (iIndex = 0 ; iIndex < 512 ; iIndex++)
		if ( (Node_Array[iIndex].active == 1) && (Node_Array[iIndex].count < count) )
		{
			count = Node_Array[iIndex].count ;
			result = iIndex ;
		}
	if (result == -1)
		return false ;
	Node_Array[result].active = 0 ;	// set non-active
	*min = result ;
	return true ;
}

#endif
