/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       stree.hpp
 * @brief      树模板--提供对一般的数据类型实现树结构
 * @version    v1.0      
 * @author     soui      
 * @date       2014-07-13
 * 
 * Describe    此类是下拉窗口的父类 只需要派生该类即可
 *
 *              version: 1.0　2003-10-24 实现基本功能
 *                       2.0  2004-12-29 增加两个遍历接口，修改内存释放部分可能存在的bug
 *                       2.1  2006-10-17 为节点增加一个hChildLast数据,以加快在数据插入时的速度
 *                       2.2  2008-10-16 修改一个遍历接口的问题
 *                       2.3  2011-10-17 将数据释放的接口从回调函数改成虚函数
 */
#if !defined(AFX_STREE_H__D2332B4E_2C7E_4357_BE22_EC55BF496C1C__INCLUDED_)
#define AFX_STREE_H__D2332B4E_2C7E_4357_BE22_EC55BF496C1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef SASSERT
#define SASSERT(x)
#endif

//typedef ULONG_PTR HSTREEITEM;

#define STVI_ROOT    ((HSTREEITEM)0xFFFF0000)//=TVI_ROOT
#define STVI_FIRST    ((HSTREEITEM)0xFFFF0001)//=TVI_FIRST
#define STVI_LAST    ((HSTREEITEM)0xFFFF0002)//=TVI_LAST
#define STVN_ROOT    ((HSTREENODE)(ULONG_PTR)0xFFFF0000)//=STVN_ROOT
#define STVN_FIRST    ((HSTREENODE)(ULONG_PTR)0xFFFF0001)//=STVN_FIRST
#define STVN_LAST    ((HSTREENODE)(ULONG_PTR)0xFFFF0002)//=STVN_LAST
#define STVL_ROOT    ((PSTREELINK)(ULONG_PTR)0xFFFF0000)

/**
 * @class      CSTree 模板类
 * @brief      CSTree 模板类
 * 
 * Describe    CSTree 模板类
 */
template<class T>
class CSTree  
{
    /**
     * @class      _STREENODE
     * @brief      节点结构体
     * 
     * Describe    节点结构体
     */
    typedef struct _STREENODE{
        struct _STREENODE *hParent;      /**< 父节点 */
        struct _STREENODE *hChildFirst;  /**< 第一个子节点 */
        struct _STREENODE *hChildLast;   /**< 最后一个子节点 */
        struct _STREENODE *hPrevSibling; /**< 兄节点 */
        struct _STREENODE *hNextSibling; /**< 弟节点 */
        T data;                          /**< 数据 */
    }STREENODE,*HSTREENODE;

    /**
     * @class      _STREELINK
     * @brief      树结点的链接
     * 
     * Describe    此结构体用于连接树节点
     */
    typedef struct _STREELINK{
        HSTREENODE  hParent;       /**< 父节点 */
        HSTREENODE  hChildFirst;   /**< 第一个子节点 */
        HSTREENODE  hChildLast;    /**< 最后一个子节点 */
        HSTREENODE  hPrevSibling;  /**< 兄节点 */
        HSTREENODE  hNextSibling;  /**< 弟节点 */
    }STREELINK,*PSTREELINK;
    
    /**
     * 回调函数
     * @brief    下拉窗口鼠标左键按下事件
     * @param    T *:当前遍历到的结点的数据
     * @param    LPARAM:回调时使用的参数
     * @return   FALSE－继续，TRUE－中断遍历
     *
     * Describe  遍历一个树结点的回调函数原型，
     */
    typedef BOOL (*CBTRAVERSING)(T*,LPARAM);

public:
    /**
     * CSTree::CSTree
     * @brief    构造函数
     *
     * Describe  构造函数
     */  
    CSTree()
    {
        m_hRootFirst=NULL;
        m_hRootLast=NULL;
    }

    /**
     * CSTree::~CSTree
     * @brief    析构函数
     *
     * Describe  析构函数
     */ 
    virtual ~CSTree(){DeleteAllItems();}

