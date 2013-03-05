#ifndef  __FOO_STANDARD_DEFINE_H__
#define  __FOO_STANDARD_DEFINE_H__

//=============================================================================
// max/min macro
//=============================================================================
template<class T>
inline const T& FMax (const T& _X, const T& _Y) {return (_X < _Y ? _Y : _X); }
template<class T>
inline const T& FMin (const T& _X, const T& _Y) {return (_Y < _X ? _Y : _X); }

#ifndef WINVER
    #define WINVER 0x0400
#endif

#ifdef WIN32
    #include <windows.h>
    #include <TCHAR.H>
    #define FLib_forceinline    __forceinline
#else
    #include "FLib_Macro.h"
    #define FLib_forceinline    inline
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include "StorageManage.h"

//=============================================================================
// namespace macro , 04/6/11 copy from dyunze
#ifdef      F_USE_NAMESPACE
    #define    FLIB_NAMESPACE           FLib
    #define    FLIB_NAMESPACE_START     namespace FLIB_NAMESPACE{
    #define    FLIB_NAMESPACE_END       } // end of namespace
    #define    FLIB_NAMESPACE_USE       using namespace FLIB_NAMESPACE;
#else
    #define    FLIB_NAMESPACE
    #define    FLIB_NAMESPACE_START
    #define    FLIB_NAMESPACE_END
    #define    FLIB_NAMESPACE_USE
#endif
//=============================================================================
// VC6默认SDK
#ifndef ULONG_PTR
    #define ULONG_PTR ULONG
#endif
//=============================================================================
#ifdef _DEBUG
	#define FAssert(f)        assert(f)
#else
	#define FAssert(f)        ((void)0)
#endif

