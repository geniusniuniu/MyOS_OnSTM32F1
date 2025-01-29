#include "tLib.h"

/**********************************************************************************************************
** Function name        :   tNodeInit
** Descriptions         :   初始化结点类型
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void listNodeInit (listNode * node)
{
    node->next = node;
    node->prev = node;
}

/**********************************************************************************************************
** Function name        :   tListInit
** Descriptions         :   链表初始化
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void tListInit (tList * list)
{
	list->headNode.next = &(list->headNode);
    list->headNode.prev = &(list->headNode);
    list->nodeCount = 0;
}

/**********************************************************************************************************
** Function name        :   tListCount
** Descriptions         :   返回链表中结点的数量
** parameters           :   无
** Returned value       :   结点数量
***********************************************************************************************************/
uint32_t tListCount (tList * list)
{
	return list->nodeCount;
}

/**********************************************************************************************************
** Function name        :   tListFirst
** Descriptions         :   返回链表的首个结点
** parameters           :   list 查询的链表
** Returned value       :   首个结点，如果链表为空，则返回0
***********************************************************************************************************/
listNode * tListFirst (tList * list)
{
    listNode * node = (listNode *)0;
	
	if (list->nodeCount != 0) 
	{
		node = list->headNode.next;
	}    
    return  node;
}

/**********************************************************************************************************
** Function name        :   tListLast
** Descriptions         :   返回链表的最后一个结点
** parameters           :   list 查询的链表
** Returned value       :   最后的结点，如果链表为空，则返回0
***********************************************************************************************************/
listNode * tListLast (tList * list)
{
    listNode * node = (listNode *)0;
	
	if (list->nodeCount != 0) 
	{
		node = list->headNode.prev;
	}    
    return  node;
}

/**********************************************************************************************************
** Function name        :   tListPre
** Descriptions         :   返回链表中指定结点的前一结点
** parameters           :   list 查询的链表
** parameters           :   node 参考结点
** Returned value       :   前一结点结点，如果结点没有前结点（链表为空），则返回0
***********************************************************************************************************/
listNode * tListPre (tList * list, listNode * node)
{
	if (node->prev == node) 
	{
		return (listNode *)0;
	} 
	else 
	{
		return node->prev;
	}
}

/**********************************************************************************************************
** Function name        :   tListnextNode
** Descriptions         :   返回链表中指定结点的后一结点
** parameters           :   list 查询的链表
** parameters           :   node 参考结点
** Returned value       :   后一结点结点，如果结点没有前结点（链表为空），则返回0
***********************************************************************************************************/
listNode * tListNext (tList * list, listNode * node)
{
	if (node->next == node) 
	{
		return (listNode *)0;
	}
	else 
	{
		return node->next;
	}
}

/**********************************************************************************************************
** Function name        :   tListRemoveAll
** Descriptions         :   移除链表中的所有结点
** parameters           :   list 待清空的链表
** Returned value       :   无
***********************************************************************************************************/
void tListRemoveAll (tList * list)
{
	uint32_t num;
	listNode * nextNode = list->headNode.next;
	for(num = list->nodeCount; num > 0; num--)
	{
		listNode * currentNode = nextNode;
		nextNode = nextNode->next;		
		
		//重置结点自己的信息
		currentNode->prev = currentNode;		
		currentNode->next = currentNode;
	}
	list->headNode.next = &(list->headNode);
	list->headNode.prev = &(list->headNode);
	list->nodeCount = 0;
}

/**********************************************************************************************************
** Function name        :   tListAddFirst
** Descriptions         :   将指定结点添加到链表的头部
** parameters           :   list 待插入链表
** parameters			:   node 待插入的结点
** Returned value       :   无
***********************************************************************************************************/
void tListAddFirst (tList * list, listNode * node)  
{  
	//prev 指向之前的有效节点：保持链表的连贯性和有效性，确保每个节点都能正确连接到它的前驱和后继节点。
	//prev 指向头节点：headNode不应该被连接为实际的数据节点，会导致链表的结构不正确
    node->prev = list->headNode.next->prev;
    node->next = list->headNode.next;

    list->headNode.next->prev = node;
    list->headNode.next = node;

    list->nodeCount++;                    // 更新节点计数  
}

/**********************************************************************************************************
** Function name        :   tListAddLast
** Descriptions         :   将指定结点添加到链表的末尾
** parameters           :   list 待插入链表
** parameters			:   node 待插入的结点
** Returned value       :   无
***********************************************************************************************************/
void tListAddLast (tList * list, listNode * node)
{
	node->next = &(list->headNode);
	node->prev = list->headNode.prev;
	
	list->headNode.prev->next = node;
	list->headNode.prev = node;
	
    list->nodeCount++;
}

/**********************************************************************************************************
** Function name        :   tListRemoveFirst
** Descriptions         :   移除链表中的第1个结点
** parameters           :   list 待移除链表
** Returned value       :   如果链表为空，返回0，否则的话，返回第1个结点
***********************************************************************************************************/
listNode * tListRemoveFirst (tList * list)
{
    listNode * node = (listNode *)0;

	if( list->nodeCount != 0 )
    {
        node = list->headNode.next;

        node->next->prev = &(list->headNode);
        list->headNode.next = node->next;
        list->nodeCount--;
    }
    return  node;
}

/**********************************************************************************************************
** Function name        :   tListInsertAfter
** Descriptions         :   将指定的结点插入到某个结点后面
** parameters           :   list 			待插入的链表
** parameters           :   nodeAfter 		参考结点
** parameters           :   nodeToInsert 	待插入的结构
** Returned value       :   无
***********************************************************************************************************/
void tListInsertAfter (tList * list, listNode * node, listNode * nodeToIns)
{
	nodeToIns->prev = node;
	nodeToIns->next = node->next;
	
	node->next->prev = nodeToIns;
	node->next = nodeToIns;
	
    list->nodeCount++;
}

/**********************************************************************************************************
** Function name        :   tListRemove
** Descriptions         :   将指定结点从链表中移除
** parameters           :   list 	待移除的链表
** parameters           :   node 	待移除的结点
** Returned value       :   无
***********************************************************************************************************/
void tListRemove (tList * list, listNode * node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
    list->nodeCount--;
}
