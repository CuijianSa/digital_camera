#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

/*
	标准输入模块的设备初始化函数,用于设置标准输入的属性,
       比如默认的标准输入是接收到回车换行符时才返回数据,
       在本程序里把它改为"接收到任意一个字符即返回数据"
*/
static int StdinDevInit(void)
{
	struct termios tTTYState;
	tcgetattr(STDIN_FILENO, &tTTYState);
	tTTYState.c_lflag &= ~ICANON;
	tTTYState.c_cc[VMIN] = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);
	return 0;
}

int StdinDevExit(void)
{
	struct termios tTTYState;
    tcgetattr(STDIN_FILENO, &tTTYState);
 
    //turn on canonical mode
    tTTYState.c_lflag |= ICANON;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);	
	return 0;
}

int StdinGetInputEvent(PT_InputEvent ptInputEvent)
{
	char c;
	ptInputEvent->iType = INPUT_TYPE_STDIN;

	c = fgetc(stdin);//block while recv a char

	gettimeofday(&ptInputEvent->tTime,NULL);
	ptInputEvent->iKey = c;
#if 0	
	if (c == 'u')
	{
		ptInputEvent->iVal = INPUT_VALUE_UP;
	}
	else if (c == 'n')
	{
		ptInputEvent->iVal = INPUT_VALUE_DOWN;
	}
	else if (c == 'q')
	{
		ptInputEvent->iVal = INPUT_VALUE_EXIT;
	}
	else
	{
		ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
	}		
#endif	
	return 0;
}


static T_InputOpr g_tStdinOpr = {
	.name          = "stdin",
	.DeviceInit    = StdinDevInit,
	.DeviceExit    = StdinDevExit,
	.GetInputEvent = StdinGetInputEvent,
};

int StdinInit(void)
{
	return RegisterInputOpr(&g_tStdinOpr);
}

