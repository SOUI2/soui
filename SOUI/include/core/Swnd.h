/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       Swnd.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI基础DUI窗口模块
*/

#pragma once
#include "SWindowMgr.h"
#include "SwndContainer-i.h"
#include "interface/slayout-i.h"
#include <interface/saccproxy-i.h>
#include "helper/STimerEx.h"
#include "helper/SwndMsgCracker.h"
#include "layout/SLayoutSize.h"
#include "event/EventSubscriber.h"
#include "event/events.h"
#include "event/EventSet.h"
#include "res.mgr/SStylePool.h"
#include "res.mgr/SSkinPool.h"
#include "SwndStyle.h"
#include "SSkin.h"
#include <OCIdl.h>

#define SC_WANTARROWS     0x0001      /* Control wants arrow keys         */
#define SC_WANTTAB        0x0002      /* Control wants tab keys           */
#define SC_WANTRETURN     0x0004      /* Control wants return keys        */
#define SC_WANTCHARS      0x0008      /* Want WM_CHAR messages            */
#define SC_WANTALLKEYS    0xFFFF      /* Control wants all keys           */
#define SC_WANTSYSKEY     0x80000000    /* System Key */

#define ICWND_FIRST    ((SWindow*)-1)   /*子窗口插入在开头*/
#define ICWND_LAST    NULL              /*子窗口插入在末尾*/

#define UM_SETSCALE		 (WM_USER+1000)
#define UM_SETLANGUAGE   (WM_USER+1001)
#define UM_SETCOLORIZE	 (WM_USER+1002)

namespace SOUI
{

    /////////////////////////////////////////////////////////////////////////
    enum {NormalShow=0,ParentShow=1};    //提供WM_SHOWWINDOW消息识别是父窗口显示还是要显示本窗口
    enum {NormalEnable=0,ParentEnable=1};    //提供WM_ENABLE消息识别是父窗口可用还是直接操作当前窗口

    
    class STimerID
    {
    public:
        DWORD    Swnd:24;        //窗口句柄,如果窗口句柄超过24位范围，则不能使用这种方式设置定时器
        DWORD    uTimerID:7;        //定时器ID，一个窗口最多支持128个定时器。
        DWORD    bSwndTimer:1;    //区别通用定时器的标志，标志为1时，表示该定时器为SWND定时器

        STimerID(SWND hWnd,char id)
        {
            SASSERT(hWnd<0x00FFFFFF && id>=0);
            bSwndTimer=1;
            Swnd=hWnd;
            uTimerID=id;
        }
        STimerID(DWORD dwID)
        {
            memcpy(this,&dwID,sizeof(DWORD));
        }
        operator DWORD &() const
        {
            return *(DWORD*)this;
        }
    };


    class SOUI_EXP SPainter
    {
    public:
        SPainter(): oldTextColor(CR_INVALID)
        {
        }

        CAutoRefPtr<IFont> oldFont;
        COLORREF           oldTextColor;
    };

    class SOUI_EXP SMsgHandleState
    {
    public:
        SMsgHandleState():m_bMsgHandled(FALSE)
        {

        }

        BOOL IsMsgHandled() const
        {
            return m_bMsgHandled;
        }

        void SetMsgHandled(BOOL bHandled)
        {
            m_bMsgHandled = bHandled;
        }

        BOOL m_bMsgHandled;
    };


    //////////////////////////////////////////////////////////////////////////
    // SWindow
    //////////////////////////////////////////////////////////////////////////

    typedef enum tagGW_CODE
    {
        GSW_FIRSTCHILD=0,
        GSW_LASTCHILD,
        GSW_PREVSIBLING,
        GSW_NEXTSIBLING,
        GSW_PARENT,
        GSW_OWNER,
    } GW_CODE;

    typedef struct SWNDMSG
    {
        UINT uMsg;
        WPARAM wParam;
        LPARAM lParam;
    } *PSWNDMSG;

    struct SwndToolTipInfo
    {
        SWND    swnd;       //拥有tooltip的窗口
        DWORD   dwCookie;   //tooltip在窗口内的ID，对应一个窗口不同区域显示不同tip的情况，一般可以不提供
        CRect   rcTarget;   //tooltip感应区
        SStringT strTip;    //top字符串
    };

	enum{
		HRET_FLAG_STYLE = (1<<16),
		HRET_FLAG_LAYOUT = (1<<17),
		HRET_FLAG_LAYOUT_PARAM = (1<<18),
	};

	struct ITrCtxProvider
	{
		virtual const SStringW & GetTrCtx() const = 0;
	};

	class  SOUI_EXP STrText
	{
	public:
		STrText(ITrCtxProvider *pProvider = NULL);
		void SetCtxProvider(ITrCtxProvider *pProvider);

		void SetText(const SStringT& strText);

		SStringT GetText(BOOL bRawText=FALSE) const;

		void TranslateText();

	protected:
		ITrCtxProvider * pTrCtxProvider;

		SStringT strRaw;	//原始字符串
		SStringT strTr;		//翻译后的字符串
	};

