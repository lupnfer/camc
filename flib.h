
#ifndef FLIB_H
#define FLIB_H


#define LIB_PROC                  WINAPI


#ifdef  __cplusplus
extern "C" {
#endif


BOOL    LIB_PROC Load (PCSTR szFileName) ; // 从文件中读取


#ifdef  __cplusplus
}

#endif


#endif