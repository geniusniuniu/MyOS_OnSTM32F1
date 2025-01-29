#ifndef __tMEMBLOCK_H
#define __tMEMBLOCK_H

#include "tinyOS.h"

typedef struct _tMemBlock
{
	tEvent event;
	tList blockList;
	void * memStart;
	uint32_t blockSize;
	uint32_t maxCount;
	
}tMemBlock;


typedef struct _tMemBlockInfo
{
	uint32_t blockSize;
	uint32_t count;
	uint32_t maxCount;
	uint32_t taksCount;
	
}tMemBlockInfo;

void tMemBlockInit(tMemBlock * memBlock,uint8_t * memStart,uint32_t blockSize,uint32_t blockCnt);

uint32_t tMemBlockWait(tMemBlock * memBlock,uint8_t **mem,uint32_t waitTicks);

uint32_t tMemBlockNoWait(tMemBlock * memBlock,uint8_t **mem);

void tMemBlockNotify(tMemBlock * memBlock,uint8_t * mem);

void tMemBlockGetInfo(tMemBlock * memBlock,tMemBlockInfo * info);

uint32_t tMemBlockDelete(tMemBlock * memBlock);


#endif



