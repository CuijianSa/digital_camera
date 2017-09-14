#include <config.h>
#include <fonts_manager.h>
#include <string.h>

static PT_FontOpr g_ptFontOprHead = NULL;
static int g_dwFontSize;

int RegisterFontOpr(PT_FontOpr ptFontOpr)
{
	PT_FontOpr ptTmp;
	if(!g_ptFontOprHead)
	{
		g_ptFontOprHead = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptFontOprHead;
		while(ptTmp->ptNext)
			ptTmp = ptTmp->ptNext;
		ptTmp->ptNext = ptFontOpr;
		ptFontOpr->ptNext = NULL;
	}
	return 0;
}

void ShowFontOpr(void)
{
	PT_FontOpr ptTmp = g_ptFontOprHead;
	int i = 0;

	while(ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
	return;
}
int FontsInit(void)
{
	int iError;
		
	iError = ASCIIInit();
	if (iError)
	{
		DBG_PRINTF("ASCIIInit error!\n");
		return -1;
	}

	iError = GBKInit();
	if (iError)
	{
		DBG_PRINTF("GBKInit error!\n");
		return -1;
	}	
	
	iError = FreeTypeInit();
	if (iError)
	{
		DBG_PRINTF("FreeTypeInit error!\n");
		return -1;
	}

	return 0;
}

PT_FontOpr GetFontOpr(char *pcName)
{
	PT_FontOpr ptTmp = g_ptFontOprHead;

	while(ptTmp)
	{
		if(strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

void SetFontSize(unsigned int dwFontSize)
{
	PT_FontOpr ptTmp = g_ptFontOprHead;

	g_dwFontSize = dwFontSize;

	while(ptTmp)
	{
		if(ptTmp->SetFontSize)
			ptTmp->SetFontSize(dwFontSize);
		ptTmp = ptTmp->ptNext;
	}
	return;
}

unsigned int GetFontSize(void)
{
	return g_dwFontSize;
}

int GetFontBitmap(unsigned int dwCode, PT_FontBitMap ptFontBitMap)
{
	PT_FontOpr ptTmp = g_ptFontOprHead;
	int iError = 0;

	while(ptTmp)
	{
		iError = ptTmp->GetFontBitmap(dwCode, ptFontBitMap);
		if(0 == iError)
			return 0;
		ptTmp = ptTmp->ptNext;
	}
	return -1;
}

// 设置字体模块的信息,比如指定字库文件,指定字符尺寸
int SetFontsDetail(char *pcFontsName, char *pcFontsFile, unsigned int dwFontSize)
{
	int iError = 0;
	PT_FontOpr ptFontOpr;

	ptFontOpr = GetFontOpr(pcFontsName);
	if(ptFontOpr == NULL)
		return -1;
	g_dwFontSize = dwFontSize;

	iError = ptFontOpr->FontInit(pcFontsFile, dwFontSize);

	return iError;
}