    /**
    * @class     SWindow
    * @brief     SOUI窗口基类 
    * 
    * Describe   SOUI窗口基类,实现窗口的基本接口
    */
    class SOUI_EXP SWindow :  public SObject
		, public SMsgHandleState
		, public ITrCtxProvider
        , public TObjRefImpl2<IObjRef,SWindow>
    {
        SOUI_CLASS_NAME_EX(SWindow, L"window",Window)
        friend class SwndLayoutBuilder;
        friend class SWindowRepos;
        friend class SHostWnd;
        friend class SwndContainerImpl;
        friend class FocusSearch;
    public:
        SWindow();

        virtual ~SWindow();

	public:
		
		IAccessible * GetAccessible();
		IAccProxy * GetAccProxy();
		void accNotifyEvent(DWORD dwEvt);
	public:

		ILayout * GetLayout() const{
			return m_pLayout;
		}

		template<class T>
		T * GetLayoutParamT() const
		{
			return sobj_cast<T>(GetLayoutParam());
		}

		virtual ILayoutParam * GetLayoutParam() const
		{
			return m_pLayoutParam;
		}

		bool SetLayoutParam(ILayoutParam * pLayoutParam);

		bool IsFloat() const{
			return !!m_bFloat;
		}

		bool IsDisplay() const{
			return m_bDisplay!=0;
		}

		/**
        * GetNextLayoutChild
        * @brief    枚举布局脏子窗口
		* @param	SWindow *pCurChild -- 当前子窗口指针
        * @return   SWindow * -- 下一个布局子窗口 
        *
        * Describe  
        */
		SWindow * GetNextLayoutChild(SWindow *pCurChild) const;

    public://SWindow状态相关方法
        /**
        * GetSwnd
        * @brief    获得窗口句柄
        * @return   SWND 
        *
        * Describe  
        */
        SWND GetSwnd() const;
        SWindow *GetWindow(int uCode) const;    

		SWindow * GetChild(int iChild);

        /**
        * GetWindowText
        * @brief    获得窗口文本
        * @return   SStringT 
        *
        * Describe  
        */
        virtual SStringT GetWindowText(BOOL bRawText=FALSE);

        /**
        * SetWindowText
        * @brief    设置窗口文本
        * @param    LPCTSTR lpszText --  窗口文本
        * @return   void 
        *
        * Describe  
        */
        virtual void SetWindowText(LPCTSTR lpszText);

		virtual void SetToolTipText(LPCTSTR pszText);

		virtual SStringT GetToolTipText();

        virtual LPCWSTR GetName() const {return m_strName;}
        void SetName(LPCWSTR pszName){m_strName=pszName;}

        virtual int GetID() const{return m_nID;}
        void SetID(int nID){m_nID=nID;}

        /**
         * GetEventSet
         * @brief    获取SEventSet对象
         * @return   SEventSet * SEventSet对象指针
         *
         * Describe  
         */
        SEventSet * GetEventSet(){return &m_evtSet;}

        /**
         * GetStyle
         * @brief    获取style对象
         * @return   SwndStyle& style对象
         *
         * Describe  
         */
        SwndStyle& GetStyle();

        BOOL IsChecked();
        void SetCheck(BOOL bCheck);

        BOOL IsDisabled(BOOL bCheckParent = FALSE);
        void EnableWindow( BOOL bEnable,BOOL bUpdate=FALSE);

        BOOL IsVisible(BOOL bCheckParent = FALSE);
        void SetVisible(BOOL bVisible,BOOL bUpdate=FALSE);

        void SetOwner(SWindow *pOwner);
        SWindow *GetOwner();

        DWORD GetState(void);
        DWORD ModifyState(DWORD dwStateAdd, DWORD dwStateRemove,BOOL bUpdate=FALSE);

        ISwndContainer *GetContainer();
		const ISwndContainer* GetContainer() const;
        void SetContainer(ISwndContainer *pContainer);

        /**
        * GetUserData
        * @brief    读userdata
        * @return   ULONG_PTR 
        *
        * Describe  
        */
        ULONG_PTR GetUserData();
        /**
        * SetUserData
        * @brief    设置userdata
        * @param    ULONG_PTR uData --  原来的userdata
        * @return   ULONG_PTR 
        *
        * Describe  
        */
        ULONG_PTR SetUserData(ULONG_PTR uData);

        /**
        * GetTextAlign
        * @brief    获得文本的对齐标志
        * @return   UINT 
        *
        * Describe  获得文本的对齐标志
        */
        UINT GetTextAlign();    

        /**
        * GetWindowRect
        * @brief    获得窗口在宿主中的位置
        * @param    [out] LPRECT prect --  窗口矩形
        * @return   void 
        *
        * Describe  
        */    
        void GetWindowRect(LPRECT prect);

        CRect GetWindowRect() {
            CRect rc;
            GetWindowRect(&rc);
            return rc;
        }
                
        /**
        * GetClientRect
        * @brief    获得窗口的客户区
        * @param    [out] LPRECT pRect --  窗口矩形
        * @return   void 
        *
        * Describe  
        */
        virtual void GetClientRect(LPRECT pRect) const;
        
        virtual CRect GetClientRect() const;
        
        /**
        * ContainPoint
        * @brief    判断一个点是不是在窗口区域内
        * @param    const POINT &pt --  测试点
        * @param    BOOL bClientOnly --  只判断客户区标志
        * @return   BOOL -- TRUE:窗口包含目标点 
        *
        * Describe  
        */
        virtual BOOL IsContainPoint(const POINT &pt,BOOL bClientOnly) const;
        
        /**
        * DoColorize
        * @brief    调节窗口色调
        * @param    COLORREF cr --  目标颜色
        * @return   void
        *
        * Describe  
        */
        void DoColorize(COLORREF cr);

		/**
         * GetColorizeColor
         * @brief    获得当前调色状态
         * @return   COLORREF 调色值
         * Describe  
         */
		COLORREF GetColorizeColor() const;

    protected:

        /**
        * OoColorize
        * @brief    调节窗口色调
        * @param    COLORREF cr --  目标颜色
        * @return   void
        *
        * Describe  
        */
        virtual void OnColorize(COLORREF cr);
        
    public://窗口树结构相关方法
        /**
         * GetParent
         * @brief    获得父窗口
         * @return   SWindow * 父窗口指针
         *
         * Describe  
         */
        SWindow *GetParent() const;

        /**
         * GetTopLevelParent
         * @brief    获得顶层窗口
         * @return   SWindow * 顶层窗口指针
         *
         * Describe  
         */
        SWindow * GetTopLevelParent() const;
        
        /**
         * GetRoot
         * @brief    Get root window
         * @return   SWindow * -- root window
         * Describe  
         */    
        SWindow * GetRoot(){return GetTopLevelParent();}
        
        /**
         * IsDesendant
         * @brief    determining whether this window is a descendant of pWnd or not
         * @param    const SWindow * pWnd --  the tested ascendant window
         * @return   BOOL -- TRUE: descendant
         * Describe  
         */    
        BOOL IsDescendant(const SWindow *pWnd) const;

        /**
         * DestroyWindow
         * @brief    销毁窗口
         * @return   BOOL -- TRUE: 成功
         * Describe  
         */    
        BOOL DestroyWindow();
        
        /**
         * BringWindowToTop
         * @brief    将当前窗口的Z-order提到同级子窗口最前
         * @return   void 
         *
         * Describe  
         */
        void BringWindowToTop();

        /**
        * FindChildByID
        * @brief    通过ID查找对应的子窗口
        * @param    int nID --  窗口ID
        * @param    int nDeep --  搜索深度,-1代表无限度
        * @return   SWindow* 
        *
        * Describe  
        */
        SWindow* FindChildByID(int nID, int nDeep =-1);

        /**
        * FindChildByID2
        * @brief    FindChildByID的模板类，支持类型转换
        * @param    int nID --  窗口ID
        * @param    int nDeep --  搜索深度,-1代表无限度
        * @return   T* 
        *
        * Describe  
        */
        template<class T>
        T* FindChildByID2(int nID, int nDeep =-1)
        {
            SWindow *pTarget = FindChildByID(nID,nDeep);

            if(!pTarget || !pTarget->IsClass(T::GetClassName()))
            {
                SLOGFMTD(_T("FindChildByID2 Failed, no window of class [%s] with id of [%d] was found within [%d] levels"),T::GetClassName(),nID,nDeep);
                return NULL;
            }
            return (T*)pTarget;
        }

        /**
        * FindChildByName
        * @brief    通过名字查找子窗口
        * @param    const SStringW & pszName --  窗口name属性
        * @param    int nDeep --  搜索深度,-1代表无限度
        * @return   SWindow* 
        *
        * Describe  
        */
        SWindow* FindChildByName(LPCWSTR strName , int nDeep =-1);

        SWindow* FindChildByName(LPCSTR strName , int nDeep =-1)
        {
            return FindChildByName(S_CA2W(strName),nDeep);
        }

        /**
        * FindChildByName2
        * @brief    FindChildByName的模板类，支持类型转换
        * @param    LPCWSTR pszName --  窗口name属性
        * @param    int nDeep --  搜索深度,-1代表无限度
        * @return   T* 
        *
        * Describe  
        */
        template<class T>
        T* FindChildByName2(LPCWSTR pszName, int nDeep =-1)
        {
            SWindow *pTarget = FindChildByName(pszName,nDeep);
            if(!pTarget || !pTarget->IsClass(T::GetClassName()))
            {
				SLOGFMTD(_T("FindChildByName2 Failed, no window of class [%s] with name of [%s] was found within [%d] levels"), T::GetClassName(), pszName, nDeep);
                return NULL;
            }
            return (T*)pTarget;
        }

        template<class T>
        T* FindChildByName2(LPCSTR pszName, int nDeep =-1)
        {
            return FindChildByName2<T>(S_CA2W(pszName),nDeep);
        }

        /**
        * CreateChildren
        * @brief    从XML创建子窗口
        * @param    LPCWSTR pszXml --  合法的utf16编码XML字符串
        * @return   SWindow * 创建成功的的最后一个窗口
        *
        * Describe  
        */
        SWindow *CreateChildren(LPCWSTR pszXml);
        
         /**
         * GetChildrenCount
         * @brief    获得子窗口数量
         * @return   UINT 子窗口数量 
         *
         * Describe  
         */
        UINT GetChildrenCount() const;

        /**
         * InsertChild
         * @brief    在窗口树中插入一个子窗口
         * @param    SWindow * pNewChild --  子窗口对象
         * @param    SWindow * pInsertAfter --  插入位置
         * @return   void 
         *
         * Describe  一般用于UI初始化的时候创建，插入的窗口不会自动进入布局流程
         */
        void InsertChild(SWindow *pNewChild,SWindow *pInsertAfter=ICWND_LAST);

        /**
         * RemoveChild
         * @brief    从窗口树中移除一个子窗口对象
         * @param    SWindow * pChild --  子窗口对象
         * @return   BOOL 
         *
         * Describe  子窗口不会自动释放
         */
        BOOL RemoveChild(SWindow *pChild);
        
        /**
         * DestroyChild
         * @brief    销毁一个子窗口
         * @param    SWindow * pChild --  子窗口对象
         * @return   BOOL 
         *
         * Describe  先调用RemoveChild，再调用pChild->Release来释放子窗口对象
         */
        BOOL DestroyChild(SWindow *pChild);
        
    public://窗口消息相关方法
        /**
        * SSendMessage
        * @brief    向SWND发送条窗口消息
        * @param    UINT Msg --  消息类型
        * @param    WPARAM wParam --  参数1
        * @param    LPARAM lParam --  参数2
        * @param [out] BOOL * pbMsgHandled -- 消息处理标志 
        * @return   LRESULT 消息处理状态，依赖于消息类型
        *
        * Describe  
        */
        LRESULT SSendMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0,BOOL *pbMsgHandled=NULL);

