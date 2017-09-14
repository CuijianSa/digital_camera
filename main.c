#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <pic_operation.h>
#include <render.h>
#include <string.h>
#include <picfmt_manager.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>

static void debug_test()
{
	ShowDebugOpr();
	PT_DebugOpr stdio = GetDebugOpr("stdout");
	stdio->DebugPrint("stdout ok\n");
}

static void file_test()
{
	PT_DirContent *pptDirContents;
	int i = 0;
	int piNumber = 0;
	int ret = GetDirContents(".",  &pptDirContents,&piNumber);
	printf("GetDirContents:%d  %d\n",ret,piNumber);

	for(i = 0; i < piNumber; i++)
	{
		printf("filename:%s type:%d\n",pptDirContents[i]->strName,pptDirContents[i]->eFileType);
	}

	FreeDirContents(pptDirContents, piNumber);
}


void fb_test()
{
	T_Layout lyout = {160,90,320,180};
	PT_DispOpr fbOpr = GetDefaultDispDev();
	ShowDispOpr();
	PT_VideoMem fbvideoMem = GetDevVideoMem();
	assert((fbvideoMem != NULL) && (fbvideoMem->tPixelDatas.aucPixelDatas));
	ClearVideoMemRegion(fbvideoMem, &lyout, 0x514438);
}

void fonts_test()
{
	int iError = SetFontsDetail("ascii", NULL,16);
	if(iError < 0)
	{
		printf("SetFontsDetail error\n");
		return ;
	}
	
	PT_FontOpr pt_ansii = GetFontOpr("ascii");
	if(pt_ansii)
	{
		T_FontBitMap fontBitMap;
		fontBitMap.iCurOriginX = 0;
		fontBitMap.iCurOriginY = 16;		
		if(0 == GetFontBitmap(2, &fontBitMap));
		{
			int x = 0,y = 0,i = 7;
			unsigned char *pucbuf = fontBitMap.pucBuffer;
			//printf("%x\n",*pucbuf);
			for(y = 0; y < 16; ++y)
			{
				for(x = 0;x < 8; ++x)
				{
					if((*pucbuf >> x)&0x1)
						printf("1");
					else
						printf(" ");
				}
				printf("\n");		
				pucbuf++;
			}									
		}
	}
}

void PicFrm_test()
{
	ShowPicFmts();
	
	T_FileMap tFileMap;
	sprintf(tFileMap.strFileName,"denx.bmp");
	int iError = MapFile(&tFileMap);
	if(iError != 0)
	{
		DBG_PRINTF("MapFile:%s failure\n",tFileMap.strFileName);
		return;
	}
	PT_PicFileParser ptParser = Parser("bmp");
	if(ptParser == NULL)
	{
		DBG_PRINTF("GetParset BMP failure\n");
		return;
	}
	iError = ptParser->isSupport(&tFileMap);
	if(iError)
		DBG_PRINTF("%s is bmp file\n",tFileMap.strFileName);
	else
		DBG_PRINTF("%s isn't bmp file\n",tFileMap.strFileName);
		
	return;
}
int main(int argc,char *argv[])
{
	int iError;
	
	DebugInit();
	InitDebugChanel();

	if (argc != 2)
	{
		DBG_PRINTF("Usage:\n");
		DBG_PRINTF("%s <freetype_file>\n", argv[0]);
		return 0;
	}
	
	DisplayInit();
	SelectAndInitDefaultDispDev("fb");
	AllocVideoMem(5);
	
	InputInit();
	AllInputDevicesInit();
	
	EncodingInit();
	iError = FontsInit();
	if (iError)
	{
		DBG_PRINTF("FontsInit error!\n");
	}
	iError = SetFontsDetail("freetype", argv[1], 24);
	if (iError)
	{
		DBG_PRINTF("SetFontsDetail error!\n");
	}
	
	PicFmtsInit();
	
	PagesInit();

	Page("main")->Run(NULL);
#ifdef DEBUG
	int iError;
	T_InputEvent inputEvent;
	DebugInit();
	InitDebugChanel();
	
	DisplayInit();
	SelectAndInitDefaultDispDev("fb");
	AllocVideoMem(0);

	InputInit();
	AllInputDevicesInit();

	FontsInit();
	
	debug_test();
	//fb_test();
	file_test();
	fonts_test();

	PicFmtsInit();
	PicFrm_test();
	while(1)
	{
		GetInputEvent(&inputEvent);
		printf("%lu\t%lu  iType:%d iKey:%c\n",inputEvent.tTime.tv_sec,inputEvent.tTime.tv_usec,inputEvent.iType,inputEvent.iKey);
		
	}
#endif	
}
