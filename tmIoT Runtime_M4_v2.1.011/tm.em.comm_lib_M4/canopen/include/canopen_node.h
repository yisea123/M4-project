/*===========================================================================+
|  Function : Node                                                           |
|  Task     : Node Header File                                               |
|----------------------------------------------------------------------------|
|  Compile  : CCS 6.1                                                        |
|  Link     : CCS 6.1                                                        |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   :                                                                |
|  Version  : V1.00                                                          |
|  Creation : 2016.3.5                                                       |
|  Revision :                                                                |
+===========================================================================*/
#ifndef D__CANOPEN_NODE
#define D__CANOPEN_NODE

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_type.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef struct tagCANOPEN_NODE CANOPEN_NODE_T;

struct tagCANOPEN_NODE {
        CANOPEN_NODE_T          *pPrev;
        CANOPEN_NODE_T          *pNext;
};

/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/

/*===========================================================================+
|           Class declaration                                                |
+===========================================================================*/

/*===========================================================================+
|           Attributes                                                       |
+===========================================================================*/

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
CANOPEN_INLINE void Canopen_Node_Init(CANOPEN_NODE_T *pNode)
{
        pNode->pPrev = pNode;
        pNode->pNext = pNode;
}

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_INLINE void Canopen_Node_InsertBefore(CANOPEN_NODE_T *pNode1, CANOPEN_NODE_T *pNode2)
{
        pNode2->pNext           = pNode1;
        pNode2->pPrev           = pNode1->pPrev;
        pNode1->pPrev->pNext    = pNode2;
        pNode1->pPrev           = pNode2;
}

CANOPEN_INLINE void Canopen_Node_InsertAfter(CANOPEN_NODE_T *pNode1, CANOPEN_NODE_T *pNode2)
{
        pNode2->pNext           = pNode1->pNext;
        pNode2->pPrev           = pNode1;
        pNode1->pNext->pPrev    = pNode2;
        pNode1->pNext           = pNode2;
}

CANOPEN_INLINE void Canopen_Node_Remove(CANOPEN_NODE_T *pNode)
{
        pNode->pPrev->pNext     = pNode->pNext;
        pNode->pNext->pPrev     = pNode->pPrev;
        pNode->pNext            = pNode;
        pNode->pPrev            = pNode;
}


CANOPEN_INLINE CANOPEN_BOOL_T Canopen_Node_IsEmpty(CANOPEN_NODE_T *pNode)
{
        return pNode->pNext == pNode;
}


#define Canopen_Node_Entry(pNode, Type, Name) ((Type *)((char *)pNode - (unsigned long)(&((Type *)0)->Name)))

/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/


#ifdef __cplusplus
}
#endif


#endif
