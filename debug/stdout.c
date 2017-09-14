#include <config.h>
#include <debug_manager.h>
#include <stdio.h>
#include <string.h>

static int StdoutDebugPrint(char *strData)
{
	/* 直接把输出信息用printf打印出来 */
	printf("%s", strData);
	return strlen(strData);	
}


static T_DebugOpr g_tStdoutDbgOpr = {
	.name       = "stdout",
	.isCanUse   = 1,                 /* 1表示将使用它来输出调试信息 */
	.DebugPrint = StdoutDebugPrint,  /* 打印函数 */
};

int StdoutInit()
{
	return RegisterDebugOpr(&g_tStdoutDbgOpr);
}