    /**
     * CSTree::DeleteAllItems
     * @brief    删除所有节点
     *
     * Describe  删除所有节点
     */
    void DeleteAllItems(){
        if(m_hRootFirst)
        {
            FreeNode(STVN_ROOT);
            m_hRootFirst=NULL;
            m_hRootLast=NULL;
        }
    }
    
    /**
     * CSTree::GetNextSiblingItem
     * @brief    获取下一个兄弟
     * @param    HSTREEITEM hItem  -- 节点
     * @return   返回HSTREEITEM
     *
     * Describe  获取下一个兄弟
     */
    static HSTREEITEM GetNextSiblingItem(HSTREEITEM hItem){
        PSTREELINK pLink= (PSTREELINK)hItem;
        SASSERT(pLink&&pLink!=STVL_ROOT);
        return (HSTREEITEM)pLink->hNextSibling;
    }

    /**
     * CSTree::GetPrevSiblingItem
     * @brief    获取上一个兄弟
     * @param    HSTREEITEM hItem  -- 节点
     * @return   返回HSTREEITEM
     *
     * Describe  获取上一个兄弟
     */
    static HSTREEITEM GetPrevSiblingItem(HSTREEITEM hItem)
    {
        PSTREELINK pLink= (PSTREELINK)hItem;
        SASSERT(pLink&&pLink!=STVL_ROOT);
        return (HSTREEITEM)pLink->hPrevSibling;
    }
    
    /**
     * CSTree::GetParentItem
     * @brief    获取父结点
     * @param    HSTREEITEM hItem  -- 节点
     * @return   返回HSTREEITEM
     *
     * Describe  获取父结点
     */
    static HSTREEITEM GetParentItem(HSTREEITEM hItem)
    {
        PSTREELINK pLink= (PSTREELINK)hItem;
        SASSERT(pLink&&pLink!=STVL_ROOT);
        return (HSTREEITEM)pLink->hParent;
    }
    
    /**
     * CSTree::GetItemLevel
     * @brief    获取结点层数
     * @param    HSTREEITEM hItem  -- 节点
     * @return   返回int
     *
     * Describe  获取结点层数
     */
    static int GetItemLevel(HSTREEITEM hItem)
    {
        int nRet=-1;
        if(hItem==STVI_ROOT) hItem=NULL;
        while(hItem)
        {
            nRet++;
            hItem=GetParentItem(hItem);
        }
        return nRet;
    }
    
    /**
     * GetRootItem
     * @brief    获取指定节点的根节点
     * @param    HSTREEITEM hItem -- 当前节点 
     * @return   HSTREEITEM 根节点
     *
     * Describe  
     */
    static HSTREEITEM GetRootItem(HSTREEITEM hItem)
    {
        HSTREEITEM hParent=hItem;
        while(GetParentItem(hParent))
        {
            hParent = GetParentItem(hParent);
        }
        return hParent;
    }
    
    /**
     * CSTree::GetChildItem
     * @brief    获取第一个子结点
     * @param    HSTREEITEM hItem  -- 节点
     * @param    BOOL bFirst       -- 是否第一个节点
     * @return   返回HSTREEITEM
     *
     * Describe  获取第一个子结点
     */
    HSTREEITEM GetChildItem(HSTREEITEM hItem,BOOL bFirst=TRUE) const
    {
        HSTREENODE hsNode= (HSTREENODE)hItem;
        SASSERT(hsNode);
        if(hsNode==STVN_ROOT)
        {
            if(bFirst)
                return (HSTREEITEM)m_hRootFirst;
            else
                return (HSTREEITEM)m_hRootLast;
        }
        else
        {
            if(bFirst)
                return (HSTREEITEM)hsNode->hChildFirst;
            else
                return (HSTREEITEM)hsNode->hChildLast;
        }
    }
    
    /**
     * CSTree::GetChildrenCount
     * @brief    获取子结点数量
     * @param    HSTREEITEM hItem  -- 节点
     * @return   返回int
     *
     * Describe  获取子结点数量
     */
    int GetChildrenCount(HSTREEITEM hItem) const
    {
        int nRet=0;
        HSTREEITEM hChild=GetChildItem(hItem);
        while(hChild)
        {
            nRet++;
            hChild=GetNextSiblingItem(hChild);
        }
        return nRet;
    }

