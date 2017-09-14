#ifndef FONTS_MANAGER_H
#define FONTS_MANAGER_H

typedef struct FontBitMap{
	int iXLeft;//位图左上角X
	int iYTop;////位图左上角Y
	int iXMax;//位图最大X坐标
	int iYMax;//位图最大Y坐标
	int iBpp;
	int iPitch;//对于单色位图，两行像素之间跨度
	int iCurOriginX;//原点坐标(一般是左下角)
	int iCurOriginY;
	int iNextOriginX;//下一字符原点坐标(右边字符)
	int iNextOriginY;
	unsigned char *pucBuffer;
}T_FontBitMap, *PT_FontBitMap;

typedef struct FontOpr{
	char *name;
	int (*FontInit)(char *pcFontFile, unsigned int dwFontSize);
	int (*GetFontBitmap)(unsigned int dwCode, PT_FontBitMap ptFontBitMap);
	void (*SetFontSize)(unsigned int dwFontSize); 
	struct FontOpr *ptNext;
}T_FontOpr,*PT_FontOpr;

int RegisterFontOpr(PT_FontOpr ptFontOpr);
void ShowFontOpr(void);
int FontsInit(void);
int ASCIIInit(void);
int GBKInit(void);
int FreeTypeInit(void);
PT_FontOpr GetFontOpr(char *pcName);
void SetFontSize(unsigned int dwFontSize);
unsigned int GetFontSize(void);
int GetFontBitmap(unsigned int dwCode, PT_FontBitMap ptFontBitMap);
int SetFontsDetail(char *pcFontsName, char *pcFontsFile, unsigned int dwFontSize);



#endif
