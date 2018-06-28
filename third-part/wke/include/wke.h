/*
 * 
 * BlzFans@hotmail.com
 * http://wke.sf.net
 * http://www.github.com/BlzFans/wke
 * licence LGPL
 *
 */

#ifndef WKE_H
#define WKE_H


#ifdef BUILDING_wke
#   define WKE_API __declspec(dllexport)
#else
#   define WKE_API __declspec(dllimport)
#endif

typedef char utf8;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} wkeRect;

enum wkeMouseFlags
{
    WKE_LBUTTON = 0x01,
    WKE_RBUTTON = 0x02,
    WKE_SHIFT   = 0x04,
    WKE_CONTROL = 0x08,
    WKE_MBUTTON = 0x10,
};

enum wkeKeyFlags
{
    WKE_EXTENDED = 0x0100,
    WKE_REPEAT = 0x4000,
};

enum wkeMouseMsg
{
    WKE_MSG_MOUSEMOVE       =  0x0200,
    WKE_MSG_LBUTTONDOWN     =  0x0201,
    WKE_MSG_LBUTTONUP       =  0x0202,
    WKE_MSG_LBUTTONDBLCLK   =  0x0203,
    WKE_MSG_RBUTTONDOWN     =  0x0204,
    WKE_MSG_RBUTTONUP       =  0x0205,
    WKE_MSG_RBUTTONDBLCLK   =  0x0206,
    WKE_MSG_MBUTTONDOWN     =  0x0207,
    WKE_MSG_MBUTTONUP       =  0x0208,
    WKE_MSG_MBUTTONDBLCLK   =  0x0209,
    WKE_MSG_MOUSEWHEEL      =  0x020A,
};

typedef void* jsExecState;
typedef __int64 jsValue;

typedef void* wkeString;
typedef void (*ON_TITLE_CHANGED) (const struct _wkeClientHandler* clientHandler, const wkeString title);
typedef void (*ON_URL_CHANGED) (const struct _wkeClientHandler* clientHandler, const wkeString url);

typedef struct _wkeClientHandler {
    ON_TITLE_CHANGED onTitleChanged;
    ON_URL_CHANGED onURLChanged;
} wkeClientHandler;

typedef struct _wkeBufHandler
{
	virtual void onBufUpdated (const HDC hdc,int x, int y, int cx, int cy) = 0;
}wkeBufHandler;

/*
 *c++ interface
 *-----------------------------------------------------------------------------------------------------------
 *
 */

#ifdef __cplusplus

namespace wke
{
    class IWebView
    {
    public:
        virtual void destroy() = 0;

        virtual const char* name() const = 0;
        virtual void setName(const char* name) = 0;

        virtual bool transparent() const = 0;
        virtual void setTransparent(bool transparent) = 0;

        virtual void loadURL(const utf8* url) = 0;
        virtual void loadURL(const wchar_t* url) = 0;

        virtual void loadHTML(const utf8* html) = 0;
        virtual void loadHTML(const wchar_t* html) = 0;

        virtual void loadFile(const utf8* filename) = 0;
        virtual void loadFile(const wchar_t* filename) = 0;

        virtual bool isLoaded() const = 0;        /*document load sucessed*/
        virtual bool isLoadFailed() const = 0;    /*document load failed*/
        virtual bool isLoadComplete() const = 0;  /*document load complete*/
        virtual bool isDocumentReady() const = 0; /*document ready*/
        virtual void stopLoading() = 0;
        virtual void reload() = 0;

        virtual const utf8* title() = 0;
        virtual const wchar_t* titleW() = 0;

        virtual void resize(int w, int h) = 0;
        virtual int width() const = 0;   /*viewport width*/
        virtual int height() const = 0;  /*viewport height*/

        virtual int contentsWidth() const = 0;  /*contents width*/
        virtual int contentsHeight() const = 0; /*contents height*/

        virtual void setDirty(bool dirty) = 0;
        virtual bool isDirty() const = 0;
        virtual void addDirtyArea(int x, int y, int w, int h) = 0;

