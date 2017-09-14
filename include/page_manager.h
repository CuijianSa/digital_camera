#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <input_manager.h>
#include <disp_manager.h>

typedef struct PageParams{
	int iPageID;
	char strCurPictureFile[256];// 要处理的第1个图片文件 
}T_PageParams,*PT_PageParams;

typedef struct PageLayout{
	int iTopLeftX;
	int iTopLeftY;
	int iBotRightX;
	int iBotRightY;
	int iBpp;
	int iMaxTotalBytes;
	PT_Layout atLayout;///* 数组: 这个区域分成好几个小区域 */
}T_PageLayout, *PT_PageLayout;

typedef struct PageAction{
	char *name;
	void (*Run)(PT_PageParams ptParentPageParams);
	int (*GetInputEvent)(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent); 
	int (*Prepare)(void); 
	struct PageAction *ptNext;    /* 链表 */
}T_PageAction,*PT_PageAction;

typedef struct PageCfg{
	int iIntervalSecond;//连播模式下图片的显示间隔
	char strSeletedDir[256];//连播模式下要显示哪个目录下的图片
}T_PageCfg, *PT_PageCfg;

//calculate the only value according to string

int MainPageInit(void);
int SettingPageInit(void);
int IntervalPageInit(void);
int BrowsePageInit(void);
int AutoPageInit(void);
int ManualPageInit(void);

int RegisterPageAction(PT_PageAction ptPageAction);
void ShowPages(void);
int ID(char *strName);
PT_PageAction Page(const char *pcName);
int GeneratePage(PT_PageLayout ptPageLayout,PT_VideoMem ptVideoMem);
int TimeMSBetween(struct timeval tTimeStart, struct timeval tTimeEnd);
int GenericGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent);
int PagesInit(void);
void GetPageCfg(PT_PageCfg ptPageCfg);

#endif