    /**
     * CSTree::DeleteItem
     * @brief    删除一个节点，可以被派生类重载
     * @param    HSTREEITEM hItem  -- 节点
     *
     * Describe  删除一个节点，可以被派生类重载
     */
    virtual void DeleteItem(HSTREEITEM hItem)
    {
        HSTREENODE hsNode= (HSTREENODE)hItem;
        SASSERT(hsNode);
        if(hsNode==STVN_ROOT)
        {
            FreeNode(STVN_ROOT);
            m_hRootFirst=NULL;
            m_hRootLast=NULL;
            return;
        }
        STREENODE nodeCopy=*hsNode;
        BOOL bRootFirst=hsNode==m_hRootFirst;
        BOOL bRootLast=hsNode==m_hRootLast;
        FreeNode(hsNode);

        if(nodeCopy.hPrevSibling)//has prevsibling
            nodeCopy.hPrevSibling->hNextSibling=nodeCopy.hNextSibling;
        else if(nodeCopy.hParent)//parent's first child
            nodeCopy.hParent->hChildFirst=nodeCopy.hNextSibling;
        if(nodeCopy.hNextSibling)// update next sibling's previous sibling
            nodeCopy.hNextSibling->hPrevSibling=nodeCopy.hPrevSibling;
        else if(nodeCopy.hParent)//parent's last child
            nodeCopy.hParent->hChildLast=nodeCopy.hPrevSibling;
        //update root item
        if(bRootFirst)    m_hRootFirst=nodeCopy.hNextSibling;
        if(bRootLast)   m_hRootLast=nodeCopy.hPrevSibling;
    }

    /**
     * CSTree::DeleteItemEx
     * @brief    删除一个结点分枝，如果该结点的父结点没有其它子节点则一起删除
     * @param    HSTREEITEM hItem  -- 节点
     * @return   返回BOOL
     *
     * Describe  删除一个结点分枝，如果该结点的父结点没有其它子节点则一起删除
     */
    BOOL DeleteItemEx(HSTREEITEM hItem)
    {
        if(GetChildItem(hItem)) return FALSE;
        while(hItem && !GetChildItem(hItem))
        {
            HSTREEITEM hParent=GetParentItem(hItem);
            DeleteItem(hItem);
            hItem=hParent;
        }
        return TRUE;
    }

    /**
     * CSTree::GetItem
     * @brief    获取结点中保存的数据
     * @param    HSTREEITEM hItem  -- 节点
     * @return   返回T
     *
     * Describe  获取结点中保存的数据
     */
    static T GetItem(HSTREEITEM hItem){
        SASSERT(hItem!=STVI_ROOT);
        HSTREENODE hsNode= (HSTREENODE)hItem;
        SASSERT(hsNode);
        return hsNode->data;
    }
    
    /**
     * CSTree::GetItem
     * @brief    获取结点中保存的数据
     * @param    HSTREEITEM hItem  -- 节点
     * @return   返回T
     *
     * Describe  获取结点中保存的数据
     */
    static T& GetItemRef(HSTREEITEM hItem){
        SASSERT(hItem!=STVI_ROOT);
        HSTREENODE hsNode= (HSTREENODE)hItem;
        SASSERT(hsNode);
        return hsNode->data;
    }
    
    /**
     * CSTree::GetItemPt
     * @brief    获取结点中保存的数据
     * @param    HSTREEITEM hItem  -- 节点
     * @return   返回T*
     *
     * Describe  获取结点中保存的数据
     */
    static T *GetItemPt(HSTREEITEM hItem){
        SASSERT(hItem!=STVI_ROOT);
        HSTREENODE hsNode= (HSTREENODE)hItem;
        SASSERT(hsNode);
        return &hsNode->data;
    }

