//////////////////////////////////////////////////////////////////
//																//
//		用途 : Image对象										//
//		更新 : 2004-2-22										//												//
//			1 . Image对象的坐标原点(0,0)取图象的左上角			//
//			2 . 对于rect, 右和下边界与图形学里一样为			//
//				开区间 (即不包含)。如选全图为:					//
//				(0, 0, 800, 600) 而不是 (0, 0, 799, 599)		//
//			3 . DIB像素从左下角存储, 在内存中存储格式为 B-G-R-a //
//			4 . 对于文件的Save, 文件存在则覆盖					//
//////////////////////////////////////////////////////////////////
#ifndef	 __FOO_OBJECT_IMAGE_H__
#define	 __FOO_OBJECT_IMAGE_H__

#include "ObjBase.h"
#include "FreeImage.h"
#include "VBDefine.h"
#include "cxcore.h"

class FCObjProgress ; // external class
class FCSinglePixelProcessBase ; // external class

//=============================================================================
//	encapsulate the operations of DIB
//=============================================================================
class FCObjImage : public FCObjGraph
{
public :
	// constructors
	FCObjImage () ;
	FCObjImage (const FCObjImage & img) ;
	FCObjImage (int iWidth, int iHeight, WORD wColorBit) ;
	FCObjImage & operator= (const FCObjImage & img) ;
	virtual ~FCObjImage () ;
	void	BoundRect (RECT_F & rect) const ;
	BOOL	BoundRect (RECT & rect) const ;
	virtual int  Serialize (BOOL bSave, BYTE * pSave) ; // save/load image

	// initialization
	
	BOOL	Create (const BITMAPINFOHEADER * pBmif) ; // 只创建，不设置象素和调色板
	BOOL	Create (int iWidth, int iHeight, WORD wColorBit = 0) ;
	void	Destroy () ; // 清除当前位图

	// attributes
	BOOL    IsValidImage () const ;         // 本对象是否有效
	BYTE  * GetBits (int iLine = 0) const ; // 取得第 iLine 行指针, 左上角为(0,0), 自上而下
	BYTE  * GetBits (int x, int y) const ;  // 取得 (x,y) 点的指针, 左上角为(0,0), 自上而下，自左而右
	BYTE  * GetMemStart () const ;          // 获得DIB左下角象素指针(也即起始地址)
    INT		Width () const ;                // 宽
	INT		Height () const ;               // 高
	WORD    ColorBits () const ;            // 颜色位数
	DWORD	GetPitch () const ;             // 行宽字节数 (32位补齐)
	//FIBITMAP *GetFIBitmap();				//取得位图FIBITMAP指针
	

	void	GetRectInCanvas (RECT * rect) const ; // 在canvas上的坐标
    void    GetImageInfo (BITMAPINFOHEADER * pInfo) const ;
    void    GetBitFields (void * pFields) const ;


	// pixel operations
	bool	IsInside (int x, int y) const ; // img坐标
	DWORD	GetPixelData (int x, int y) const ;
	void	SetPixelData (int x, int y, DWORD dwPixel) ;
	double	GetPixelData1 (int x, int y) ;
	void    GetPixel(int x,int y, BYTE &r,BYTE &g,BYTE &b);
	void    SetPixel(int x, int y ,BYTE r,BYTE g,BYTE b);
	void   SetGrayPixel(int x, int y ,BYTE gray);

	// palette operations, index is 0-based
	bool	IsGrayPalette () const ;
	void	SetGrayPalette () ;
	BOOL	GetColorTable (int iFirstIndex, int iNumber, RGBQUAD * pColors) const ;
	BOOL	SetColorTable (int iFirstIndex, int iNumber, RGBQUAD * pColors) ;
	static void  fooCopyPalette (FCObjImage & DibDest, const FCObjImage & DibSrc) ;

	// color convert
	void	ConvertQuantize (int nColorBits = 8) ; // ==> 1 or 4 or 8
	void	ConvertToGray () ; // 转换成灰度图
	void	ConvertTo16Bit () ;	// 1, 4, 8, 24, 32 ==> 16
	void	ConvertTo24Bit () ;	// 1, 4, 8, 16, 32 ==> 24
	void	ConvertTo32Bit () ;	// 1, 4, 8, 16, 24 ==> 32

