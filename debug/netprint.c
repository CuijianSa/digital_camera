#include <config.h>
#include <debug_manager.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

#define SERVER_PORT 5678
#define PRINT_BUF_SIZE   (16*1024)

static int g_iSocketServer = -1;
static struct sockaddr_in g_tSocketServerAddr;
static struct sockaddr_in g_tSocketClientAddr;
static char *g_pcNetPrintBuf;
static int g_iHaveConnected = 0;
static int g_iReadPos  = 0;
static int g_iWritePos = 0;


static pthread_t g_tSendTreadID;
static pthread_t g_tRecvTreadID;

static pthread_mutex_t g_tNetDbgSendMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tNetDbgSendConVar = PTHREAD_COND_INITIALIZER;

static int isEmpty(void)
{
	return (g_iWritePos == g_iReadPos);
}

static int isFull(void)
{
	return (((g_iWritePos + 1)%PRINT_BUF_SIZE) == g_iReadPos);
}

static int GetData(char *pcVal)
{
	if (isEmpty())
		return -1;
	else
	{
		*pcVal = g_pcNetPrintBuf[g_iReadPos];
		g_iReadPos = (g_iReadPos + 1) % PRINT_BUF_SIZE;
		return 0;
	}
}

static int PutData(char cVal)
{
	if(isFull())
		return -1;
	else
	{
		g_pcNetPrintBuf[g_iWritePos] = cVal;
		g_iWritePos = (g_iWritePos + 1) % PRINT_BUF_SIZE;
		return 0;
	}
}


static void *NetDbgSendTreadFunction(void *pVoid)
{
	char strTmpBuf[512];
	char cVal;
	int i;
	int iAddrLen;
	int iSendLen;

	while(1)
	{
		pthread_mutex_lock(&g_tNetDbgSendMutex);
		pthread_cond_wait(&g_tNetDbgSendConVar,&g_tNetDbgSendMutex);
		pthread_mutex_unlock(&g_tNetDbgSendMutex);

		while (g_iHaveConnected && !isEmpty())
		{
			i = 0;
			/* 把环形缓冲区的数据取出来, 最多取512字节 */
			while ((i < 512) && (0 == GetData(&cVal)))
			{
				strTmpBuf[i] = cVal;
				i++;
			}
			iAddrLen = sizeof(struct sockaddr);
			iSendLen = sendto(g_iSocketServer,strTmpBuf, i, 0,(const struct sockaddr *)&g_tSocketClientAddr, iAddrLen);
		}
	}
	return NULL;
}

static void *NetDbgRecvTreadFunction(void *pVoid)
{
	socklen_t iAddrLen;
	int iRecvLen;
	char ucRecvBuf[1000];
	struct sockaddr_in tSocketClientAddr;

	while(1)
	{
		iAddrLen = sizeof(struct sockaddr);
		DBG_PRINTF("in NetDbgRecvThreadFunction\n");
		iRecvLen = recvfrom(g_iSocketServer,ucRecvBuf, 999, 0, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);

		if(iRecvLen > 0)
		{
			ucRecvBuf[iRecvLen] = '\0';
			DBG_PRINTF("netprint.c get msg: %s\n", ucRecvBuf);
			/* 瑙ｆ瀽鏁版嵁:
			 * setclient			: 璁剧疆鎺ユ敹鎵撳嵃淇℃伅鐨勫鎴风
			 * dbglevel=0,1,2...	: 淇敼鎵撳嵃绾у埆
			 * stdout=0 			: 鍏抽棴stdout鎵撳嵃
			 * stdout=1 			: 鎵撳紑stdout鎵撳嵃
			 * netprint=0			: 鍏抽棴netprint鎵撳嵃
			 * netprint=1			: 鎵撳紑netprint鎵撳嵃
			 */

			if(strcmp(ucRecvBuf,"setclient") == 0)
			{
				g_tSocketClientAddr = tSocketClientAddr;
				g_iHaveConnected = 1;
			}
			else if (strncmp(ucRecvBuf, "dbglevel=", 9) == 0)
			{
				/* 璁剧疆鎵撳嵃绾у埆 */
				SetDbgLevel(ucRecvBuf);
			}
			else
			{
				/* 寮�/鍏虫墦鍗伴�氶亾 */
				SetDbgChanel(ucRecvBuf);
			}
		}
	}
	return NULL;
}



int NetDbgInit(void)
{
	int iRet;
	
	g_iSocketServer = socket(AF_INET, SOCK_DGRAM, 0);
	if(g_iSocketServer < 0)
	{
		printf("socket error!\n");
		return -1;
	}
	g_tSocketServerAddr.sin_family      = AF_INET;
	g_tSocketServerAddr.sin_port        = htons(SERVER_PORT);  /* host to net, short */
 	g_tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
	memset(g_tSocketServerAddr.sin_zero, 0, 8);
	
	iRet = bind(g_iSocketServer, (const struct sockaddr *)&g_tSocketServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("bind error!\n");
		return -1;
	}

	g_pcNetPrintBuf = malloc(PRINT_BUF_SIZE);
	if (NULL == g_pcNetPrintBuf)
	{
		close(g_iSocketServer);
		return -1;
	}

	pthread_create(&g_tSendTreadID, NULL, NetDbgSendTreadFunction, NULL);			
	pthread_create(&g_tRecvTreadID, NULL, NetDbgRecvTreadFunction, NULL);			
	return 0;
}

int NetDbgExit(void)
{
	close(g_iSocketServer);
	free(g_pcNetPrintBuf);
	return 0;

}

int NetDbgPrint(char *strData)
{
	int i;
	
	for (i = 0; i < strlen(strData); i++)
	{
		if (0 != PutData(strData[i]))
			break;
	}
	pthread_mutex_lock(&g_tNetDbgSendMutex);
	pthread_cond_signal(&g_tNetDbgSendConVar);
	pthread_mutex_unlock(&g_tNetDbgSendMutex);

	return i;
}



static T_DebugOpr g_tNetDbgOpr = {
	.name       = "netprint",
	.isCanUse   = 1,
	.DebugInit  = NetDbgInit,
	.DebugExit  = NetDbgExit,
	.DebugPrint = NetDbgPrint,
};

int NetPrintInit(void)
{
	return RegisterDebugOpr(&g_tNetDbgOpr);
}