        /**
        * SSendMessage
        * @brief    向所有子窗口发送消息
        * @param    uMsg --  消息类型
		* @param	wParam -- wParam
		* @param	lParam -- lParam
        * @return   void
        *
        * Describe  遍历所有子窗口发送消息
        */
        void SDispatchMessage(UINT uMsg,WPARAM wParam,LPARAM lParam);

        /**
        * GetCurMsg
        * @brief    获得当前正在处理的消息
        * @return   PSWNDMSG 
        *
        * Describe  
        */
        PSWNDMSG GetCurMsg()
        {
            return m_pCurMsg;
        }
        
        BOOL IsMsgTransparent();

    public://操控SWindow的方法

        void SetFocus();
        void KillFocus();
        BOOL IsFocused();
        
        void Invalidate();
        void InvalidateRect(LPCRECT lprect);
        
        /**
        * InvalidateRect
        * @brief    刷新窗口
        * @param    const CRect& rect --  刷新区域
        * @param    BOOL bFromThis --  是否是由本窗口触发的刷新，子窗口的刷新导致父窗口刷新时bFromThis为FALSE
        * @return   void 
        *
        * Describe 
        */
        void InvalidateRect(const CRect& rect,BOOL bFromThis=TRUE);
        
        void LockUpdate();
        void UnlockUpdate();
        BOOL IsUpdateLocked();
        void UpdateWindow();

