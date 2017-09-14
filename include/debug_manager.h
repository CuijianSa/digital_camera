#ifndef _DEBUG_MANAGER_H
#define _DEBUG_MANAGER_H

#define	APP_EMERG	"<0>"	/* system is unusable			*/
#define	APP_ALERT	"<1>"	/* action must be taken immediately	*/
#define	APP_CRIT	"<2>"	/* critical conditions			*/
#define	APP_ERR	    "<3>"	/* error conditions			*/
#define	APP_WARNING	"<4>"	/* warning conditions			*/
#define	APP_NOTICE	"<5>"	/* normal but significant condition	*/
#define	APP_INFO	"<6>"	/* informational			*/
#define	APP_DEBUG	"<7>"	/* debug-level messages			*/

#define DEFAULT_DBGLEVEL 4

typedef struct DebugOpr{
	char *name;
	int isCanUse;
	int (*DebugInit)(void);
	int (*DebugExit)(void);
	int (*DebugPrint)(char *strData);
	struct DebugOpr *ptNext;
}T_DebugOpr,*PT_DebugOpr;

int RegisterDebugOpr(PT_DebugOpr ptDebugOpr);
void ShowDebugOpr(void);
PT_DebugOpr GetDebugOpr(char *pcName);
/*
	dbglevel=3
*/
int SetDbgLevel(char *strBuf);
/*
 *	stdout=0			   : �ر�stdout��ӡ
 *   	stdout=1			   : ��stdout��ӡ
 *   	netprint=0		   : �ر�netprint��ӡ
 * 	netprint=1		   : ��netprint��ӡ
*/
int SetDbgChanel(char *strBuf);
int DebugPrint(const char *pcFormat, ...);
int DebugInit(void);
int InitDebugChanel(void);
int StdoutInit(void);
int NetPrintInit(void);

#endif
