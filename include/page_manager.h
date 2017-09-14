#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <input_manager.h>
#include <disp_manager.h>

typedef struct PageParams{
	int iPageID;
	char strCurPictureFile[256];// Ҫ����ĵ�1��ͼƬ�ļ� 
}T_PageParams,*PT_PageParams;

typedef struct PageLayout{
	int iTopLeftX;
	int iTopLeftY;
	int iBotRightX;
	int iBotRightY;
	int iBpp;
	int iMaxTotalBytes;
	PT_Layout atLayout;///* ����: �������ֳɺü���С���� */
}T_PageLayout, *PT_PageLayout;

typedef struct PageAction{
	char *name;
	void (*Run)(PT_PageParams ptParentPageParams);
	int (*GetInputEvent)(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent); 
	int (*Prepare)(void); 
	struct PageAction *ptNext;    /* ���� */
}T_PageAction,*PT_PageAction;

typedef struct PageCfg{
	int iIntervalSecond;//����ģʽ��ͼƬ����ʾ���
	char strSeletedDir[256];//����ģʽ��Ҫ��ʾ�ĸ�Ŀ¼�µ�ͼƬ
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
