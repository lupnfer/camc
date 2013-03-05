#include "stdafx.h"
#include "fooBit.h"
#include "Huffman.h"


//===================================================================

// 2 . ����huffman��, ����root��λ��
DWORD  Huff_Build_Tree (DWORD		Count_Array[256], 
						HUFF_NODE	Node_Array [512])
{
	DWORD	dwTemp ;
	int		min_1, min_2 ;

	//	��ʼ��Ҷ�ڵ�
	for (dwTemp = 0 ; dwTemp < 256 ; dwTemp++)
		if (Count_Array[dwTemp] != 0)
		{
			Node_Array[dwTemp].active  = 1 ;	// Ϊ��ڵ�
			Node_Array[dwTemp].number  = (BYTE) dwTemp ;	// ֵ
			Node_Array[dwTemp].count   = Count_Array[dwTemp] ;	// ͳ�Ƽ���
			Node_Array[dwTemp].child_0 = 0xFFFF ;
		}
		else
			Node_Array[dwTemp].active = 0 ;	// Ϊ����ڵ�

	dwTemp = 256 ;  // ָ��Node_Array�Ŀ�λ��
	while (true)
	{
		if ( !__fooSearchMin2 (Node_Array, &min_1, &min_2) )
			break ;
		Node_Array[dwTemp].child_0 = min_1 ;
		Node_Array[dwTemp].child_1 = min_2 ;
		Node_Array[dwTemp].count   = Node_Array[min_1].count + Node_Array[min_2].count ;
		Node_Array[dwTemp].active  = 1 ;	 // ��ڵ�
		dwTemp++ ;
	}
	return (--dwTemp) ;
}

//===================================================================

// 3 . ����ӳ���
void  Huff_Create_Table (HUFF_NODE Node_Array[512], HUFF_CODE Code_Array[256],
						 DWORD code_walk,	// init == 0
						 WORD  bit_walk,	// init == 0
						 DWORD root)
{
	if (Node_Array[root].child_0 == 0xFFFF)	// ����Ҷ�ڵ�
	{
		Code_Array[root].code		= code_walk ;
		Code_Array[root].bit_length = bit_walk ;
		return ;
	}

	code_walk <<= 1 ;
	bit_walk++ ;
	Huff_Create_Table (Node_Array, Code_Array, code_walk,
					   bit_walk, Node_Array[root].child_0) ;
	Huff_Create_Table (Node_Array, Code_Array, code_walk | 1,
					   bit_walk, Node_Array[root].child_1) ;
	return ;
}

//===================================================================

DWORD  Huff_Encode (BYTE * InBuffer, DWORD dwInSize,
					BYTE * OutBuffer, BYTE * WriteBit)
{
	HUFF_NODE	Node_Array [512] ;
	HUFF_CODE	Code_Array[256] ;
	DWORD		Count_Array[256] ;
	DWORD		dwRoot ;

	Huff_Count (InBuffer, dwInSize, Count_Array) ;
	dwRoot = Huff_Build_Tree (Count_Array, Node_Array) ;
	Huff_Create_Table (Node_Array, Code_Array, 0, 0, dwRoot) ;

	BYTE	* pOldOut = OutBuffer ;
	BYTE	BitPos  = 8 ; // Out Bit Pointer
	BYTE	code ;
	DWORD	index = 0 ;	// In Buffer index
	DWORD	dwTmp ;
	register BYTE	byLeftBit ;
	while (index < dwInSize)
	{
		code = InBuffer[index++] ;

		byLeftBit = (BYTE)Code_Array[code].bit_length ;
		while (byLeftBit > BitPos)
		{
			dwTmp = Code_Array[code].code ;
			dwTmp >>= byLeftBit - BitPos ;
			*OutBuffer++ |= (BYTE)dwTmp ;
			byLeftBit -= BitPos ;
			BitPos = 8 ;
		}

		BYTE	byTmp = (BYTE)Code_Array[code].code ;
		byTmp <<= BitPos - byLeftBit ;
		*OutBuffer |= byTmp ;
		BitPos -= byLeftBit ;
		if (BitPos == 0)
		{
			OutBuffer++ ;
			BitPos = 8 ;
		}
	}
	if (WriteBit != NULL)
		*WriteBit = 8 - BitPos ;
	return OutBuffer - pOldOut ;
}

//===================================================================

void  Huff_Decode (BYTE * InBuffer, DWORD Count_Array[256],
				   BYTE * OutBuffer, DWORD dwOutLength)
{
	HUFF_NODE	Node_Array [512] ;
	HUFF_CODE	Code_Array [256] ;

	//	���� �� && ӳ���
	DWORD	dwRoot = Huff_Build_Tree (Count_Array, Node_Array) ;
	Huff_Create_Table (Node_Array, Code_Array, 0, 0, dwRoot) ;

	HUFF_NODE	code ;
	char		cBit = 7 ;

	while (dwOutLength > 0)
	{
		code = Node_Array[dwRoot] ;
		while (true)
		{
			if (fooTestBit (*InBuffer, cBit))
				code = Node_Array[code.child_1] ;
			else
				code = Node_Array[code.child_0] ;
			if (--cBit < 0)
			{
				cBit = 7 ;
				InBuffer++ ;
			}
			if (code.child_0 == 0xFFFF)	// Ҷ�ڵ�����
			{
				*OutBuffer++ = code.number ;
				dwOutLength-- ;
				break ;
			}
		}
	}
	return ;
}

//===================================================================
