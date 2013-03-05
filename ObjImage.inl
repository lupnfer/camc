//////////////////////////////////////////////////////////////////
//                                                              //
//      用途 : Image对象的inline实现                            //
//      创建 : [Foolish] / 2001-4-27 (原FLib中的FCImage)        //
//      更新 : 2003-4-16                                        //
//      主页 : http://www.crazy-bit.com/                        //
//      邮箱 : crazybit@263.net                                 //
//                             	    (c) 1999 - 2004 =USTC= 付黎	//
//////////////////////////////////////////////////////////////////

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