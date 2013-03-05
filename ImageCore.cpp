#include "StdAfx.h"
#include "ObjImage.h"
#include "Win32_Func.h"
#include <fstream>



//===================================================================
BOOL  FCObjImage::ExportASCII (LPCTSTR szFileName)
{
	int					i, j, k, h;
	int					tint;
	char				tchar;
	int					TransHeight, TransWidth;
	int					grayindex;
	char				* lpPtr;
	
	ConvertToGray();

	char ch[95]={	' ',
					'`','1','2','3','4','5','6','7','8','9','0','-','=','\\',
					'q','w','e','r','t','y','u','i','o','p','[',']',
					'a','s','d','f','g','h','j','k','l',';','\'',
					'z','x','c','v','b','n','m',',','.','/',
					'~','!','@','#','$','%','^','&','*','(',')','_','+','|',
					'Q','W','E','R','T','Y','U','I','O','P','{','}',
					'A','S','D','F','G','H','J','K','L',':','"',
					'Z','X','C','V','B','N','M','<','>','?'
				};
	int  gr[95]={	 0,
					 7,22,28,31,31,27,32,22,38,32,40, 6,12,20,38,32,26,20,24,40,
  					 29,24,28,38,32,32,26,22,34,24,44,33,32,32,24,16, 6,22,26,22,
					 26,34,29,35,10, 6,20,14,22,47,42,34,40,10,35,21,22,22,16,14,
					 26,40,39,29,38,22,28,36,22,36,30,22,22,36,26,36,25,34,38,24,
					 36,22,12,12,26,30,30,34,39,42,41,18,18,22
				 };

	for(i=0;i<94;i++)
		for(j=i+1;j<95;j++){
			if(gr[i]>gr[j]){
				tchar=ch[i],tint=gr[i];
				ch[i]=ch[j],gr[i]=gr[j];
				ch[j]=tchar,gr[j]=tint;
			}
		};
		
	TransWidth  = Width()/8;
	TransHeight = Height()/16;

//    hf=_lcreat(szFileName,0);
    // 生成文件，存在即覆盖
    std::ofstream    outFile (szFileName, std::ios::out|std::ios::binary|std::ios::trunc) ;
    if (!outFile.is_open())
        return FALSE ;

	DWORD LineBytes = GetPitch();
	DWORD BufSize = GetPitch()*Height();

	for(i=0;i<TransHeight;i++){	
		for(j=0;j<TransWidth;j++){
			grayindex=0;
			for(k=0;k<16;k++)
				for(h=0;h<8;h++){
					lpPtr=(char *)m_pByte + BufSize - LineBytes-(i*16+k)*LineBytes+j*8+h;
					grayindex+=(unsigned char)*lpPtr;
				}
			grayindex/=16*8;
			grayindex=gr[94]*grayindex/255;
			k=0;
			while(gr[k+1]<grayindex)
				k++;
            outFile.write ((char *)&ch[k],sizeof(char)) ;
        }	
		tchar=(char)13;
        outFile.write ((char *)&tchar,sizeof(char)) ;
		tchar=(char)10;
        outFile.write ((char *)&tchar,sizeof(char)) ;
	}
    return TRUE ;
}
//===================================================================
// 根据文件扩展名判断图像文件类型
IMAGE_TYPE  GetImageType_FromName (PCSTR szFileName)
{
    const std::string     strFile(szFileName) ;
    const std::string     strExt(fooGetFileExtension(strFile)) ;
          char            * szConvert = new char[strExt.size() + 1] ;
    ZeroMemory (szConvert, strExt.size() + 1) ;
    strcpy (szConvert, strExt.c_str()) ;

    // 转成小写字母
    for (int i=0 ; i < (int)strExt.size() ; i++)
        szConvert[i] = tolower(szConvert[i]) ;

    IMAGE_TYPE      imgType = IMG_UNKNOW ;
    if ((strcmp (szConvert, "jpg") == 0) || (strcmp (szConvert, "jpeg") == 0))
        imgType = IMG_JPG ;
    else if (strcmp (szConvert, "gif") == 0)
        imgType = IMG_GIF ;
    else if (strcmp (szConvert, "png") == 0)
        imgType = IMG_PNG ;
    else if (strcmp (szConvert, "bmp") == 0)
        imgType = IMG_BMP ;
    else if (strcmp (szConvert, "pcx") == 0)
        imgType = IMG_PCX ;
    else if ((strcmp (szConvert, "tif") == 0) || (strcmp (szConvert, "tiff") == 0))
        imgType = IMG_TIF ;
    else if (strcmp (szConvert, "tga") == 0)
        imgType = IMG_TGA ;
    else if (strcmp (szConvert, "ico") == 0)
        imgType = IMG_ICO ;
    else if (strcmp (szConvert, "psd") == 0)
        imgType = IMG_PSD ;
    else if (strcmp (szConvert, "xpm") == 0)
        imgType = IMG_XPM ;
    delete[] szConvert ;
    return imgType ;
}

BOOL  FCObjImage::Save (PCSTR szFileName, int nFlag)
{
  
    return FALSE ;
}
//===================================================================