//=============================================================================
//	编译警告
//=============================================================================
#define chSTR2(x)	   #x
#define chSTR(x)	chSTR2(x)
#define fooWARN(desc) message(__FILE__ "(" chSTR(__LINE__) "):" #desc)
//=============================================================================
//	常数
//=============================================================================
const double   LIB_SQRT2 = 1.4142135623730950488016887242097 ; // sqrt(2)
const double   LIB_PI = 3.1415926535897932384626433832795 ;
const double   LIB_2PI = 2.0*LIB_PI ;
const long     MMAX = 99999999;
const long     MMIN =-99999999;
inline LONG  RECTWIDTH(const RECT & fRect) {return fRect.right - fRect.left;}
inline LONG  RECTHEIGHT(const RECT & fRect) {return fRect.bottom - fRect.top;}
inline double AngleToRadian (int nAngle) {return LIB_PI * nAngle / 180.0;} // 角度 ==> 弧度
inline int  RadianToAngle (double dRadian) {return (int)(180.0 * dRadian / LIB_PI);} // 弧度 ==> 角度
inline int  FClamp0255 (int nValue) {return FMax (0, FMin (0xFF, nValue));} // 饱和到0--255
inline int  FRound (const double &x) { // 四舍五入取整
	if (x >= 0.0)
		return (int)(x + 0.5) ;
	else
		return (int)(x - 0.5) ;
}
// 饱和到 tLow--tHigh
template <class T> inline T  FClamp (const T &tValue, const T &tLow, const T &tHigh) {
	return FMax (tLow, FMin (tHigh, tValue)) ;
}
// 直角三角形求斜边
template <class T> inline T  FHypot (const T &x, const T &y) {
	return (T)sqrt(x*x + y*y) ;
}
// 得到最大/最小值
template <class T> inline T  FFindMAX (T * pList, int nNum) {
	T    tMax = pList[0] ;
	for (int i=1 ; i < nNum ; i++)
	{
		if (pList[i] > tMax)
			tMax = pList[i] ;
	}
	return tMax ;
}
template <class T> inline T  FFindMIN (T * pList, int nNum) {
	T    tMin = pList[0] ;
	for (int i=1 ; i < nNum ; i++)
	{
		if (pList[i] < tMin)
			tMin = pList[i] ;
	}
	return tMin ;
}
// 计算平方
template <class T> inline T  FSquare (const T &nValue) {
	return nValue*nValue ;
}
// 交换值
template <class T> inline void  FSwap (T & t1, T & t2) {
	T temp ; temp = t1 ; t1 = t2 ; t2 = temp ;
}
// 两点距离的平方
inline int  FSquarePointDistance (const POINT &pt1, const POINT &pt2) {
    return FSquare(pt1.x - pt2.x) + FSquare(pt1.y - pt2.y) ;
}
// 三点夹角 [0，n]
inline double  FPointAngle (const POINT &pt1, const POINT &pt2, const POINT &ptCenter) {
    int    nPt1 = FSquarePointDistance (pt1, ptCenter) ;
	if (nPt1 == 0)    return -360.0 ; // pt1和center重合
	int    nPt2 = FSquarePointDistance (pt2, ptCenter) ;
	if (nPt2 == 0)    return -360.0 ; // pt2和center重合

    int    nTemp = nPt1 + nPt2 - FSquarePointDistance (pt1, pt2) ;
	return acos ((double)nTemp / (2.0*sqrt((double)nPt1)*sqrt((double)nPt2))) ;
}
// 三点夹角 [0，2n]，pt1 --> pt2 的顺时针方向
inline double  FPointAngleClockwise (const POINT &pt1, const POINT &pt2, const POINT &ptCenter) {
	double    fAngle = FPointAngle (pt1, pt2, ptCenter) ;
	if (fAngle < 0.0) // 有一点和center重合
		return -360.0 ;
	if (pt1.x == ptCenter.x) // 垂直情况
		return ((pt2.x >= ptCenter.x) ? fAngle : (LIB_PI - fAngle)) ;
	// 判断在直线上还是下
	double    yLine = ptCenter.y + (pt2.x - ptCenter.x) * (ptCenter.y - pt1.y) / (double)(ptCenter.x - pt1.x) ;
	return (((double)pt2.y <= yLine) ? fAngle : (LIB_2PI - fAngle)) ;
}
// pt1顺时针旋转 [0, 360] 后的坐标
inline POINT  FClockwisePoint (const POINT &pt1, const POINT &ptCenter, const double &dAngle) {
	if ((pt1.x == ptCenter.x) && (pt1.y == ptCenter.y)) // 和center重合
		return ptCenter ;

	double   dx = pt1.x - ptCenter.x, dy = -pt1.y + ptCenter.y ;
	double   cost = cos(dAngle), sint = sin(dAngle) ;

	POINT    ptRet ;
	ptRet.x = ptCenter.x + (int)(dx*cost + dy*sint) ;
	ptRet.y = ptCenter.y - (int)(dy*cost - dx*sint) ;
	return ptRet ;
}
inline POINT  FClockwisePoint (const POINT &pt1, const POINT &ptCenter, int nAngle) {
	return FClockwisePoint (pt1, ptCenter, AngleToRadian(nAngle %= 360)) ;
}
inline void  NormalizeRect (RECT & rc)
{
	if (rc.left > rc.right)   { FSwap (rc.left, rc.right) ; }
	if (rc.top > rc.bottom)   { FSwap (rc.top, rc.bottom) ; }
}
inline BOOL  IsRectInRect (const RECT & rcOut, const RECT & rcIn) {
	return (rcIn.left >= rcOut.left) && (rcIn.top >= rcOut.top) && (rcIn.right <= rcOut.right) && (rcIn.bottom <= rcOut.bottom) ;
}
inline BOOL operator!=(RGBTRIPLE & cr1, RGBTRIPLE & cr2) {
	return (cr1.rgbtBlue != cr2.rgbtBlue) || (cr1.rgbtGreen != cr2.rgbtGreen) || (cr1.rgbtRed != cr2.rgbtRed) ;
}
//=============================================================================
//	结构
//=============================================================================
typedef struct tagRECT_F // 浮点数RECT
{
	double   left ;
    double   top ;
    double   right ;
    double   bottom ;
} RECT_F, *PRECT_F ;
inline void  SetRect_F (RECT_F * pRectf, const double &fLeft, const double &fTop, const double &fRight, const double &fBottom)
{
	if (pRectf != NULL)
	{
		pRectf->left = fLeft ; pRectf->right = fRight ;
		pRectf->top = fTop ; pRectf->bottom = fBottom ;
	}
}
inline void  OffsetRect_F (RECT_F * pRectf, const double & dx, const double & dy)
{
	if (pRectf != NULL)
	{
		pRectf->left += dx ; pRectf->right += dx ;
		pRectf->top += dy ; pRectf->bottom += dy ;
	}
}
inline void  NormalizeRect_F (RECT_F & rc)
{
	if (rc.left > rc.right)  { FSwap (rc.left, rc.right) ; }
	if (rc.top > rc.bottom)  { FSwap (rc.top, rc.bottom) ; }
}