	//new=========================================================================================
	
	//new=========================================================================================
	// channel operations (all received channel's bit == 8)
	void	GetAlphaChannel (FCObjImage * imgAlpha) const ;
	void	AppendAlphaChannel (const FCObjImage & alpha8) ;
	void	SetAlphaChannelValue (int nValue) ;
	void	CopyAlphaChannel (const FCObjImage & SrcImg32) ;

	// operations with another image object
	BOOL	GetSubBlock (FCObjImage * SubImg, const RECT & rcBlock) const ;
	BOOL	CoverBlock (const FCObjImage & Img, int x, int y) ;
    BOOL    TileBlock (const FCObjImage & Img, int x, int y) ;
	//==============================================================================
	// alpha混和是图像处理的心脏，它的效率和安全性关系着整个软件
	// 根据MaskImg32中的alpha通道计算
	void	AlphaBlend (const FCObjImage & MaskImg32, const RECT &rcDest, const RECT &rcSrc, int nAlphaPercent) ;
	//==============================================================================
	void	CombineAlphaImg (const FCObjImage & img32, int x=0, int y=0, int nAlphaPercent = 100) ;

	// pixel logical operation (>=8 bit)
	void	LogicalBlend (const FCObjImage & MaskImg, LOGICAL_OP LogOP, int x=0, int y=0) ;

	// base transform, don't change image's position
	void	ExpandFrame (BOOL bCopyEdge, int iLeft, int iTop, int iRight, int iBottom);// 加边框/add frame (>=8 bit)
	void	EraseFrame (int iLeft, int iTop, int iRight, int iBottom); // 擦除边框/erase frame (>=8 bit)
	void	Stretch (int nNewWidth, int nNewHeight); // 缩放/stretch (>=8 bit)
	void	Stretch_Smooth (int nNewWidth, int nNewHeight) ; // 缩放/stretch (>=24 bit)
	void	Rotate_Smooth (int nAngle) ; // 旋转/rotate (32 bit)
	void    AntiAliased_Zoom(int w);
	// 单象素处理过程
	void	SinglePixelProcessProc (FCSinglePixelProcessBase & PixelProcessor, FCObjProgress * progress = NULL) ;

	// 以下为图像文件格式的读写/image file operations
	BOOL    Load (PCSTR szFileName) ; // 从文件中读取
	BOOL	Load (IplImage* pImg);
	BOOL    Save (PCSTR szFileName, int nFlag = -1);
	


    BOOL	ExportASCII (LPCTSTR szFileName) ;

	void  Unload ();
	HBITMAP	GetHandle () const;
	BOOL __Create (int iWidth, int iHeight, WORD wColorBit, DWORD dwType, const DWORD pField[3]);
	BOOL  Create (BITMAPINFO * bmif, BYTE * pBits);
	BYTE *   __fooImageReadFile (PCTSTR szFileName, HANDLE * hFile, HANDLE * hMap) ;
	void  __fooImageUnmapFile (BYTE * pBase, HANDLE hMap, HANDLE hFile);
	//void     GenPaper(double ab,int ppType);
	//CVirtualPaper& GetPaper();

	IplImage* ToIplImage();
	
protected :
	// image object informations
	BITMAPINFO          *m_pDibInfo;
	BITMAPINFOHEADER    m_DibInfo ; // DIB Info
	BYTE              * m_pByte ;   // Bitmap start bits, left-bottom point
	BYTE             ** m_ppLine ;  // Line-pointer, ppLine[] ; 自上而下
    DWORD               m_dwBitFields[3] ; // 16位图像中的mask，按照R,G,B顺序排列
    RGBQUAD           * m_pPalette ; // 调色板
	HBITMAP				m_hBitmap ; // Handle from CreateDIBSection (DIB-Handle)
	
protected :
	void	__InitClassMember () ; // initialize the member var
	void	__ConvertToTrueColor (int iColor) ; // 转换为真彩色。iColor为24 or 32
protected:
	//CVirtualPaper   m_paper;
public:

	static FCObjImage& GetBGImage();
} ;
static FCObjImage BACKGROUND;