        virtual void layoutIfNeeded() = 0;
		virtual void tick() = 0;
        virtual void paint(void* bits, int pitch)=0;
        virtual void paint(void* bits, int bufWid, int bufHei, int xDst, int yDst, int w, int h, int xSrc, int ySrc, bool bCopyAlpha)=0;
        virtual HDC getViewDC() =0;
        virtual bool canGoBack() const = 0;
        virtual bool goBack() = 0;
        virtual bool canGoForward() const = 0;
        virtual bool goForward() = 0;

        virtual void selectAll() = 0;
        virtual void copy() = 0;
        virtual void cut() = 0;
        virtual void paste() = 0;
        virtual void delete_() = 0;

        virtual void setCookieEnabled(bool enable) = 0;
        virtual bool cookieEnabled() const = 0;

        virtual void setMediaVolume(float volume) = 0;
        virtual float mediaVolume() const = 0;

        virtual bool mouseEvent(unsigned int message, int x, int y, unsigned int flags) = 0;
        virtual bool contextMenuEvent(int x, int y, unsigned int flags) = 0;
        virtual bool mouseWheel(int x, int y, int delta, unsigned int flags) = 0;
        virtual bool keyUp(unsigned int virtualKeyCode, unsigned int flags, bool systemKey) = 0;
        virtual bool keyDown(unsigned int virtualKeyCode, unsigned int flags, bool systemKey) = 0;
        virtual bool keyPress(unsigned int virtualKeyCode, unsigned int flags, bool systemKey) = 0;

        virtual void focus() = 0;
        virtual void unfocus() = 0;

        virtual wkeRect getCaret() = 0;

        virtual jsValue runJS(const utf8* script) = 0;
        virtual jsValue runJS(const wchar_t* script) = 0;
        virtual jsExecState globalExec() = 0;

        virtual void sleep() = 0; //moveOffscreen
        virtual void awaken() = 0; //moveOnscreen
        virtual bool isAwake() const = 0;

        virtual void setZoomFactor(float factor) = 0;
        virtual float zoomFactor() const = 0;

        virtual void setEditable(bool editable) = 0;

        virtual void setClientHandler(const wkeClientHandler* handler) = 0;
        virtual const wkeClientHandler* getClientHandler() const = 0;

		virtual void setBufHandler(wkeBufHandler *handler) = 0;
		virtual const wkeBufHandler * getBufHandler() const  = 0;
    };
}

typedef wke::IWebView* wkeWebView;

#else

typedef void* wkeWebView;

#ifndef HAVE_WCHAR_T
typedef unsigned short wchar_t;
#endif

#ifndef HAVE_BOOL
typedef unsigned char bool;
#define true 1
#define false 0
#endif

#endif /*__cplusplus*/