    /**
     * CSTree::InsertItem
     * @brief    插入一个新结点
     * @param    const T &data -- 结点数据
     * @param    HSTREEITEM hParent -- 新结点的父结点
     * @param    HSTREEITEM hInsertAfter -- 新结点的前一个兄弟结点
     * @return   返回HSTREEITEM 新结点的指针
     *
     * Describe  获取结点中保存的数据
     */
    HSTREEITEM InsertItem(const T &data,HSTREEITEM hParent=STVI_ROOT,HSTREEITEM hInsertAfter=STVI_LAST){
        HSTREENODE hParentNode=(HSTREENODE) hParent;
        HSTREENODE hInsertAfterNode=(HSTREENODE) hInsertAfter;
        if(hParentNode==STVN_ROOT)
            hParentNode=NULL;
        SASSERT(hInsertAfter);
        if(hInsertAfterNode!=STVN_FIRST && hInsertAfterNode!=STVN_LAST)
        {
            if(hInsertAfterNode->hParent!=hParentNode) return NULL;
            if(hInsertAfterNode->hNextSibling==NULL) hInsertAfterNode=STVN_LAST;
        }

        HSTREENODE hInserted=new STREENODE;
        hInserted->data=data;
        hInserted->hParent=hParentNode;
        hInserted->hChildFirst=NULL;
        hInserted->hChildLast=NULL;

        if(hInsertAfterNode==STVN_FIRST)
        {
            hInserted->hPrevSibling=NULL;
            if(hParentNode==NULL)//root
            {
                hInserted->hNextSibling=m_hRootFirst;
                if(m_hRootFirst) m_hRootFirst->hPrevSibling=hInserted;
                m_hRootFirst=hInserted;
                if(m_hRootLast==NULL) m_hRootLast=hInserted;
            }else    //has parent
            {
                hInserted->hNextSibling=hParentNode->hChildFirst;
                if(hInserted->hNextSibling)
                {
                    hInserted->hNextSibling->hPrevSibling=hInserted;
                    hParentNode->hChildFirst=hInserted;
                }else
                {
                    hParentNode->hChildLast=hParentNode->hChildFirst=hInserted;
                }
            }
        }else if(hInsertAfterNode==STVN_LAST)
        {
            hInserted->hNextSibling=NULL;
            if(hParentNode==NULL)//root
            {
                hInserted->hPrevSibling=m_hRootLast;
                if(m_hRootLast) m_hRootLast->hNextSibling=hInserted;
                m_hRootLast=hInserted;
                if(!m_hRootFirst) m_hRootFirst=hInserted;
            }else
            {
                hInserted->hPrevSibling=hParentNode->hChildLast;
                if(hParentNode->hChildLast) 
                {
                    hInserted->hPrevSibling->hNextSibling=hInserted;
                    hParentNode->hChildLast=hInserted;
                }else
                {
                    hParentNode->hChildLast=hParentNode->hChildFirst=hInserted;
                }                
            }
        }else
        {
            HSTREENODE hNextSibling=hInsertAfterNode->hNextSibling;
            hInserted->hPrevSibling=hInsertAfterNode;
            hInserted->hNextSibling=hNextSibling;
            hNextSibling->hPrevSibling = hInsertAfterNode->hNextSibling = hInserted;
        }
        return (HSTREEITEM)hInserted;
    }

    /**
     * CSTree::TraversingRecursion
     * @brief    采用递归方式遍历一个树结点
     * @param    HSTREEITEM hItem -- 起始结点
     * @param    CBTRAVERSING funTraversing -- 执行实际操作的回调函数
     * @param    LPARAM lParam -- 回调时使用的参数
     * @return   返回HSTREEITEM
     *
     * Describe  采用递归方式遍历一个树结点
     */
    HSTREEITEM TraversingRecursion(HSTREEITEM hItem,CBTRAVERSING funTraversing,LPARAM lParam)
    {
        SASSERT(hItem);
        if(hItem!=STVI_ROOT)
        {
            if(funTraversing(GetItemPt(hItem),lParam)) return hItem;
        }
        HSTREEITEM hChild=GetChildItem(hItem);
        while(hChild)
        {
            HSTREEITEM hTmp=GetChildItem(hChild);
            if(hTmp)
            {
                HSTREEITEM hRet=TraversingRecursion(hTmp,funTraversing,lParam);
                if(hRet) return hRet;
            }else
            {
                if(funTraversing(GetItemPt(hChild),lParam)) return hChild;
            }
            hChild=GetNextSiblingItem(hChild);
        }
        return NULL;
    }
   