typedef struct tagPOINT_F // 浮点数POINT
{
    double   x ;
    double   y ;
} POINT_F, *PPOINT_F ;

typedef struct tagSIZE_F // 浮点数SIZE
{
    double   cx ;
    double   cy ;
} SIZE_F, *PSIZE_F ;
//=============================================================================
//	坐标系统
//=============================================================================
enum AXIS_SYS
{
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	AXIS_TIME // 四维的:-)
} ;
//=============================================================================
//	平面8个方向
//=============================================================================
enum DIRECT_SYS
{
	DIRECT_TOP_LEFT,
	DIRECT_TOP,
	DIRECT_TOP_RIGHT,
	DIRECT_LEFT,
	DIRECT_RIGHT,
	DIRECT_BOTTOM_LEFT,
	DIRECT_BOTTOM,
	DIRECT_BOTTOM_RIGHT
} ;
//=============================================================================
//	形状
//=============================================================================
enum SHAPE_TYPE
{
	SHAPE_LINE,
	SHAPE_RECT,
	SHAPE_SQUARE,
	SHAPE_ELLIPSE,
	SHAPE_CIRCLE,
	SHAPE_ROUNDRECT,
	SHAPE_POLYGON
} ;
//=============================================================================
//	渐变过渡类型
//=============================================================================
enum REPEAT_MODE
{
	REPEAT_NONE,
	REPEAT_SAWTOOTH, // 锯齿波重复
	REPEAT_TRIANGULAR // 三角波重复
} ;
//=============================================================================
//	扫描线
//=============================================================================
enum SCAN_FIELDS
{
	SCAN_EVEN = 0, // 偶数
	SCAN_ODD = 1 // 奇数
} ;
//=============================================================================
//	逻辑操作
//=============================================================================
enum LOGICAL_OP
{
	LOGI_AND,	// c = a & b
	LOGI_OR,	// c = a | b
	LOGI_XOR,	// c = a ^ b
	LOGI_ADD,	// c = a + b
	LOGI_SUB,	// c = a - b
	LOGI_MUL,	// c = a * b
	LOGI_DIV,	// c = a / b
	LOGI_LOG,	// c = log(a)
	LOGI_EXP,	// c = exp(a)
	LOGI_SQRT,	// c = sqrt(a)
	LOGI_TRIG,	// c = sin/cos/tan(a)
	LOGI_INVERT,// c = (2B - 1) - a

    LOGI_SEL_ADD, // 用在区域处理里
    LOGI_SEL_SUB, // 用在区域处理里
} ;
//=============================================================================
//	16 位色的掩码
//=============================================================================
#define		MASK16_RED_565				0x0000F800
#define		MASK16_GREEN_565			0x000007E0
#define		MASK16_BLUE_565				0x0000001F
#define		MASK16_RED_555				0x00007C00
#define		MASK16_GREEN_555			0x000003E0
#define		MASK16_BLUE_555				0x0000001F
//=============================================================================
//  图像文件格式
//=============================================================================
enum IMAGE_TYPE
{
    IMG_UNKNOW,
    IMG_BMP,
    IMG_PCX,
    IMG_JPG,
    IMG_GIF,
    IMG_TGA,
    IMG_TIF,
    IMG_PNG,
    IMG_PSD,
    IMG_ICO,
    IMG_WMF,
    IMG_XPM
} ;
//=============================================================================
//	RGBA通道，这样的定义值是为了mask
//=============================================================================
enum IMAGE_CHANNEL
{
	CHANNEL_RED = 0x01,
	CHANNEL_GREEN = 0x02,
	CHANNEL_BLUE = 0x04,
	CHANNEL_ALPHA = 0x08,
	CHANNEL_RGB = 0x07,
	CHANNEL_RGBA = 0x0F,
	CHANNEL_GRAY = 0x10
} ;
//=============================================================================
// 线样式
//=============================================================================
#define  LINE_STYLE UINT
//enum LINE_STYLE
//{
#define  LINE_STYLE_SOLID PS_SOLID
#define  LINE_STYLE_DASH  PS_DASH
#define  LINE_STYLE_DOT   PS_DOT
//} ;
//=============================================================================
// 色调区域（这三个值的顺序一定不能变）
//=============================================================================
enum TONE_REGION
{
	TONE_SHADOWS = 0,
	TONE_MIDTONES = 1,
	TONE_HIGHLIGHTS = 2
} ;
//=============================================================================
//	阴影数据结构
//=============================================================================
typedef struct tagSHADOWDATA
{
	UINT32      nSmooth ; // 模糊度
	RGBQUAD     crShadow ; // 颜色/硬度
	UINT32      nAlpha ; // 透明度
	INT32       nOffsetX ; // X偏移
	INT32       nOffsetY ; // Y偏移
} SHADOWDATA, * PSHADOWDATA ;