/*
 *c interface
 *----------------------------------------------------------------------------------------------------------
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif

WKE_API void wkeInit();
WKE_API void wkeShutdown();
WKE_API void wkeUpdate();
WKE_API unsigned int wkeVersion();
WKE_API const utf8* wkeVersionString();

typedef void* (*FILE_OPEN) (const char* path);
typedef void (*FILE_CLOSE) (void* handle);
typedef size_t (*FILE_SIZE) (void* handle);
typedef int (*FILE_READ) (void* handle, void* buffer, size_t size);
typedef int (*FILE_SEEK) (void* handle, int offset, int origin);

WKE_API void wkeSetFileSystem(FILE_OPEN pfn_open, FILE_CLOSE pfn_close, FILE_SIZE pfn_size, FILE_READ pfn_read, FILE_SEEK pfn_seek);


WKE_API wkeWebView wkeCreateWebView();
WKE_API wkeWebView wkeGetWebView(const char* name);
WKE_API void wkeDestroyWebView(wkeWebView webView);

WKE_API const char* wkeWebViewName(wkeWebView webView);
WKE_API void wkeSetWebViewName(wkeWebView webView, const char* name);

WKE_API bool wkeIsTransparent(wkeWebView webView);
WKE_API void wkeSetTransparent(wkeWebView webView, bool transparent);

WKE_API void wkeLoadURL(wkeWebView webView, const utf8* url);
WKE_API void wkeLoadURLW(wkeWebView webView, const wchar_t* url);

WKE_API void wkeLoadHTML(wkeWebView webView, const utf8* html);
WKE_API void wkeLoadHTMLW(wkeWebView webView, const wchar_t* html);

WKE_API void wkeLoadFile(wkeWebView webView, const utf8* filename);
WKE_API void wkeLoadFileW(wkeWebView webView, const wchar_t* filename);

WKE_API bool wkeIsLoaded(wkeWebView webView);
WKE_API bool wkeIsLoadFailed(wkeWebView webView);
WKE_API bool wkeIsLoadComplete(wkeWebView webView);
WKE_API bool wkeIsDocumentReady(wkeWebView webView);
WKE_API bool wkeIsLoading(wkeWebView webView);
WKE_API void wkeStopLoading(wkeWebView webView);
WKE_API void wkeReload(wkeWebView webView);

WKE_API const utf8* wkeTitle(wkeWebView webView);
WKE_API const wchar_t* wkeTitleW(wkeWebView webView);

WKE_API void wkeResize(wkeWebView webView, int w, int h);
WKE_API int wkeWidth(wkeWebView webView);
WKE_API int wkeHeight(wkeWebView webView);

WKE_API int wkeContentsWidth(wkeWebView webView);
WKE_API int wkeContentsHeight(wkeWebView webView);

WKE_API void wkeSetDirty(wkeWebView webView, bool dirty);
WKE_API bool wkeIsDirty(wkeWebView webView);
WKE_API void wkeAddDirtyArea(wkeWebView webView, int x, int y, int w, int h);
WKE_API void wkeLayoutIfNeeded(wkeWebView webView);
WKE_API void wkePaint(wkeWebView webView, void* bits,int bufWid, int bufHei, int xDst, int yDst, int w, int h, int xSrc, int ySrc, bool bCopyAlpha);
WKE_API void wkePaint2(wkeWebView webView, void* bits,int pitch);

WKE_API bool wkeCanGoBack(wkeWebView webView);
WKE_API bool wkeGoBack(wkeWebView webView);
WKE_API bool wkeCanGoForward(wkeWebView webView);
WKE_API bool wkeGoForward(wkeWebView webView);

WKE_API void wkeSelectAll(wkeWebView webView);
WKE_API void wkeCopy(wkeWebView webView);
WKE_API void wkeCut(wkeWebView webView);
WKE_API void wkePaste(wkeWebView webView);
WKE_API void wkeDelete(wkeWebView webView);

WKE_API void wkeSetCookieEnabled(wkeWebView webView, bool enable);
WKE_API bool wkeCookieEnabled(wkeWebView webView);

WKE_API void wkeSetMediaVolume(wkeWebView webView, float volume);
WKE_API float wkeMediaVolume(wkeWebView webView);

WKE_API bool wkeMouseEvent(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags);
WKE_API bool wkeContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags);
WKE_API bool wkeMouseWheel(wkeWebView webView, int x, int y, int delta, unsigned int flags);
WKE_API bool wkeKeyUp(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey);
WKE_API bool wkeKeyDown(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey);
WKE_API bool wkeKeyPress(wkeWebView webView, unsigned int charCode, unsigned int flags, bool systemKey);

WKE_API void wkeFocus(wkeWebView webView);
WKE_API void wkeUnfocus(wkeWebView webView);

WKE_API wkeRect wkeGetCaret(wkeWebView webView);

WKE_API jsValue wkeRunJS(wkeWebView webView, const utf8* script);
WKE_API jsValue wkeRunJSW(wkeWebView webView, const wchar_t* script);

WKE_API jsExecState wkeGlobalExec(wkeWebView webView);

WKE_API void wkeSleep(wkeWebView webView);
WKE_API void wkeAwaken(wkeWebView webView);
WKE_API bool wkeIsAwake(wkeWebView webView);

WKE_API void wkeSetZoomFactor(wkeWebView webView, float factor);
WKE_API float wkeZoomFactor(wkeWebView webView);

WKE_API void wkeSetEditable(wkeWebView webView, bool editable);

WKE_API void wkeSetClientHandler(wkeWebView webView, const wkeClientHandler* handler);
WKE_API const wkeClientHandler* wkeGetClientHandler(wkeWebView webView);

WKE_API const utf8* wkeToString(const wkeString string);
WKE_API const wchar_t* wkeToStringW(const wkeString string);

/***JavaScript Bind***/
#define JS_CALL __fastcall
typedef jsValue (JS_CALL *jsNativeFunction) (jsExecState es);