    /**
     * CSTree::TraversingSequence
     * @brief    按顺序方式从指定结点开始查找后面的结点，包括自己的子节点及自己向下的兄弟结点
     * @param    HSTREEITEM hItem -- 起始结点
     * @param    CBTRAVERSING funTraversing -- 执行实际操作的回调函数
     * @param    LPARAM lParam -- 回调时使用的参数
     * @return   返回HSTREEITEM
     *
     * Describe  按顺序方式从指定结点开始查找后面的结点，包括自己的子节点及自己向下的兄弟结点
     */
    HSTREEITEM TraversingSequence(HSTREEITEM hItem,CBTRAVERSING funTraversing,LPARAM lParam)
    {
        if(!m_hRootFirst) return NULL;
        if(hItem!=STVI_ROOT)
        {
            if(funTraversing(GetItemPt(hItem),lParam)) return hItem;
        }
        HSTREEITEM hNext=GetNextItem(hItem);
        while(hNext)
        {
            if(funTraversing(GetItemPt(hNext),lParam)) return hNext;
            hNext=GetNextItem(hNext);
        }
        return NULL;
    }

    /**
     * CSTree::GetRootItem
     * @brief    获取根节点
     * @param    BOOL bFirst -- 是否从根节点
     * @return   返回HSTREEITEM 
     *
     * Describe  获取根节点
     */
    HSTREEITEM GetRootItem(BOOL bFirst=TRUE){
        return (HSTREEITEM)(bFirst?m_hRootFirst:m_hRootLast);
    }

    /**
     * CSTree::GetDesendants
     * @brief    获取结点的子孙结点数
     * @param    HSTREEITEM hItem -- 起始结点
     * @return   返回int
     *
     * Describe  获取结点的子孙结点数
     */
    int GetDesendants(HSTREEITEM hItem)
    {
        int nRet=0;
        HSTREEITEM hChild=GetChildItem(hItem);
        while(hChild)
        {
            nRet += 1+GetDesendants(hChild);
            hChild=GetNextSiblingItem(hChild);
        }
        return nRet;
    }

    /**
     * CSTree::GetNextItem
     * @brief    获取当前结点的下一个结点
     * @param    HSTREEITEM hItem -- 当前结点
     * @return   返回HSTREEITEM  当前结点的下一个结点
     *
     * Describe  获取当前结点的下一个结点
     * 如果当前结点有子结点，则返回自己的第一个子结点，
     * 否则如果有向下的兄弟结点，则返回自己向下兄弟结点、
     * 否则搜索自己的父结点的向下兄弟结点
     */
    HSTREEITEM GetNextItem(HSTREEITEM hItem)
    {
        if(hItem==STVI_ROOT) return (HSTREEITEM)m_hRootFirst;

        HSTREEITEM hRet=GetChildItem(hItem);
        if(hRet) return hRet;
        HSTREEITEM hParent=hItem;
        while(hParent)
        {
            hRet=GetNextSiblingItem(hParent);
            if(hRet) return hRet;
            hParent=GetParentItem(hParent);
        }
        return NULL;
    }

    /**
     * CSTree::GetNextItem
     * @brief    获取当前结点的下一个结点
     * @param    HSTREEITEM hItem -- 当前结点
     * @param    int &nLevel -- 当前结点(hItem)与目标结点(return)的层次关系,
     *                          1-父子关系，0－兄弟关系，-n－子->父的兄弟
     * @return   返回HSTREEITEM  当前结点的下一个结点
     *
     * Describe  获取当前结点的下一个结点
     *           如果当前结点有子结点，则返回自己的第一个子结点，
     *           否则如果有向下的兄弟结点，则返回自己向下兄弟结点、
     *           否则搜索自己的父结点的向下兄弟结点
     */
    HSTREEITEM GetNextItem(HSTREEITEM hItem,int &nLevel)
    {
        if(hItem==STVI_ROOT)
        {
            nLevel=1;
            return (HSTREEITEM)m_hRootFirst;
        }

        HSTREEITEM hRet=GetChildItem(hItem);
        if(hRet)
        {
            nLevel=1;
            return hRet;
        }
        HSTREEITEM hParent=hItem;
        nLevel=0;
        while(hParent)
        {
            hRet=GetNextSiblingItem(hParent);
            if(hRet) return hRet;
            nLevel--;
            hParent=GetParentItem(hParent);
        }
        return NULL;
    }
        