//=============================================================================
// 获得文件的扩展名
inline std::string  fooGetFileExtension (const std::string & strFile)
{
    std::string     strOut ("") ;
    int			    nPos = (int)strFile.find_last_of (".") ;
    if (nPos != std::string::npos)
        strOut = strFile.substr (nPos + 1) ;
    return strOut ;
}

//=============================================================================
// Msel  liyongming add 20050106

typedef enum {
	left = 1,
	right = -1
} searchDir;

typedef struct {
	long x;
	long y;
} _tag_locationType;

typedef struct {
	_tag_locationType start;
	searchDir sdir;
	int numPixels;
	char * chain;
} _tag_chaincodeNode;

typedef _tag_chaincodeNode listNode;

typedef struct _tag_listS * listSingle;
typedef struct _tag_listS {
	listSingle next;
	listNode   node;
} listNodeSingle;

typedef struct {
	double smoothSigma;
	int stopHeight;
	int rootNodeBorder;
	double ratio;
	double ar1;
	int markovLength;
	char markov[2];
	int minimumEdgeLength;
} _tag_selOptions;

typedef struct {
	double mu[2];
	double sigma[2];
} _tag_gaussianParameters;

typedef struct {
	_tag_gaussianParameters gpara;
	double * ARModel;
	double offset;

	int markovLength;
	double * markovLogProb;

	_tag_locationType root;
	searchDir sdir;
} _tag_selParameters;

typedef struct {
	_tag_selOptions sopt;
	int levels;
} _tag_mselOptions;

typedef struct {
	_tag_locationType loc;
	int          dir;
	double       metric; 
	int          prev;
	int          end;
} _tag_stackType;

typedef struct {
	enum EMEndingConditions EMEnd;
	int iterations;
	double P[2][2];
} _tag_EMInformation;

enum EMEndingConditions {
	Normal,
	NotANumber,
	ProbabilityLimit
};

#define SDFACTOR 3.5
#define BORDER 0
#define PIXEL_STACK_LENGTH 10000
#define BRACKET_SIZE 2
#define STACK_LENGTH 10000    
#define MIN_CHAIN_SIZE 5           
#define MAX_MARKOV_ARRAY_LENGTH 59049 
#define MAX_STATIC_ARRAY 600000 /* findPathValues */
#define MAX_ESTIMATES 8
#define STARTING_LENGTH 8
#define MU_TOL 0.5                  /* convergence values for  */
#define SIGMA_TOL 5                 /* estimateGaussParaFromPath */
#define LOWEST_PROB 0.000001

#define REVERSE(x) (((x)+4)%8)
#define LEFT(x)    (((x)+2)%8)
#define RIGHT(x)   (((x)+6)%8)

#define SQR(x) ((x)*(x))
#define ANGLE_TO_DIR(x)  ((((x)+16)/32)%8)
#define DIFF_CHAIN(y,x) (((int)(y) - (int)(x) + 8 + 1) % 8)
#define NUMBER_Q(x)  (((x) < -1.0) || ((x) >= -1.0))

//;
//const double M_PI = atan(1.0) * 4.0;
#define M_PI  (3.141592653589793)

