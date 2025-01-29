#ifndef __tLIB_H
#define __tLIB_H

#include <stdint.h>

#define tNodeParent(node, parent, name) (parent *)((uint32_t)node - (uint32_t)&((parent *)0)->name)

static const uint8_t quickFindTable[] =     
{
	/* 00 */ 0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,//0xff填充无实际意义,仅占位
	/* 10 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 20 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 30 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 40 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 50 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 60 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 70 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 80 */ 7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* 90 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* A0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* B0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* C0 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* D0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* E0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/* F0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};


// 位图类型
typedef struct 
{
	uint32_t bitmap;
}tBitmap;

void tBitmapInit (tBitmap * bitmap);
uint32_t tBitmapPosCount (void);
void tBitmapSet (tBitmap * bitmap, uint32_t pos);
void tBitmapClear (tBitmap * bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet (tBitmap * bitmap);
	


/**********************************************************************************************************
** Function name        :   tNodeInit
** Descriptions         :   初始化结点类型
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
// tinyOS链表的结点类型
typedef struct listNode
{
	// 该结点的前一个结点
    struct listNode * prev;
    // 该结点的后一个结点
    struct listNode * next;
	
}listNode;

// tinyOS链表类型
typedef struct tList
{   
	// 该链表的头结点
    listNode headNode;

    // 该链表中所有结点数量
    uint32_t nodeCount;
}tList;


extern tBitmap taskPrioBM;



void listNodeInit (listNode * node);
void tListInit (tList * list);
uint32_t tListCount (tList * list);
listNode * tListFirst (tList * list);
listNode * tListLast (tList * list);
listNode * tListPre (tList * list, listNode * node);
listNode * tListNext (tList * list, listNode * node);
void tListRemoveAll (tList * list);
void tListAddFirst (tList * list, listNode * node);
void tListAddLast (tList * list, listNode * node);
listNode * tListRemoveFirst (tList * list);
void tListInsertAfter (tList * list, listNode * nodeAfter, listNode * nodeToInsert);
void tListRemove (tList * list, listNode * node);	

#endif