//=============================================================================
//	inline Implement
//=============================================================================
inline FCObjImage::FCObjImage () {
	this->__InitClassMember () ;
}
inline FCObjImage::FCObjImage (const FCObjImage & img) {
	this->__InitClassMember () ;
	this->operator= (img) ;
}
inline FCObjImage::FCObjImage (int iWidth, int iHeight, WORD wColorBit) {
	this->__InitClassMember () ;
	this->Create (iWidth, iHeight, wColorBit) ;
}
inline FCObjImage::~FCObjImage () {
	this->Destroy () ;
}
// attributes -----------------------------------------------------
inline BOOL  FCObjImage::IsValidImage () const {
	return (m_pByte != NULL) ;
}
inline BYTE * FCObjImage::GetBits (int iLine) const {
	FAssert (IsInside(0,iLine)) ;
	return m_ppLine[iLine] ;
}
FLib_forceinline BYTE * FCObjImage::GetBits (int x, int y) const {
	FAssert (IsInside(x,y)) ;
	if (ColorBits() == 32)
		return (m_ppLine[y] + x * 4) ;
	if (ColorBits() == 8)
		return (m_ppLine[y] + x) ;
	return (m_ppLine[y] + x * ColorBits() / 8) ;
}
inline BYTE * FCObjImage::GetMemStart () const {
	return m_pByte ;
}
inline INT FCObjImage::Width () const {
	return m_DibInfo.biWidth ;
}
inline INT FCObjImage::Height () const {
	return m_DibInfo.biHeight ;
}
inline WORD FCObjImage::ColorBits () const {
	return m_DibInfo.biBitCount ;
}
inline DWORD FCObjImage::GetPitch () const {
	return ( 4 * ((Width() * (DWORD)ColorBits() + 31) / 32) ) ;
}
inline void  FCObjImage::GetRectInCanvas (RECT * pRect) const {
	if (pRect != NULL)
		::SetRect (pRect, m_ObjPos.x, m_ObjPos.y, m_ObjPos.x+Width(), m_ObjPos.y+Height()) ;
}
inline void  FCObjImage::GetImageInfo (BITMAPINFOHEADER * pInfo) const {
	if (pInfo != NULL)
		CopyMemory (pInfo, &m_DibInfo, sizeof(m_DibInfo)) ;
}
inline void  FCObjImage::GetBitFields (void * pFields) const {
	if (pFields != NULL)
		CopyMemory (pFields, &m_dwBitFields, 12) ;
}
// pixel operations -----------------------------------------------------
inline bool  FCObjImage::IsInside (int x, int y) const {
	return (x >= 0) && (x < Width()) && (y >= 0) && (y < Height()) ;
}
//===================================================================
inline BYTE *  FCObjImage::__fooImageReadFile (PCTSTR szFileName, HANDLE * hFile, HANDLE * hMap) {
	*hFile = ::CreateFile (szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL) ;
	if (*hFile == INVALID_HANDLE_VALUE)
		return NULL ;
	*hMap = ::CreateFileMapping (*hFile, NULL, PAGE_READONLY, 0, 0, NULL) ;
	if (*hMap == NULL)
		return NULL ;
	return (BYTE *) ::MapViewOfFile (*hMap, FILE_MAP_READ, 0, 0, 0) ;
}
//===================================================================

inline void  FCObjImage::__fooImageUnmapFile (BYTE * pBase, HANDLE hMap, HANDLE hFile) {
	if (pBase != NULL)	::UnmapViewOfFile (pBase) ;
	if (hMap != NULL)	::CloseHandle (hMap) ;
	if (hFile != INVALID_HANDLE_VALUE)	::CloseHandle (hFile) ;
}
//===================================================================

inline HBITMAP	FCObjImage::GetHandle () const {
	return m_hBitmap ;
}
//===================================================================

inline void  FCObjImage::Unload () {
	if (m_ppLine != NULL)
		delete[] m_ppLine ;
	if (m_hBitmap != NULL)
		::DeleteObject (m_hBitmap) ;
	m_hBitmap	= NULL ;
	m_pByte		= NULL ;
	m_ppLine	= NULL ;
	::ZeroMemory (&m_DibInfo, sizeof(m_DibInfo)) ;
}

#endif
