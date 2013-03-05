//////////////////////////////////////////////////////////////////
//																//
//		用途 : 次序0的静态Huffman压缩算法						//
//		创建 : [Foolish] / 2001-3-6								//
//		更新 : 2002-1-12										//
//		主页 : http://www.crazy-bit.com/					//
//		邮箱 : crazybit@263.net									//
//									(c) 1999 - 2002 =USTC= 付黎	//
//////////////////////////////////////////////////////////////////
#ifndef		__FOO_HUFF_COMPRESS_H__
#define		__FOO_HUFF_COMPRESS_H__
#include "../StdDefine.h"

//	叶节点child_0 == 0xFFFF
typedef struct tagHUFF_NODE
{
	BYTE	active ;  // 是否为活动节点
	BYTE	number ;  // 只有叶节点才有
	DWORD	count ;	  // 统计计数
	WORD	child_0 ; // 0-bit 支
	WORD	child_1 ; // 1-bit 支
	WORD	reserve ; // filled, DWORD对齐
} HUFF_NODE ;	// Total = 12-Byte

//  映射表项
typedef struct tagHUFF_CODE
{
	DWORD	code ;	// 32Bit - Code
	DWORD	bit_length ;
} HUFF_CODE ;

//===================================================================
//	Huffman - 压缩算法
//===================================================================
////////////////////////////////////////////////////////////
//	功  能 ：统计在DataBuffer中0x00--0xFF每个字符出现的个数
//	参  数 ：统计计数放在 Count_Array[256] 中
//	返回值 ：
//	说  明 ：
////////////////////////////////////////////////////////////
void  Huff_Count (BYTE * pDataBuffer, DWORD dwBufferLength,
									  DWORD Count_Array[256]) ; // <-- 统计计数
////////////////////////////////////////////////////////////
//	功  能 ：找出概率最小的两个数
//	参  数 ：min 中返回在树中的位置
//	返回值 ：Return false 则无活动节点
//	说  明 ：
////////////////////////////////////////////////////////////
bool  __fooSearchMin  (HUFF_NODE Node_Array[513], int * min) ;
bool  __fooSearchMin2 (HUFF_NODE Node_Array[513], int * min_1, int * min_2) ;
////////////////////////////////////////////////////////////
//	功  能 ：建立 huffman 树
//	参  数 ：
//	返回值 ：返回 root 的位置
//	说  明 ：
////////////////////////////////////////////////////////////
DWORD  Huff_Build_Tree (DWORD		Count_Array[256], 
						HUFF_NODE	Node_Array [512]) ; // <--树
////////////////////////////////////////////////////////////
//	功  能 ：建立映射表
//	参  数 ：
//	返回值 ：
//	说  明 ：
////////////////////////////////////////////////////////////
void  Huff_Create_Table (HUFF_NODE	Node_Array[512],  // <--Huffman树
						 HUFF_CODE	Code_Array[256],  // <--映射表存放于此
						 DWORD		code_walk,  // init == 0
						 WORD		bit_walk,	// init == 0
						 DWORD		root) ;		// 树根位置
////////////////////////////////////////////////////////////
//	功  能 ：Huffman 压缩
//	参  数 ：
//	返回值 ：返回写入OutBuffer的字节数
//	说  明 ：使用前必须把 OutBuffer 置 0 
////////////////////////////////////////////////////////////
DWORD  Huff_Encode (BYTE * InBuffer, DWORD dwInSize, BYTE * OutBuffer,
					BYTE * WriteBit = NULL) ;
////////////////////////////////////////////////////////////
//	功  能 ：Huffman 解压缩
//	参  数 ：dwOutLength为压缩前代码的长度
//	返回值 ：
//	说  明 ：使用前必须把 OutBuffer 置 0 
////////////////////////////////////////////////////////////
void  Huff_Decode (BYTE * InBuffer, DWORD Count_Array[256],
				   BYTE * OutBuffer,
				   DWORD  dwOutLength) ;	// 压缩前代码的长度 (文件大小)

//===================================================================
//	Implement
//===================================================================
inline void  Huff_Count (BYTE * DataBuffer, DWORD BufferLength, DWORD Count_Array[256]) {
	::memset (Count_Array, 0, 256 * sizeof(DWORD)) ;
	for (DWORD count = 0 ; count < BufferLength ; count++)
		Count_Array[DataBuffer[count]]++ ;
	return ;
}
//	Temporary : search count最低的两个节点, 并置为不活动
inline bool  __fooSearchMin2 (HUFF_NODE Node_Array[512], int * min_1, int * min_2) {
	bool	result = __fooSearchMin (Node_Array, min_1) ;
	result &= __fooSearchMin (Node_Array, min_2) ;
	return result ;
}
//	Temporary : search count 最低的节点, 并置为不活动
//				Return false 则无活动节点
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