static int nnew = -1;
static int nmax = -1;
//===================================================================
//-----------------------------------------------------------------
static BYTE g_ant_data[8][8] =
{
	{
		0xF0,    /*  ####----  */
			0xE1,    /*  ###----#  */
			0xC3,    /*  ##----##  */
			0x87,    /*  #----###  */
			0x0F,    /*  ----####  */
			0x1E,    /*  ---####-  */
			0x3C,    /*  --####--  */
			0x78,    /*  -####---  */
	},
	{
		0xE1,    /*  ###----#  */
			0xC3,    /*  ##----##  */
			0x87,    /*  #----###  */
			0x0F,    /*  ----####  */
			0x1E,    /*  ---####-  */
			0x3C,    /*  --####--  */
			0x78,    /*  -####---  */
			0xF0,    /*  ####----  */
	},
		{
			0xC3,    /*  ##----##  */
				0x87,    /*  #----###  */
				0x0F,    /*  ----####  */
				0x1E,    /*  ---####-  */
				0x3C,    /*  --####--  */
				0x78,    /*  -####---  */
				0xF0,    /*  ####----  */
				0xE1,    /*  ###----#  */
		},
		{
			0x87,    /*  #----###  */
				0x0F,    /*  ----####  */
				0x1E,    /*  ---####-  */
				0x3C,    /*  --####--  */
				0x78,    /*  -####---  */
				0xF0,    /*  ####----  */
				0xE1,    /*  ###----#  */
				0xC3,    /*  ##----##  */
		},
			{
				0x0F,    /*  ----####  */
					0x1E,    /*  ---####-  */
					0x3C,    /*  --####--  */
					0x78,    /*  -####---  */
					0xF0,    /*  ####----  */
					0xE1,    /*  ###----#  */
					0xC3,    /*  ##----##  */
					0x87,    /*  #----###  */
			},
			{
				0x1E,    /*  ---####-  */
					0x3C,    /*  --####--  */
					0x78,    /*  -####---  */
					0xF0,    /*  ####----  */
					0xE1,    /*  ###----#  */
					0xC3,    /*  ##----##  */
					0x87,    /*  #----###  */
					0x0F,    /*  ----####  */
			},
				{
					0x3C,    /*  --####--  */
						0x78,    /*  -####---  */
						0xF0,    /*  ####----  */
						0xE1,    /*  ###----#  */
						0xC3,    /*  ##----##  */
						0x87,    /*  #----###  */
						0x0F,    /*  ----####  */
						0x1E,    /*  ---####-  */
				},
				{
					0x78,    /*  -####---  */
						0xF0,    /*  ####----  */
						0xE1,    /*  ###----#  */
						0xC3,    /*  ##----##  */
						0x87,    /*  #----###  */
						0x0F,    /*  ----####  */
						0x1E,    /*  ---####-  */
						0x3C,    /*  --####--  */
				},
};
//////////////////////////////////////////////////////////////////////////
typedef struct tagSectionPoint{
	BOOL m_bUse;
	COLORREF m_c;
} SP,*LPSECTION;

#define AutoStorage (CStorageManage::Instance()) 
#define MV_NEW_IMG(w,h,depth,nChannels) (AutoStorage->CreateIplImage(w,h,depth,nChannels))
#define MV_DEL_IMG(img) (AutoStorage->ReleaseIplImage(img))
#define MV_NEW_MAT(r,c,type) (AutoStorage->CreateMat(r,c,type))
#define MV_DEL_MAT(mat) (AutoStorage->ReleaseMat(mat))

typedef enum Parts_Direction{ 
	FRONT		=0,
	BACK		=1,
	EAST_SIDE	=2,
	WEST_SIDE   =3,
	NORTH_SIDE  =4,
	SOUTH_SIDE  =5

} PartsDirection;
//////////////////////////////////////////////////////////////////////////


static BYTE g_ant_xmask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01} ;
//===================================================================
//工具栏
#define  DEFAULT -1
#define  SELECTSQUARE_TOOL	20
#define  PEN_TOOL			21
#define  LINE_TOOL			22
#define  ERASE_TOOL			23
#define  CURVE_TOOL			24	
#define  BSPLINE_TOOL		25
#define  ZOOM_IN_TOOL		26	
#define  ZOOM_OUT_TOOL		27
#define  SELECTMAGICWAND_TOOL	28
#define  OBJECT_TOOL			29
#define  MOVE_TOOL              30
#define  POLYSEL_TOOL           31
#define  Draw_Status        32
#define  Drag_Status        33
#define  COLORBASESDIMP     34
#define  COLORPICK_TOOL     35
#define  SELECT_TOOL        36
#define  ROTATE_TOOL        37
#define  DISTORTION_TOOL    38
#define  CAM_CALIBRATION    39
#define  CAM_HOMOPHOGRAPHY  40
//===================================================================

#endif