        /**
        * Move
        * @brief    将窗口移动到指定位置
        * @param    LPCRECT prect --  新的窗口坐标矩形，为NULL则恢复窗口坐标自动计算
        * @return   void 
        *
        * Describe 
        */
        void Move(LPCRECT prect);

        /**
        * Move
        * @brief    将窗口移动到指定位置
        * @param    int x --  left
        * @param    int y --  top
        * @param    int cx --  width
        * @param    int cy --  height
        * @return   void 
        *
        * Describe 
        * @see     Move(LPRECT prect)
        */
        void Move(int x,int y, int cx=-1,int cy=-1);


        /**
        * SetWindowRgn
        * @brief    设置窗口区域
        * @param    IRegion *pRgn --  有效区域,区域左上角坐标为(0,0)
        * @param    BOOL bRedraw -- 重绘标志
        * @return   void 
        *
        * Describe 
        */
        void SetWindowRgn(IRegion *pRgn,BOOL bRedraw=TRUE);
        
        /**
        * GetWindowRgn
        * @brief    获取窗口区域
        * @param    IRegion *pRgn --  有效区域,区域左上角坐标为(0,0)
        * @return   BOOL -- TRUE:有区域
        *
        * Describe 
        */
        BOOL GetWindowRgn(IRegion *pRgn);
        
        /**
        * SetTimer
        * @brief    利用窗口定时器来设置一个ID为0-127的SWND定时器
        * @param    char id --  定时器ID
        * @param    UINT uElapse --  延时(MS)
        * @return   BOOL 
        *
        * Describe  参考::SetTimer
        */
        BOOL SetTimer(char id,UINT uElapse);

        /**
        * KillTimer
        * @brief    删除一个SWND定时器
        * @param    char id --  定时器ID
        * @return   void 
        *
        * Describe  
        */
        void KillTimer(char id);

        /**
        * SetTimer2
        * @brief    利用函数定时器来模拟一个兼容窗口定时器
        * @param    UINT_PTR id --  定时器ID
        * @param    UINT uElapse --  延时(MS)
        * @return   BOOL 
        *
        * Describe  由于SetTimer只支持0-127的定时器ID，SetTimer2提供设置其它timerid
        *           能够使用SetTimer时尽量不用SetTimer2，在Kill时效率会比较低
        */
        BOOL SetTimer2(UINT_PTR id,UINT uElapse);

        /**
        * KillTimer2
        * @brief    删除一个SetTimer2设置的定时器
        * @param    UINT_PTR id --  SetTimer2设置的定时器ID
        * @return   void 
        *
        * Describe  需要枚举定时器列表
        */
        void KillTimer2(UINT_PTR id);

        /**
        * AnimateWindow
        * @brief    窗口动画效果
        * @param    DWORD dwTime --  执行时间
        * @param    DWORD dwFlags --  执行模式
        * @return   BOOL 
        *
        * Describe  
        */
        BOOL AnimateWindow(DWORD dwTime,DWORD dwFlags);
        
        SWND GetCapture();
        SWND SetCapture();
        BOOL ReleaseCapture();

    public:// Virtual functions

		virtual int GetScale() const;

        /**
        * IsSiblingsAutoGroupped
        * @brief    同类窗口自动成组标志
        * @return   BOOL 
        *
        * Describe  主要是给RadioButton用的
        */
        virtual BOOL IsSiblingsAutoGroupped(){return FALSE;}

        /**
        * GetSelectedSiblingInGroup
        * @brief    获得在一个group中选中状态的窗口
        * @return   SWindow * 
        *
        * Describe  不是group中的窗口时返回NULL
        */
        virtual SWindow * GetSelectedSiblingInGroup(){return NULL;}

        /**
        * GetSelectedChildInGroup
        * @brief    获取有选择状态的子窗口
        * @return   SWindow * -- 选中状态窗口
        * Describe  
        */    
        virtual SWindow * GetSelectedChildInGroup();


        // Set current cursor, when hover
        virtual BOOL OnSetCursor(const CPoint &pt);

