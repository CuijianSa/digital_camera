#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <config.h>
#include <file.h>


int MapFile(PT_FileMap ptFileMap)
{
	int iFd;
	FILE *tFp;
	struct stat tStat;
	tFp = fopen(ptFileMap->strFileName, "r+");
	if (tFp == NULL)
	{
		DBG_PRINTF("can't open %s\n", ptFileMap->strFileName);
		return -1;
	}
	ptFileMap->tFp = tFp;
    iFd = fileno(tFp);

	fstat(iFd, &tStat);
	ptFileMap->iFileSize = tStat.st_size;
	ptFileMap->pucFileMapMem = (unsigned char *)mmap(NULL , tStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFd, 0);
	if (ptFileMap->pucFileMapMem == (unsigned char *)-1)
	{
		DBG_PRINTF("mmap error!\n");
		return -1;
	}
	return 0;
}

void UnMapFile(PT_FileMap ptFileMap)
{
	munmap(ptFileMap->pucFileMapMem, ptFileMap->iFileSize);
	fclose(ptFileMap->tFp);
}

static int isDir(char *strFilePath, char *strFileName)
{
    char strTmp[256];
    struct stat tStat;

    snprintf(strTmp, 256, "%s/%s", strFilePath, strFileName);
    strTmp[255] = '\0';

    if ((stat(strTmp, &tStat) == 0) && S_ISDIR(tStat.st_mode))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int isRegDir(char *strDirPath, char *strSubDirName)
{
    static const char *astrSpecailDirs[] = {"sbin", "bin", "usr", "lib", "proc", "tmp", "dev", "sys", NULL};
    int i;
    
    if (0 == strcmp(strDirPath, "/"))
    {
        while (astrSpecailDirs[i])
        {
            if (0 == strcmp(strSubDirName, astrSpecailDirs[i]))
                return 0;
            i++;
        }
    }
    return 1;    
}
//判断一个文件是否常规的文件,设备节点/链接文件/FIFO文件等是特殊文件
static int isRegFile(char *strFilePath, char *strFileName)
{
    char strTmp[256];
    struct stat tStat;

    snprintf(strTmp, 256, "%s/%s", strFilePath, strFileName);
    strTmp[255] = '\0';

    if ((stat(strTmp, &tStat) == 0) && S_ISREG(tStat.st_mode))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


int GetDirContents(char *strDirName, PT_DirContent **pptDirContents, int *piNumber)
{
	PT_DirContent *aptDirContents;
	struct dirent **aptNameList;
	int iNumber;
	int i;
	int j;

	iNumber = scandir(strDirName, &aptNameList, 0, alphasort);
	if (iNumber < 0)
	{
		DBG_PRINTF("scandir error : %s!\n", strDirName);
		return -1;
	}

	aptDirContents = malloc(sizeof(PT_DirContent) * (iNumber - 2));
	if (NULL == aptDirContents)
	{
		DBG_PRINTF("malloc error!\n");
		return -1;
	}
	*pptDirContents = aptDirContents;

	for (i = 0; i < iNumber - 2; i++)
	{
		aptDirContents[i] = malloc(sizeof(T_DirContent));
		if (NULL == aptDirContents)
		{
			DBG_PRINTF("malloc error!\n");
			return -1;
		}
	}

	for (i = 0, j = 0; i < iNumber; i++)
	{
		if ((0 == strcmp(aptNameList[i]->d_name, ".")) || (0 == strcmp(aptNameList[i]->d_name, "..")))
			continue;
		/* if (aptNameList[i]->d_type == DT_DIR) */
		if (isDir(strDirName, aptNameList[i]->d_name))
		{
			strncpy(aptDirContents[j]->strName, aptNameList[i]->d_name, 256);
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType	= FILETYPE_DIR;
			free(aptNameList[i]);
			aptNameList[i] = NULL;
			j++;
		}
	}

	for (i = 0; i < iNumber; i++)
	{
		if (aptNameList[i] == NULL)
			continue;
		
		if ((0 == strcmp(aptNameList[i]->d_name, ".")) || (0 == strcmp(aptNameList[i]->d_name, "..")))
			continue;
		/* if (aptNameList[i]->d_type == DT_REG) */
		if (isRegFile(strDirName, aptNameList[i]->d_name))
		{
			strncpy(aptDirContents[j]->strName, aptNameList[i]->d_name, 256);
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType	= FILETYPE_FILE;
			free(aptNameList[i]);
			aptNameList[i] = NULL;
			j++;
		}
	}

	for (i = j; i < iNumber - 2; i++)
	{
		free(aptDirContents[i]);
	}

	for (i = 0; i < iNumber; i++)
	{
		if (aptNameList[i])
		{
			free(aptNameList[i]);
		}
	}
	free(aptNameList);

	*piNumber = j;
	
	return 0;
}


void FreeDirContents(PT_DirContent *aptDirContents, int iNumber)
{
	int i;
	for (i = 0; i < iNumber; i++)
	{
		free(aptDirContents[i]);
	}
	free(aptDirContents);
}

/**********************************************************************
 * 函数名称： GetFilesIndir
 * 功能描述： 以深度优先的方式获得目录下的文件 
 *            即: 先获得顶层目录下的文件, 再进入一级子目录A
 *                再获得一级子目录A下的文件, 再进入二级子目录AA, ...
 *                处理完一级子目录A后, 再进入一级子目录B
 *
 * "连播模式"下调用该函数获得要显示的文件
 * 有两种方法获得这些文件:
 * 1. 事先把所有文件的名字保存到某个缓冲区中
 * 2. 用到时再去搜索取出若干个文件名
 * 第1种方法比较简单,但是当文件很多时有可能导致内存不足.
 * 我们使用第2种方法:
 * 假设某目录(包括所有子目录)下所有的文件都给它编一个号
 *
 * 输入参数：strDirName            : 要获得哪个目录下的内容 
 *           piStartNumberToRecord : 从第几个文件开始取出它们的名字
 *           iFileCountTotal       : 总共要取出多少个文件的名字
 * 输出参数：piFileCountHaveGet    : 已经得到了多少个文件的名字
 *           apstrFileNames[][256] : 用来存储搜索到的文件名
 * 输出/输出参数：
 *           piCurFileNumber       : 当前搜索到的文件编号
 * 返 回 值：0 - 成功
 *           1 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2013/02/08	     V1.0	  韦东山	      创建
 ***********************************************************************/

int GetFilesIndir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256])
{
	int iError;
	PT_DirContent *aptDirContents;  
	int iDirContentsNumber;		   
	int i;
	char strSubDirName[256];

#define MAX_DIR_DEEPNESS 10
	static int iDirDeepness = 0;

	if(iDirDeepness > MAX_DIR_DEEPNESS)
		return -1;

	iDirDeepness++;

	iError = GetDirContents(strDirName,&aptDirContents, &iDirContentsNumber);
	if (iError)
    {
        DBG_PRINTF("GetDirContents error!\n");
        iDirDeepness--;
        return -1;
    }

    for(i = 0; i < iDirContentsNumber; i++)
    {
		if(aptDirContents[i]->eFileType == FILETYPE_FILE)
		{
			if (*piCurFileNumber >= *piStartNumberToRecord)
            {
                snprintf(apstrFileNames[*piFileCountHaveGet], 256, "%s/%s", strDirName, aptDirContents[i]->strName);
                (*piFileCountHaveGet)++;
                (*piCurFileNumber)++;
                (*piStartNumberToRecord)++;
                if (*piFileCountHaveGet >= iFileCountTotal)
                {
                    FreeDirContents(aptDirContents, iDirContentsNumber);
                    iDirDeepness--;
                    return 0;
                }
            }
            else
            {
                (*piCurFileNumber)++;
            }
		}
    }

    for(i = 0; i < iDirContentsNumber; i++)
    {
		if ((aptDirContents[i]->eFileType == FILETYPE_DIR) && isRegDir(strDirName, aptDirContents[i]->strName))
        {
            snprintf(strSubDirName, 256, "%s/%s", strDirName, aptDirContents[i]->strName);
            GetFilesIndir(strSubDirName, piStartNumberToRecord, piCurFileNumber, piFileCountHaveGet, iFileCountTotal, apstrFileNames);
            if (*piFileCountHaveGet >= iFileCountTotal)
            {
                FreeDirContents(aptDirContents, iDirContentsNumber);
                iDirDeepness--;
                return 0;
            }
        }
    }
    FreeDirContents(aptDirContents, iDirContentsNumber);
    iDirDeepness--;
    return 0;
}