typedef enum
{
	JSTYPE_NUMBER,
	JSTYPE_STRING,
	JSTYPE_BOOLEAN,
	JSTYPE_OBJECT,
	JSTYPE_FUNCTION,
	JSTYPE_UNDEFINED,
} jsType;

WKE_API void jsBindFunction(const char* name, jsNativeFunction fn, unsigned int argCount);
WKE_API void jsBindGetter(const char* name, jsNativeFunction fn); /*get property*/
WKE_API void jsBindSetter(const char* name, jsNativeFunction fn); /*set property*/

WKE_API int jsArgCount(jsExecState es);
WKE_API jsType jsArgType(jsExecState es, int argIdx);
WKE_API jsValue jsArg(jsExecState es, int argIdx);

WKE_API jsType jsTypeOf(jsValue v);
WKE_API bool jsIsNumber(jsValue v);
WKE_API bool jsIsString(jsValue v);
WKE_API bool jsIsBoolean(jsValue v);
WKE_API bool jsIsObject(jsValue v);
WKE_API bool jsIsFunction(jsValue v);
WKE_API bool jsIsUndefined(jsValue v);
WKE_API bool jsIsNull(jsValue v);
WKE_API bool jsIsArray(jsValue v);
WKE_API bool jsIsTrue(jsValue v);
WKE_API bool jsIsFalse(jsValue v);

WKE_API int jsToInt(jsExecState es, jsValue v);
WKE_API float jsToFloat(jsExecState es, jsValue v);
WKE_API double jsToDouble(jsExecState es, jsValue v);
WKE_API bool jsToBoolean(jsExecState es, jsValue v);
WKE_API const utf8* jsToString(jsExecState es, jsValue v);
WKE_API const wchar_t* jsToStringW(jsExecState es, jsValue v);

WKE_API jsValue jsInt(int n);
WKE_API jsValue jsFloat(float f);
WKE_API jsValue jsDouble(double d);
WKE_API jsValue jsBoolean(bool b);

WKE_API jsValue jsUndefined();
WKE_API jsValue jsNull();
WKE_API jsValue jsTrue();
WKE_API jsValue jsFalse();

WKE_API jsValue jsString(jsExecState es, const utf8* str);
WKE_API jsValue jsStringW(jsExecState es, const wchar_t* str);
WKE_API jsValue jsObject(jsExecState es);
WKE_API jsValue jsArray(jsExecState es);

WKE_API jsValue jsFunction(jsExecState es, jsNativeFunction fn, unsigned int argCount);

//return the window object
WKE_API jsValue jsGlobalObject(jsExecState es);

WKE_API jsValue jsEval(jsExecState es, const utf8* str);
WKE_API jsValue jsEvalW(jsExecState es, const wchar_t* str);

WKE_API jsValue jsCall(jsExecState es, jsValue func, jsValue thisObject, jsValue* args, int argCount);
WKE_API jsValue jsCallGlobal(jsExecState es, jsValue func, jsValue* args, int argCount);

WKE_API jsValue jsGet(jsExecState es, jsValue object, const char* prop);
WKE_API void jsSet(jsExecState es, jsValue object, const char* prop, jsValue v);

WKE_API jsValue jsGetGlobal(jsExecState es, const char* prop);
WKE_API void jsSetGlobal(jsExecState es, const char* prop, jsValue v);

WKE_API jsValue jsGetAt(jsExecState es, jsValue object, int index);
WKE_API void jsSetAt(jsExecState es, jsValue object, int index, jsValue v);

WKE_API int jsGetLength(jsExecState es, jsValue object);
WKE_API void jsSetLength(jsExecState es, jsValue object, int length);

WKE_API wkeWebView jsGetWebView(jsExecState es);

WKE_API void jsGC(); //garbage collect

#ifdef __cplusplus
}
#endif


#endif