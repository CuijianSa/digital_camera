#include <config.h>
#include <pic_operation.h>
#include <stdlib.h>
#include <string.h>

int PicZoom(PT_PixelDatas ptOriginPic,PT_PixelDatas ptZoomPic)
{
	unsigned long dwDstWidth = ptZoomPic->iWidth;
	unsigned long *pdwSrcXTable;
	unsigned long x,y,dwSrcY;
	unsigned char *pucDest;
	unsigned char *pucSrc;
	unsigned long dwPixelBytes = ptOriginPic->iBpp / 8;

	if(ptOriginPic->iBpp != ptZoomPic->iBpp)
		return -1;
	pdwSrcXTable = malloc(sizeof(unsigned long) * dwDstWidth);
	if (NULL == pdwSrcXTable)
    {
        DBG_PRINTF("malloc error!\n");
        return -1;
    }

    for(x = 0; x < dwDstWidth; ++x)//Éú³É±í
    	pdwSrcXTable[x] = (x*ptOriginPic->iWidth / ptZoomPic->iWidth);

    for(y = 0; y < ptZoomPic->iHeight; ++y)
    {
		dwSrcY = (y *ptOriginPic->iHeight / ptZoomPic->iHeight);
		pucDest = ptZoomPic->aucPixelDatas + y*ptZoomPic->iLineBytes;
		pucSrc  = ptOriginPic->aucPixelDatas + dwSrcY*ptOriginPic->iLineBytes;

		for (x = 0; x <dwDstWidth; x++)
		{
			memcpy(pucDest + x*dwPixelBytes, pucSrc+pdwSrcXTable[x]*dwPixelBytes, dwPixelBytes);
		}
    }
    free(pdwSrcXTable);
    return 0;
}