        /**
        * OnUpdateToolTip
        * @brief    处理tooltip
        * @param    const CPoint & pt --  测试点
        * @param [out]  SwndToolTipInfo & tipInfo -- tip信息 
        * @return   BOOL -- TRUE:更新tooltip，FALSE:不更新tooltip
        *
        * Describe  
        */
        virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo);

        virtual void OnStateChanging(DWORD dwOldState,DWORD dwNewState);
        virtual void OnStateChanged(DWORD dwOldState,DWORD dwNewState);

        virtual BOOL InitFromXml(pugi::xml_node xmlNode);
        virtual BOOL CreateChildren(pugi::xml_node xmlNode);
        
        /**
        * RequestRelayout
        * @brief    请求重新布局
        * @param    SWindow *pSource --  请求重新布局的源窗口
        * @return   void 
        *
        * Describe  
        */
		void RequestRelayout();
        virtual void RequestRelayout(SWND hSource,BOOL bSourceResizable);
        
        virtual void UpdateLayout();
        
		virtual void OnContentChanged();

        virtual SStringW tr(const SStringW &strSrc);

        virtual SWND SwndFromPoint(CPoint ptHitTest, BOOL bOnlyText);

        virtual BOOL FireEvent(EventArgs &evt);

        virtual UINT OnGetDlgCode();

        virtual BOOL IsFocusable();

        virtual BOOL OnNcHitTest(CPoint pt);

        virtual BOOL IsClipClient()
        {
            return m_bClipClient;
        }

        /**
        * UpdateChildrenPosition
        * @brief    更新子窗口位置
        * @return   void 
        *
        * Describe  
        */
        virtual void UpdateChildrenPosition();
        
        /**
        * OnUpdateFloatPosition
        * @brief    给float窗口一个自己实现自动布局的机会
        * @param    const CRect & rcParent --  父窗口位置
        * @return   void 
        *
        * Describe  2016年3月2日新增加方法
        */
		virtual void OnUpdateFloatPosition(const CRect & rcParent){};
                
        /**
        * OnRelayout
        * @brief    窗口位置发生变化
        * @param    const CRect & rcWnd --  新位置
        * @return   void 
        *
        * Describe  窗口位置发生变化,更新窗口及计算子窗口位置
        */
		virtual BOOL OnRelayout(const CRect &rcWnd);

        /**
        * GetChildrenLayoutRect
        * @brief    获得子窗口的布局空间
        * @return   CRect 
        *
        * Describe  通常是客户区，但是tab,group这样的控件不一样
        */
        virtual CRect GetChildrenLayoutRect();

        /**
        * GetDesiredSize
        * @brief    当没有指定窗口大小时，通过如皮肤计算窗口的期望大小
        * @param    LPRECT pRcContainer --  容器位置
        * @return   CSize 
        *
        * Describe  
        */
        virtual CSize GetDesiredSize(LPCRECT pRcContainer);

        /**
        * GetDesiredSize
        * @brief    当没有指定窗口大小时，通过如皮肤计算窗口的期望大小
        * @param    int nParentWid -- 容器宽度，<0代表容器宽度依赖当前窗口宽度
        * @param    int nParentHei -- 容器高度，<0代表容器高度依赖当前窗口高度
        * @return   CSize 
        *
        * Describe  
        */
		virtual CSize GetDesiredSize(int nParentWid, int nParentHei);

        /**
         * NeedRedrawWhenStateChange
         * @brief    定义状态改变时控件是否重绘
         * @return   BOOL -- TRUE：重绘
         *
         * Describe  
         */
        virtual BOOL NeedRedrawWhenStateChange();

        /**
         * GetTextRect
         * @brief    计算窗口中文本的显示位置
         * @param  [out]  LPRECT pRect --  期望文本的显示位置
         * @return   void 
         *
         * Describe  
         */
        virtual void GetTextRect(LPRECT pRect);
        
        /**
         * DrawText
         * @brief    绘制文本方法
         * @param    IRenderTarget * pRT --  渲染RT
         * @param    LPCTSTR pszBuf --  文本
         * @param    int cchText --  文本长度
         * @param    LPRECT pRect --  限制渲染范围
         * @param    UINT uFormat --  格式
         * @return   void 
         *
         * Describe  子控件重载该方法来定义具体绘制行为
         */
        virtual void DrawText(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat);
        
        /**
         * DrawFocus
         * @brief    绘制控件的焦点状态
         * @param    IRenderTarget * pRT --  渲染RT
         * @return   void 
         *
         * Describe  默认绘制一个虚线框
         */
        virtual void DrawFocus(IRenderTarget *pRT);
        
        /**
        * BeforePaint
        * @brief    为RT准备好当前窗口的绘图环境
        * @param    IRenderTarget * pRT --  
        * @param    SPainter & painter --  
        * @return   void 
        *
        * Describe  
        */
        virtual void BeforePaint(IRenderTarget *pRT, SPainter &painter);

        /**
        * AfterPaint
        * @brief    恢复由BeforePaint设置的RT状态
        * @param    IRenderTarget * pRT --  
        * @param    SPainter & painter --  
        * @return   void 
        *
        * Describe  
        */
        virtual void AfterPaint(IRenderTarget *pRT, SPainter &painter);
        
		virtual const SStringW & GetTrCtx() const;
    public://caret相关方法
        virtual BOOL CreateCaret(HBITMAP pBmp,int nWid,int nHeight);
        virtual void ShowCaret(BOOL bShow);   
        virtual void SetCaretPos(int x,int y); 
    public://render相关方法
        /**
        * RedrawRegion
        * @brief    将窗口及子窗口内容绘制到RenderTarget
        * @param    IRenderTarget * pRT --  渲染目标RT
        * @param    IRegion * pRgn --  渲染区域，为NULL时渲染整个窗口
        * @return   void 
        *
        * Describe  
        */
        void RedrawRegion(IRenderTarget *pRT, IRegion *pRgn);

        /**
        * GetRenderTarget
        * @brief    获取一个与SWND窗口相适应的内存DC
        * @param    LPCRECT pRc --  RT范围
        * @param    DWORD gdcFlags --  同OLEDCFLAGS
        * @return   IRenderTarget * 
        *
        * Describe  使用ReleaseRenderTarget释放
        */
        IRenderTarget * GetRenderTarget(LPCRECT pRc=NULL,DWORD gdcFlags=OLEDC_NODRAW,BOOL bClientRT=TRUE);

        /**
        * GetRenderTarget
        * @brief    获取一个与SWND窗口相适应的内存DC
        * @param    DWORD gdcFlags --  同OLEDCFLAGS
        * @param    IRegion *pRgn --  RT范围
        * @return   IRenderTarget * 
        *
        * Describe  使用ReleaseRenderTarget释放
        */
        IRenderTarget * GetRenderTarget(DWORD gdcFlags,IRegion *pRgn);


        /**
        * ReleaseRenderTarget
        * @brief    
        * @param    IRenderTarget * pRT --  释放由GetRenderTarget获取的RT
        * @return   void 
        *
        * Describe  
        */
        void ReleaseRenderTarget(IRenderTarget *pRT);

        /**
        * PaintBackground
        * @brief    画窗口的背景内容
        * @param    IRenderTarget * pRT --  目标RT
        * @param    LPRECT pRc --  目标位置
        * @return   void 
        *
        * Describe  目标位置必须在窗口位置内
        */
        void PaintBackground(IRenderTarget *pRT,LPRECT pRc);

        /**
        * PaintForeground
        * @brief    画窗口的前景内容
        * @param    IRenderTarget * pRT --  目标RT
        * @param    LPRECT pRc --  目标位置
        * @return   void 
        *
        * Describe  目标位置必须在窗口位置内,不包括当前窗口的子窗口
        */
        void PaintForeground(IRenderTarget *pRT,LPRECT pRc);
		void PaintForeground2(IRenderTarget *pRT, LPRECT pRc);
        /**
        * BeforePaintEx
        * @brief    为DC准备好当前窗口的绘图环境,从顶层窗口开始设置
        * @param    IRenderTarget * pRT --  渲染RT
        * @return   void 
        *
        * Describe  一般应该和CreateRanderTarget配合使用
        */
        void BeforePaintEx(IRenderTarget *pRT);

        
        /**
         * GetScriptModule
         * @brief    获得脚本模块
         * @return   IScriptModule * -- 模块模块指针
         * Describe  
         */    
        IScriptModule * GetScriptModule();

    public:

        /**
        * FireCommand
        * @brief    激活窗口的EVT_CMD事件
        * @return   BOOL-- true:EVT_CMD事件被处理
        *
        * Describe  
        */
        BOOL FireCommand();

        /**
        * FireCtxMenu
        * @brief    激活快捷菜单事件
        * @param    CPoint pt --  鼠标点击位置
        * @return   BOOL -- true:外部处理了快捷菜单事件
        *
        * Describe  
        */
        BOOL FireCtxMenu(CPoint pt);
        

    protected://cache相关方法

        /**
        * IsCacheDirty
        * @brief    查询Cache的Dirty标志
        * @return   bool -- true表示Cache已经Dirty
        * Describe  
        */    
        bool IsCacheDirty() const  {return IsDrawToCache()&&m_bCacheDirty;}
        
        /**
        * MarkCacheDirty
        * @brief    标记Cache的Dirty标志
        * @param    bool bDirty --  Dirty标志
        * @return   void
        * Describe  
        */    
        void MarkCacheDirty(bool bDirty) {m_bCacheDirty = bDirty;}


		/**
        * IsLayoutDirty
        * @brief    检测布局Dirty标志
		* @return   bool -- true表示layout已经Dirty
        * Describe  
        */    
		bool IsLayoutDirty() const {return m_layoutDirty != dirty_clean;}

        /**
        * IsDrawToCache
        * @brief    查看当前是否是把窗口内容绘制到cache上
        * @return   bool -- true绘制到cache上。
        * Describe  
        */    
        virtual bool IsDrawToCache() const;


        /**
         * IsLayeredWindow
         * @brief    确定渲染时子窗口的内容是不是渲染到当前窗口的缓存上
         * @return   BOOL -- TREU:子窗口的内容先渲染到this的缓存RT上
         * Describe  
         */    
        virtual BOOL IsLayeredWindow() const;
    
        IRenderTarget * GetLayerRenderTarget();

    protected://helper functions

		SWindow* _FindChildByID(int nID, int nDeep);
		SWindow* _FindChildByName(const SStringW & strName, int nDeep);

        void _Update();
        
    
        /**
         * _GetCurrentRenderContainer
         * @brief    获得当前窗口所属的渲染层宿主窗口
         * @return   SWindow * -- 渲染层宿主窗口
         * Describe  
         */    
        SWindow * _GetCurrentLayeredWindow();

        /**
        * _GetRenderTarget
        * @brief    获取一个与SWND窗口相适应的内存DC
        * @param  [in,out]  CRect & rcGetRT --  RT范围,保存最后的有效绘制区
        * @param    DWORD gdcFlags --  同OLEDCFLAGS
        * @param    IRegion *pRgn --  绘制区域
        * @return   IRenderTarget * 
        *
        * Describe  使用ReleaseRenderTarget释放
        */
        IRenderTarget * _GetRenderTarget(CRect & rcGetRT,DWORD gdcFlags,IRegion *pRgn);


        /**
        * _ReleaseRenderTarget
        * @brief    
        * @param    IRenderTarget * pRT --  释放由GetRenderTarget获取的RT
        * @return   void 
        *
        * Describe  
        */
        void _ReleaseRenderTarget(IRenderTarget *pRT);

        //将窗口内容绘制到RenderTarget上
        void _PaintClient(IRenderTarget *pRT);
        void _PaintNonClient(IRenderTarget *pRT);
		void _RedrawNonClient();
        void _PaintRegion(IRenderTarget *pRT, IRegion *pRgn,UINT iZorderBegin,UINT iZorderEnd);
        void _PaintRegion2(IRenderTarget *pRT, IRegion *pRgn,UINT iZorderBegin,UINT iZorderEnd);

        void DrawDefFocusRect(IRenderTarget *pRT,CRect rc);
        void DrawAniStep(CRect rcFore,CRect rcBack,IRenderTarget *pRTFore,IRenderTarget * pRTBack,CPoint ptAnchor);
        void DrawAniStep( CRect rcWnd,IRenderTarget *pRTFore,IRenderTarget * pRTBack,BYTE byAlpha);
        
        void UpdateCacheMode();
        void UpdateLayeredWindowMode();

        void TestMainThread();
        
		void GetScaleSkin(ISkinObj * &pSkin,int nScale);
    protected:// Message Handler

        /**
        * SwndProc
        * @brief    默认的消息处理函数
        * @param    UINT uMsg --  消息类型
        * @param    WPARAM wParam --  参数1
        * @param    LPARAM lParam --  参数2
        * @param    LRESULT & lResult --  消息返回值
        * @return   BOOL 是否被处理
        *
        * Describe  在消息映射表中没有处理的消息进入该函数处理
        */
        virtual BOOL SwndProc(UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT & lResult)
        {
            return FALSE;
        }

        int OnCreate(LPVOID);

        void OnSize(UINT nType, CSize size);

        void OnDestroy();

        BOOL OnEraseBkgnd(IRenderTarget *pRT);

        void OnPaint(IRenderTarget *pRT);

        void OnNcPaint(IRenderTarget *pRT);

        void OnShowWindow(BOOL bShow, UINT nStatus);

        void OnEnable(BOOL bEnable,UINT nStatus);

        void OnLButtonDown(UINT nFlags,CPoint pt);

        void OnLButtonUp(UINT nFlags,CPoint pt);

        void OnRButtonDown(UINT nFlags, CPoint point);

        void OnRButtonUp(UINT nFlags, CPoint point);

        void OnMouseMove(UINT nFlags,CPoint pt) {}

        void OnMouseHover(WPARAM wParam, CPoint ptPos);

        void OnMouseLeave();

        BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

        void OnSetFocus(SWND wndOld);
        void OnKillFocus(SWND wndFocus);
        
		LRESULT OnSetScale(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnSetLanguage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnSetColorize(UINT uMsg, WPARAM wParam, LPARAM lParam);

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
            MSG_WM_NCPAINT_EX(OnNcPaint)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_SIZE(OnSize)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_SHOWWINDOW(OnShowWindow)
            MSG_WM_ENABLE_EX(OnEnable)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_RBUTTONDOWN(OnRButtonDown)
            MSG_WM_RBUTTONUP(OnRButtonUp)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MSG_WM_MOUSEHOVER(OnMouseHover)
            MSG_WM_MOUSELEAVE(OnMouseLeave)
            MSG_WM_MOUSEWHEEL(OnMouseWheel)
            MSG_WM_SETFOCUS_EX(OnSetFocus)
            MSG_WM_KILLFOCUS_EX(OnKillFocus)
			MESSAGE_HANDLER_EX(UM_SETLANGUAGE,OnSetLanguage)
			MESSAGE_HANDLER_EX(UM_SETSCALE,OnSetScale)
			MESSAGE_HANDLER_EX(UM_SETCOLORIZE,OnSetColorize)
        WND_MSG_MAP_END_BASE()  //消息不再往基类传递，此外使用WND_MSG_MAP_END_BASE而不是WND_MSG_MAP_END

    protected:
        // 属性处理函数
        HRESULT OnAttrVisible(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrEnable(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrDisplay(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrCache(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrAlpha(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrSkin(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrLayout(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrClass(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrTrackMouseEvent(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrLayeredWindow(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrID(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrName(const SStringW& strValue, BOOL bLoading);
		HRESULT OnAttrTip(const SStringW& strValue, BOOL bLoading);

        HRESULT DefAttributeProc(const SStringW & strAttribName,const SStringW & strValue, BOOL bLoading);

        virtual HRESULT AfterAttribute(const SStringW & strAttribName,const SStringW & strValue, BOOL bLoading,HRESULT hr);

		virtual SStringW GetAttribute(const SStringW & strAttr) const;

        SOUI_ATTRS_BEGIN()
			ATTR_CUSTOM(L"layout",OnAttrLayout)
            ATTR_CUSTOM(L"class", OnAttrClass)      //解析style
            ATTR_CUSTOM(L"id",OnAttrID)
            ATTR_CUSTOM(L"name",OnAttrName)
            ATTR_CUSTOM(L"skin", OnAttrSkin)        //直接获得皮肤对象
            ATTR_SKIN(L"ncskin", m_pNcSkin, TRUE)   //直接获得皮肤对象
            ATTR_INT(L"data", m_uData, 0 )
			ATTR_I18NSTRT(L"text",m_strText,TRUE)	//从text属性中获取显示文本
			ATTR_STRINGW(L"trCtx",m_strTrCtx,FALSE) 
            ATTR_CUSTOM(L"enable", OnAttrEnable)
            ATTR_CUSTOM(L"visible", OnAttrVisible)
            ATTR_CUSTOM(L"show", OnAttrVisible)
            ATTR_CUSTOM(L"display", OnAttrDisplay)
            ATTR_CUSTOM(L"cache", OnAttrCache)
            ATTR_CUSTOM(L"alpha",OnAttrAlpha)
            ATTR_CUSTOM(L"layeredWindow",OnAttrLayeredWindow)
            ATTR_CUSTOM(L"trackMouseEvent",OnAttrTrackMouseEvent)
			ATTR_CUSTOM(L"tip",OnAttrTip)
            ATTR_BOOL(L"msgTransparent", m_bMsgTransparent, FALSE)
            ATTR_LAYOUTSIZE(L"maxWidth",m_nMaxWidth,FALSE)
            ATTR_BOOL(L"clipClient",m_bClipClient,FALSE)
            ATTR_BOOL(L"focusable",m_bFocusable,FALSE)
            ATTR_BOOL(L"drawFocusRect",m_bDrawFocusRect,TRUE)
            ATTR_BOOL(L"float",m_bFloat,FALSE)
			ATTR_CHAIN(m_style,HRET_FLAG_STYLE)					    //交给SwndStyle处理
			ATTR_CHAIN_PTR(m_pLayout,HRET_FLAG_LAYOUT)				//交给Layout处理
			ATTR_CHAIN_PTR(m_pLayoutParam,HRET_FLAG_LAYOUT_PARAM)	//交给LayoutParam处理
        SOUI_ATTRS_END()


		virtual HRESULT OnLanguageChanged();

		virtual void OnScaleChanged(int scale);

    private:
        CRect               m_rcWindow;         /**< 窗口在容器中的位置,由于它的值包含POS_INIT等，调整为private，不允许派生类中直接访问该变量的值 */

    protected:

		//布局脏标志类型
		enum LayoutDirtyType
		{
			dirty_clean = 0,//干净
			dirty_self = 1, //自己变脏
			dirty_child = 2, //有孩子变脏
		};

        SWND                m_swnd;             /**< 窗口句柄 */
        BOOL                m_bFloat;           /**< 窗口位置固定不动的标志 */

        ISwndContainer *    m_pContainer;       /**< 容器对象 */
        SEventSet           m_evtSet;           /**< 窗口事件集合 */

		CAutoRefPtr<ILayout>  m_pLayout;
		CAutoRefPtr<ILayoutParam> m_pLayoutParam;

        SWindow *           m_pOwner;           /**< 容器Owner，事件分发时，会把事件交给Owner处理 */
        SWindow *           m_pParent;          /**< 父窗口 */
        SWindow *           m_pFirstChild;      /**< 第一子窗口 */
        SWindow *           m_pLastChild;       /**< 最后窗口 */
        SWindow *           m_pNextSibling;     /**< 前一兄弟窗口 */
        SWindow *           m_pPrevSibling;     /**< 后一兄弟窗口 */
        UINT                m_nChildrenCount;   /**< 子窗口数量 */

        SWNDMSG *           m_pCurMsg;          /**< 当前正在处理的窗口消息 */

        SwndStyle           m_style;            /**< 窗口Style，是一组窗口属性 */
        STrText             m_strText;          /**< 窗口文字 */
        STrText             m_strToolTipText;   /**< 窗口ToolTip */
        SStringW            m_strName;          /**< 窗口名称 */
		SStringW			m_strTrCtx;			/**< translate context. empty than use container's tr ctx*/
        int                 m_nID;              /**< 窗口ID */
        UINT                m_uZorder;          /**< 窗口Zorder */
		int                 m_nUpdateLockCnt;   /**< 暂时锁定更新Count，锁定后，不向宿主发送Invalidate */

        DWORD               m_dwState;          /**< 窗口在渲染过程中的状态 */
        DWORD               m_bVisible:1;       /**< 窗口可见状态 */
        DWORD               m_bDisable:1;       /**< 窗口禁用状状态 */
        DWORD               m_bDisplay:1;       /**< 窗口隐藏时是否占位，不占位时启动重新布局 */
        DWORD               m_bClipClient:1;    /**< 窗口绘制时做clip客户区处理的标志,由于clip可能增加计算量，只在绘制可能走出客户区时才设置*/
        DWORD               m_bMsgTransparent:1;/**< 接收消息标志 TRUE-不处理消息 */
        DWORD               m_bFocusable:1;     /**< 窗口可获得焦点标志 */
        DWORD               m_bDrawFocusRect:1; /**< 绘制默认的焦点虚框 */
        DWORD               m_bCacheDraw:1;     /**< 支持窗口内容的Cache标志 */
        DWORD               m_bCacheDirty:1;    /**< 缓存窗口脏标志 */
        DWORD               m_bLayeredWindow:1; /**< 指示是否是一个分层窗口 */

		LayoutDirtyType     m_layoutDirty;      /**< 布局脏标志 参见LayoutDirtyType */
        CAutoRefPtr<IRenderTarget> m_cachedRT;  /**< 缓存窗口绘制的RT */
        CAutoRefPtr<IRenderTarget> m_layeredRT; /**< 分层窗口绘制的RT */
        CAutoRefPtr<IRegion>       m_rgnWnd;    /**< 窗口Region */
        ISkinObj *          m_pBgSkin;          /**< 背景skin */
        ISkinObj *          m_pNcSkin;          /**< 非客户区skin */
        ULONG_PTR           m_uData;            /**< 窗口的数据位,可以通过GetUserData获得 */

        SLayoutSize         m_nMaxWidth;        /**< 自动计算大小时，窗口的最大宽度 */

		COLORREF			m_crColorize;		/**< 调色值 */


        typedef struct GETRTDATA
        {
            CRect rcRT;             /**< GETRT调用的有效范围 */
            DWORD gdcFlags;         /**< GETRT绘制标志位 */
            CAutoRefPtr<IRegion> rgn;/**< 保存一个和rcRT对应的IRegion对象 */
        } * PGETRTDATA;
        
        PGETRTDATA m_pGetRTData;
        
        CAutoRefPtr<IRegion>    m_invalidRegion;/**< 非背景混合窗口的脏区域 */
		CAutoRefPtr<IAttrStorage> m_attrStorage;/**< 属性保存对象 */
		
#ifdef SOUI_ENABLE_ACC
		CAutoRefPtr<IAccessible>  m_pAcc;
		CAutoRefPtr<IAccProxy> m_pAccProxy;
#endif
#ifdef _DEBUG
        DWORD               m_nMainThreadId;    /**< 窗口宿线程ID */
#endif
	};
}//namespace SOUI