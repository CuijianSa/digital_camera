#ifndef ENCODING_MANAGER_H
#define ENCODING_MANAGER_H

#include <fonts_manager.h>
#include <disp_manager.h>

typedef struct EncodingOpr{
	char *name;
	int iHeadLen;
	PT_FontOpr ptFontOprSupportedHead;//把能支持这种编码的"字体模块", 放在这个链表里
	int (*isSupport)(unsigned char *pucBufHead);
	int (*GetCodeFrmBuf)(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);
	struct EncodingOpr *ptNext;
}T_EncodingOpr,*PT_EncodingOpr;

int RegisterEncodingOpr(PT_EncodingOpr ptEncodingOpr);
PT_EncodingOpr Encode(char *pcName);
void ShowEncodingOpr(void);
PT_DispOpr GetDispOpr(char *pcName);
PT_EncodingOpr SelectEncodingOprForFile(unsigned char *pucFileBufHead);

/*
	1.字符编码值
	2.编码值的表示方式
	3.字符的位图
*/
int AddFontOprForEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr);
int DelFontOprFrmEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr);
int EncodingInit(void);
int AsciiEncodingInit(void);
int  Utf16beEncodingInit(void);
int  Utf16leEncodingInit(void);
int  Utf8EncodingInit(void);
int GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

#endif