    void SortChildren(HSTREEITEM hItem,int (__cdecl *funSort)(void *,const void *,const void *), void * pCtx)
    {
        int nChilds = GetChildrenCount(hItem);
        if(nChilds>1)
        {
            HSTREEITEM *pChilds = new HSTREEITEM[nChilds];
            HSTREEITEM hChild = GetChildItem(hItem);
            pChilds[0] = hChild;
            for(int i=1;i<nChilds;i++)
            {
                 hChild = GetNextSiblingItem(hChild);
                 pChilds[i]= hChild;
            }
            //调用qsort排序
            qsort_s(pChilds,nChilds,sizeof(HSTREEITEM),funSort,pCtx);
            //重新整理链表
            for(int i=0;i<nChilds-1;i++)
            {
                HSTREENODE node = (HSTREENODE)pChilds[i];
                node->hNextSibling = (HSTREENODE)pChilds[i+1];
            }
            for(int i=1;i<nChilds;i++)
            {
                HSTREENODE node = (HSTREENODE)pChilds[i];
                node->hPrevSibling = (HSTREENODE)pChilds[i-1];
            }
            HSTREENODE node = (HSTREENODE)pChilds[0];
            node->hPrevSibling = NULL;
            node = (HSTREENODE)pChilds[nChilds-1];
            node->hNextSibling = NULL;
            if(hItem!=STVI_ROOT)
            {
                HSTREENODE parent = (HSTREENODE)hItem;
                parent->hChildFirst = (HSTREENODE)pChilds[0];
                parent->hChildLast = (HSTREENODE)pChilds[nChilds-1];
            }else
            {
                m_hRootFirst = (HSTREENODE)pChilds[0];
                m_hRootLast = (HSTREENODE)pChilds[nChilds-1];
            }
            delete []pChilds;
            
        }
        //子节点排序
        HSTREEITEM hChild = GetChildItem(hItem);
        while(hChild)
        {
            HSTREENODE node = (HSTREENODE)hChild;
            if(node->hChildFirst && node->hChildLast && node->hChildFirst != node->hChildLast)
            {
                SortChildren(hChild,funSort,pCtx);
            }
            hChild = GetNextSiblingItem(hChild);
        }

    }
private:
    /**
     * CSTree::FreeNode
     * @brief    采用后序遍历的方式释放结点占用的空间
     * @param    HSTREENODE hsNode -- 当前结点
     *
     * Describe  采用后序遍历的方式释放结点占用的空间
     */
    void FreeNode(HSTREENODE hsNode)
    {
        SASSERT(hsNode);
        HSTREENODE hSibling=(HSTREENODE)GetChildItem((HSTREEITEM)hsNode);
        while(hSibling)
        {
            HSTREENODE hNextSibling=hSibling->hNextSibling;
            FreeNode(hSibling);
            hSibling=hNextSibling;
        }
        if(hsNode!=STVN_ROOT)
        {
            OnNodeFree(hsNode->data);
            delete hsNode;
        }
    }

protected:
    /**
     * CSTree::OnNodeFree
     * @brief    在派生类中实现数据的释放操作
     * @param    T & data -- 数据
     *
     * Describe  在派生类中实现数据的释放操作
     */
    virtual void OnNodeFree(T & data){}

    HSTREENODE  m_hRootFirst;  /**< 第一个根节点 */
    HSTREENODE  m_hRootLast;   /**< 最后一个根节点 */
};

#endif // !defined(AFX_STREE_H__D2332B4E_2C7E_4357_BE22_EC55BF496C1C__INCLUDED_)
