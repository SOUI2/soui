#pragma once
//磁力窗口管理模块

class CMagnetFrame
{
public:
    enum ATTACHMODE{
        AM_NULL = 0,
        AM_LEFT ,
        AM_TOP,
        AM_RIGHT,
        AM_BOTTOM,
    };
    
    enum ATTACHALIGN
    {
        AA_NULL = 0,
        AA_TOP =1,
        AA_LEFT = 1,
        AA_BOTTOM =2,
        AA_RIGHT =2,
    };
    
public:
    CMagnetFrame(void);
    ~CMagnetFrame(void);
    
    //设置主窗口
    void SetMainWnd(HWND hWnd);
    
    //增加附属窗口
    BOOL AddSubWnd(HWND hWnd,ATTACHMODE am,ATTACHALIGN aa,int nDist = 0);
    
    //移除附属窗口
    BOOL RemoveSubWnd(HWND hWnd);
   
protected:
    BOOL SubclassWnd(HWND hWnd);
    BOOL UnsubclassWnd(HWND hWnd);
    
    struct AtatchInfo
    {
        ATTACHMODE am;
        ATTACHALIGN aa;
        int        nDist;
    };
    
    AtatchInfo CalcAttachMode(HWND hSubWnd,CRect rcSub=CRect());
    void UpdateSubWndPos(HWND hSubWnd,AtatchInfo ai);
protected:
    static LRESULT CALLBACK MagnetWndProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);

protected:
    HWND m_hMainWnd;    //吸附主窗口
    
    
    struct WndData
    {
        WNDPROC pProc;
        AtatchInfo ai;
        BOOL    bFromThis;
    };
    
    typedef SOUI::SMap<HWND,WndData> WNDDATAMAP;
    WNDDATAMAP m_mapOldProc;//吸附窗口列表
};

