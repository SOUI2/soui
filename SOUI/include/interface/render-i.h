#pragma once

#include <unknown/obj-ref-i.h>
#include <sobject/Sobject.hpp>
#include "imgdecoder-i.h"
#include "SPathEffect-i.h"
namespace SOUI
{
	struct IRenderObj;
    struct IBrush;
    struct IPen;
    struct IFont;
    struct IBitmap;
    struct IRegion;
	struct IPath;
	struct IPathMeasure;
    struct IRenderTarget;
    struct IRenderFactory;

	enum EXPEND_MODE
	{
		EM_NULL=0,      /*<不变*/
		EM_STRETCH,     /*<拉伸*/
		EM_TILE,        /*<平铺*/
	};

	enum FilterLevel {
		kNone_FilterLevel = 0,
		kLow_FilterLevel,
		kMedium_FilterLevel,
		kHigh_FilterLevel
	};

	enum GradientType{
		linear = 0,	/*<线性渐变*/
		radial,		/*<辐射渐变*/
		sweep		/*<扫描渐变*/
	};

    /**
    * @struct     IRenderFactory
    * @brief      RenderFactory对象
    * 
    * Describe
    */
    struct IRenderFactory : public IObjRef
    {
        virtual IImgDecoderFactory * GetImgDecoderFactory()=0;
        virtual void SetImgDecoderFactory(IImgDecoderFactory *pImgDecoderFac)=0;
        virtual BOOL CreateRenderTarget(IRenderTarget ** ppRenderTarget,int nWid,int nHei)=0;

        /**
        * CreateFont
        * @brief    创建字体
        * @param [out] IFont ** ppFont -- 字体对象
        * @param [in] const LOGFONT &lf -- 字体属性
        * @param [in] const IPropBag * pPropBag --  字体扩展属性，由渲染引擎解析
        * @return   BOOL -- TRUE:SUCCEED, FASLE:FAILED 
        *
        * Describe  
        */
        virtual BOOL CreateFont(IFont ** ppFont, const LOGFONT &lf)=0;

        virtual BOOL CreateBitmap(IBitmap ** ppBitmap)=0;

        virtual BOOL CreateRegion(IRegion **ppRgn)=0;

		virtual BOOL CreatePath(IPath ** ppPath)=0;

		virtual BOOL CreatePathEffect(REFGUID guidEffect,IPathEffect ** ppPathEffect) = 0;

		virtual BOOL CreatePathMeasure(IPathMeasure ** ppPathMeasure) = 0;
    };

    enum OBJTYPE
    {
        OT_NULL=0,
        OT_PEN,
        OT_BRUSH,
        OT_BITMAP,
        OT_FONT,
        OT_RGN,
		OT_PATH,
    };

    /**
    * @struct     IRenderObj
    * @brief      渲染对象基类
    * 
    * Describe    所有渲染对象全部使用引用计数管理生命周期
    */
    struct IRenderObj : public IObject, public IObjRef 
    {
        /**
         * ObjectType
         * @brief    查询对象类型
         * @return   const UINT 
         * Describe  
         */    
        virtual const OBJTYPE ObjectType() const = 0;

        /**
         * GetRenderFactory
         * @brief    获得创建该渲染对象的类厂
         * @return   IRenderFactory * -- 类厂 
         * Describe  
         */    
        virtual IRenderFactory * GetRenderFactory() const = 0;

    };


    /**
    * @struct     IBrush
    * @brief      画刷对象
    * 
    * Describe    
    */
    struct IBrush : public IRenderObj
    {
        virtual const OBJTYPE ObjectType() const
        {
            return OT_BRUSH;
        }
    };

    /**
    * @struct     IPen
    * @brief      画笔对象
    * 
    * Describe    
    */
    struct  IPen: public IRenderObj
    {
        virtual const OBJTYPE ObjectType() const
        {
            return OT_PEN;
        }
    };

    /**
    * @struct     IBitmap
    * @brief      位图对象
    * 
    * Describe    
    */
    struct  IBitmap: public IRenderObj
    {
        virtual const OBJTYPE ObjectType() const
        {
            return OT_BITMAP;
        }


        /**
         * Init
         * @brief    从32位的位图数据初始化IBitmap
         * @param    int nWid --  图片宽度
         * @param    int nHei --  图片高度
         * @param    const LPVOID pBits --  位图数据,以ARGB形式存储
         * @return   HRESULT -- 成功返回S_OK,失败返回错误代码
         * Describe  
         */    
        virtual HRESULT Init(int nWid,int nHei,const LPVOID pBits=NULL)=0;

        /**
         * Init
         * @brief    从IImgFrame初始化位图
         * @param    IImgFrame * pImgFrame --  IImgFrame指针
         * @return   HRESULT -- 成功返回S_OK,失败返回错误代码
         * Describe  
         */    
        virtual HRESULT Init(IImgFrame *pImgFrame) =0;

        /**
         * LoadFromFile
         * @brief    从文件中加载位图
         * @param    LPCTSTR pszFileName --  文件名
         * @return   HRESULT -- 成功返回S_OK,失败返回错误代码
         * Describe  
         */    
        virtual HRESULT LoadFromFile(LPCTSTR pszFileName)=0;

        /**
         * LoadFromMemory
         * @brief    从内存中加载位图
         * @param    LPBYTE pBuf --  内存地址
         * @param    size_t szLen --  内存大小
         * @return   HRESULT -- 成功返回S_OK,失败返回错误代码
         * Describe  
         */    
        virtual HRESULT LoadFromMemory(LPBYTE pBuf,size_t szLen)=0;

        /**
         * Width
         * @brief    获得图片宽度
         * @return   UINT -- 图片宽度
         * Describe  
         */    
        virtual UINT    Width() const =0;

        /**
         * Height
         * @brief    获得图片高度
         * @return   UINT -- 图片高度
         * Describe  
         */    
        virtual UINT    Height() const =0;

        /**
         * Size
         * @brief    获得图片高度及宽度
         * @return   SIZE -- 图片高度及宽度
         * Describe  
         */    
        virtual SIZE    Size() const =0;

        /**
         * LockPixelBits
         * @brief    锁定位图的数据
         * @return   LPVOID -- 位图数据地址
         * Describe  
         */    
        virtual LPVOID  LockPixelBits() =0;

        /**
         * UnlockPixelBits
         * @brief    解除数据锁定
         * @param    LPVOID --  由LockPixelBits返回的位图数据地址
         * @return   void
         * Describe  与LockPixelBits配对使用
         */    
        virtual void    UnlockPixelBits(LPVOID pBuf) =0;
        
        /**
         * GetPixelBits
         * @brief    以只读方式获取位图数据
         * @return   LPVOID -- 位图数据地址
         * Describe  
         */    
        virtual const LPVOID GetPixelBits() const = 0;
        
        
        /**
         * Clone
         * @brief    克隆一个新位图对象
         * @param    IBitmap **ppClone --  返回的IBitmap* 对象
         * @return   HRESULT
         * Describe  成功后，调用者执行IBitmap::Release
         */    
        inline virtual HRESULT Clone(IBitmap **ppClone) const;
        
		/**
         * Scale
         * @brief    缩放克隆一个新位图对象
         * @param    IBitmap **pOutput --  返回的IBitmap* 对象
		 * @param	 int nScale -- 缩放比例, 100代表不缩放
		 * @param    FilterLevel filterLevel--插值算法,render-skia支持选择算法
         * @return   HRESULT
         * Describe  成功后，调用者执行IBitmap::Release
         */
		inline virtual HRESULT Scale(IBitmap **pOutput,int nScale,FilterLevel filterLevel);
		
		/**
         * Scale
         * @brief    缩放克隆一个新位图对象
         * @param    IBitmap **pOutput --  返回的IBitmap* 对象
		 * @param	 int nWid -- 宽度
		 * @param	 int nHei -- 高度
		 * @param    FilterLevel filterLevel--插值算法,render-skia支持选择算法
         * @return   HRESULT
         * Describe  成功后，调用者执行IBitmap::Release
         */
		inline virtual HRESULT Scale(IBitmap **pOutput,int nWid,int nHei,FilterLevel filterLevel);

        /**
         * 将位图保存到文件
         * @brief    Save
         * @param    LPCWSTR pszFileName --  File name
         * @param    const LPVOID *pFormat --  image format
         * @return   HRESULT -- S_OK: succeed
         * Describe  
         */    
        inline virtual HRESULT Save(LPCWSTR pszFileName,const LPVOID pFormat);
    };

    /**
    * @struct     IFont
    * @brief      字体对象
    * 
    * Describe    
    */
    struct  IFont : public IRenderObj
    {
		virtual const OBJTYPE ObjectType() const
        {
            return OT_FONT;
        }

        /**
         * LogFont
         * @brief    获得字体的LOGFONT
         * @return   const LOGFONT * -- 包含字体信息的LOGFONT*
         * Describe  
         */    
        virtual const LOGFONT * LogFont() const =0;

        /**
         * FamilyName
         * @brief    获取字体名
         * @return   LPCTSTR -- 字体名
         * Describe  
         */    
        virtual LPCTSTR FamilyName()=0;

        /**
         * TextSize
         * @brief    获取字体大小
         * @return   int -- 字体大小
         * Describe  
         */    
        virtual int TextSize()=0;

        /**
         * IsBold
         * @brief    查询是否为粗体
         * @return   BOOL -- true为粗体，false正常
         * Describe  
         */    
        virtual BOOL IsBold()=0;

        /**
         * IsUnderline
         * @brief    查询字体下划线状态
         * @return   BOOL -- true有下划线，false正常
         * Describe  
         */    
        virtual BOOL IsUnderline()=0;

        /**
         * IsItalic
         * @brief    查询字体的斜体状态
         * @return   BOOL -- true为斜体，false正常
         * Describe  
         */    
        virtual BOOL IsItalic()=0;

        /**
         * StrikeOut
         * @brief    查询字体的删除线状态
         * @return   BOOL -- true有删除线
         * Describe  
         */    
        virtual BOOL IsStrikeOut() =0;

		virtual BOOL UpdateFont(const LOGFONT *pLogFont) = 0;
    };

    /**
    * @struct     IRegion
    * @brief      Region对象
    * 
    * Describe    
    */
    struct  IRegion : public IRenderObj
    {
        virtual const OBJTYPE ObjectType() const
        {
            return OT_RGN;
        }

        /**
         * CombineRect
         * @brief    将一个矩形与this组合
         * @param    LPCRECT lprect --  要组合的矩形
         * @param    int nCombineMode --  组合模式
         * @return   void
         * Describe  组合模式同Win32 API CombineRect
         */    
        virtual void CombineRect(LPCRECT lprect,int nCombineMode )=0;

        /**
         * CombineRect
         * @brief    将一个矩形与this组合
         * @param    const IRegion * pRgnSrc --  要组合的Region
         * @param    int nCombineMode --  组合模式
         * @return   void
         * Describe  组合模式同Win32 API CombineRgn
         */    
        virtual void CombineRgn(const IRegion * pRgnSrc,int nCombineMode )=0;

		virtual void CombineRoundRect(LPCRECT lprect, POINT ptConner, int nCombineMode) = 0;

		virtual void CombineEllipse(LPCRECT lprect , int nCombineMode) = 0;

        /**
         * PtInRegion
         * @brief    检测一个点是否在region范围内
         * @param    POINT pt --  被检测的点
         * @return   BOOL -- true在region内
         * Describe  
         */    
        virtual BOOL PtInRegion(POINT pt)=0;

        /**
         * RectInRegion
         * @brief    检测一个矩形是否与this相交
         * @param    LPCRECT lprect --  被检测的的矩形。
         * @return   BOOL -- true在region内
         * Describe  
         */    
        virtual BOOL RectInRegion(LPCRECT lprect)=0;

        /**
         * GetRgnBox
         * @brief    获得this的外包矩形
         * @param [out] LPRECT lprect --  外包矩形
         * @return   void  
         * Describe  
         */    
        virtual void GetRgnBox(LPRECT lprect)=0;

        /**
         * IsEmpty
         * @brief    查询region是否为空
         * @return   BOOL -- true为空
         * Describe  
         */    
        virtual BOOL IsEmpty()=0;

        /**
         * Offset
         * @brief    将this整体平移
         * @param    POINT pt --  平移在x,y方向的的位移
         * @return   void 
         * Describe  
         */    
        virtual void Offset(POINT pt)=0;

        /**
         * Clear
         * @brief    清空region
         * @return   void
         * Describe  
         */    
        virtual void Clear()=0;
        
         /**
         * SetRgn
         * @brief    从HRGN创建Region
         * @param    const HRGN hRgn --  HRGN
         * @return   void
         * Describe  
         */    
        virtual void SetRgn(const HRGN hRgn) = 0;
    };


	struct IPath : IRenderObj
	{

		enum FillType {
			/** Specifies that "inside" is computed by a non-zero sum of signed
			edge crossings
			*/
			kWinding_FillType,
			/** Specifies that "inside" is computed by an odd number of edge
			crossings
			*/
			kEvenOdd_FillType,
			/** Same as Winding, but draws outside of the path, rather than inside
			*/
			kInverseWinding_FillType,
			/** Same as EvenOdd, but draws outside of the path, rather than inside
			*/
			kInverseEvenOdd_FillType
		};

		enum Convexity {
			kUnknown_Convexity,
			kConvex_Convexity,
			kConcave_Convexity
		};

		/** Return the path's fill type. This is used to define how "inside" is
		computed. The default value is kWinding_FillType.

		@return the path's fill type
		*/
		virtual FillType getFillType() const PURE;

		/** Set the path's fill type. This is used to define how "inside" is
		computed. The default value is kWinding_FillType.

		@param ft The new fill type for this path
		*/
		virtual void setFillType(FillType ft) PURE;

		/** Returns true if the filltype is one of the Inverse variants */
		virtual bool isInverseFillType() const PURE;

		/**
		*  Toggle between inverse and normal filltypes. This reverse the return
		*  value of isInverseFillType()
		*/
		virtual void toggleInverseFillType() PURE;

		/**
		*  Return the path's convexity, as stored in the path. If it is currently unknown,
		*  then this function will attempt to compute the convexity (and cache the result).
		*/
		virtual Convexity getConvexity() const PURE;


		/**
		*  Store a convexity setting in the path. There is no automatic check to
		*  see if this value actually agrees with the return value that would be
		*  computed by getConvexity().
		*
		*  Note: even if this is set to a "known" value, if the path is later
		*  changed (e.g. lineTo(), addRect(), etc.) then the cached value will be
		*  reset to kUnknown_Convexity.
		*/
		virtual void setConvexity(Convexity c) PURE;

		/**
		*  Returns true if the path is flagged as being convex. This is not a
		*  confirmed by any analysis, it is just the value set earlier.
		*/
		virtual bool isConvex() const PURE;

		/** Returns true if the path is an oval.
		*
		* @param rect      returns the bounding rect of this oval. It's a circle
		*                  if the height and width are the same.
		*
		* @return true if this path is an oval.
		*              Tracking whether a path is an oval is considered an
		*              optimization for performance and so some paths that are in
		*              fact ovals can report false.
		*/
		virtual bool isOval(RECT* rect) const PURE;

		/** Clear any lines and curves from the path, making it empty. This frees up
		internal storage associated with those segments.
		On Android, does not change fSourcePath.
		*/
		virtual void reset() PURE;

		/** Similar to reset(), in that all lines and curves are removed from the
		path. However, any internal storage for those lines/curves is retained,
		making reuse of the path potentially faster.
		On Android, does not change fSourcePath.
		*/
		virtual void rewind() PURE;

		/** Returns true if the path is empty (contains no lines or curves)

		@return true if the path is empty (contains no lines or curves)
		*/
		virtual bool isEmpty() const PURE;

		/**
		*  Returns true if all of the points in this path are finite, meaning there
		*  are no infinities and no NaNs.
		*/
		virtual bool isFinite() const PURE;

		/**
		*  Returns true if the path specifies a single line (i.e. it contains just
		*  a moveTo and a lineTo). If so, and line[] is not null, it sets the 2
		*  points in line[] to the end-points of the line. If the path is not a
		*  line, returns false and ignores line[].
		*/
		virtual bool isLine(POINT line[2]) const PURE;

		/** Returns true if the path specifies a rectangle. If so, and if rect is
		not null, set rect to the bounds of the path. If the path does not
		specify a rectangle, return false and ignore rect.

		@param rect If not null, returns the bounds of the path if it specifies
		a rectangle
		@return true if the path specifies a rectangle
		*/
		virtual bool isRect(RECT* rect) const PURE;

		/** Return the number of points in the path
		*/
		virtual int countPoints() const PURE;

		/** Return the point at the specified index. If the index is out of range
		(i.e. is not 0 <= index < countPoints()) then the returned coordinates
		will be (0,0)
		*/
		virtual POINT getPoint(int index) const PURE;

		/** Returns the number of points in the path. Up to max points are copied.

		@param points If not null, receives up to max points
		@param max The maximum number of points to copy into points
		@return the actual number of points in the path
		*/
		virtual int getPoints(POINT points[], int max) const PURE;

		/** Return the number of verbs in the path
		*/
		virtual int countVerbs() const PURE;

		/** Returns the number of verbs in the path. Up to max verbs are copied. The
		verbs are copied as one byte per verb.

		@param verbs If not null, receives up to max verbs
		@param max The maximum number of verbs to copy into verbs
		@return the actual number of verbs in the path
		*/
		virtual int getVerbs(BYTE verbs[], int max) const PURE;

		/** Returns the bounds of the path's points. If the path contains 0 or 1
		points, the bounds is set to (0,0,0,0), and isEmpty() will return true.
		Note: this bounds may be larger than the actual shape, since curves
		do not extend as far as their control points.
		*/
		virtual RECT getBounds() const PURE;



		//  Construction methods

		/** Set the beginning of the next contour to the point (x,y).

		@param x    The x-coordinate of the start of a new contour
		@param y    The y-coordinate of the start of a new contour
		*/
		virtual void moveTo(float x, float y) PURE;


		/** Set the beginning of the next contour relative to the last point on the
		previous contour. If there is no previous contour, this is treated the
		same as moveTo().

		@param dx   The amount to add to the x-coordinate of the end of the
		previous contour, to specify the start of a new contour
		@param dy   The amount to add to the y-coordinate of the end of the
		previous contour, to specify the start of a new contour
		*/
		virtual void rMoveTo(float dx, float dy) PURE;

		/** Add a line from the last point to the specified point (x,y). If no
		moveTo() call has been made for this contour, the first point is
		automatically set to (0,0).

		@param x    The x-coordinate of the end of a line
		@param y    The y-coordinate of the end of a line
		*/
		virtual void lineTo(float x, float y) PURE;


		/** Same as lineTo, but the coordinates are considered relative to the last
		point on this contour. If there is no previous point, then a moveTo(0,0)
		is inserted automatically.

		@param dx   The amount to add to the x-coordinate of the previous point
		on this contour, to specify a line
		@param dy   The amount to add to the y-coordinate of the previous point
		on this contour, to specify a line
		*/
		virtual void rLineTo(float dx, float dy) PURE;

		/** Add a quadratic bezier from the last point, approaching control point
		(x1,y1), and ending at (x2,y2). If no moveTo() call has been made for
		this contour, the first point is automatically set to (0,0).

		@param x1   The x-coordinate of the control point on a quadratic curve
		@param y1   The y-coordinate of the control point on a quadratic curve
		@param x2   The x-coordinate of the end point on a quadratic curve
		@param y2   The y-coordinate of the end point on a quadratic curve
		*/
		virtual void quadTo(float x1, float y1, float x2, float y2)PURE;


		/** Same as quadTo, but the coordinates are considered relative to the last
		point on this contour. If there is no previous point, then a moveTo(0,0)
		is inserted automatically.

		@param dx1   The amount to add to the x-coordinate of the last point on
		this contour, to specify the control point of a quadratic curve
		@param dy1   The amount to add to the y-coordinate of the last point on
		this contour, to specify the control point of a quadratic curve
		@param dx2   The amount to add to the x-coordinate of the last point on
		this contour, to specify the end point of a quadratic curve
		@param dy2   The amount to add to the y-coordinate of the last point on
		this contour, to specify the end point of a quadratic curve
		*/
		virtual void rQuadTo(float dx1, float dy1, float dx2, float dy2)PURE;

		virtual void conicTo(float x1, float y1, float x2, float y2,
			float w) PURE;
		virtual void rConicTo(float dx1, float dy1, float dx2, float dy2,
			float w) PURE;

		/** Add a cubic bezier from the last point, approaching control points
		(x1,y1) and (x2,y2), and ending at (x3,y3). If no moveTo() call has been
		made for this contour, the first point is automatically set to (0,0).

		@param x1   The x-coordinate of the 1st control point on a cubic curve
		@param y1   The y-coordinate of the 1st control point on a cubic curve
		@param x2   The x-coordinate of the 2nd control point on a cubic curve
		@param y2   The y-coordinate of the 2nd control point on a cubic curve
		@param x3   The x-coordinate of the end point on a cubic curve
		@param y3   The y-coordinate of the end point on a cubic curve
		*/
		virtual void cubicTo(float x1, float y1, float x2, float y2,
			float x3, float y3)PURE;


		/** Same as cubicTo, but the coordinates are considered relative to the
		current point on this contour. If there is no previous point, then a
		moveTo(0,0) is inserted automatically.

		@param dx1   The amount to add to the x-coordinate of the last point on
		this contour, to specify the 1st control point of a cubic curve
		@param dy1   The amount to add to the y-coordinate of the last point on
		this contour, to specify the 1st control point of a cubic curve
		@param dx2   The amount to add to the x-coordinate of the last point on
		this contour, to specify the 2nd control point of a cubic curve
		@param dy2   The amount to add to the y-coordinate of the last point on
		this contour, to specify the 2nd control point of a cubic curve
		@param dx3   The amount to add to the x-coordinate of the last point on
		this contour, to specify the end point of a cubic curve
		@param dy3   The amount to add to the y-coordinate of the last point on
		this contour, to specify the end point of a cubic curve
		*/
		virtual void rCubicTo(float x1, float y1, float x2, float y2,
			float x3, float y3)PURE;

		/** Append the specified arc to the path as a new contour. If the start of
		the path is different from the path's current last point, then an
		automatic lineTo() is added to connect the current contour to the start
		of the arc. However, if the path is empty, then we call moveTo() with
		the first point of the arc. The sweep angle is treated mod 360.

		@param oval The bounding oval defining the shape and size of the arc
		@param startAngle Starting angle (in degrees) where the arc begins
		@param sweepAngle Sweep angle (in degrees) measured clockwise. This is
		treated mod 360.
		@param forceMoveTo If true, always begin a new contour with the arc
		*/
		virtual void arcTo(const RECT& oval, float startAngle, float sweepAngle,
			bool forceMoveTo)PURE;

		/** Append a line and arc to the current path. This is the same as the
		PostScript call "arct".
		*/
		virtual void arcTo(float x1, float y1, float x2, float y2,
			float radius)PURE;


		/** Close the current contour. If the current point is not equal to the
		first point of the contour, a line segment is automatically added.
		*/
		virtual void close()PURE;

		enum Direction {
			/** Direction either has not been or could not be computed */
			kUnknown_Direction,
			/** clockwise direction for adding closed contours */
			kCW_Direction,
			/** counter-clockwise direction for adding closed contours */
			kCCW_Direction,
		};


		/** Returns true if the path specifies a rectangle. If so, and if isClosed is
		not null, set isClosed to true if the path is closed. Also, if returning true
		and direction is not null, return the rect direction. If the path does not
		specify a rectangle, return false and ignore isClosed and direction.

		@param isClosed If not null, set to true if the path is closed
		@param direction If not null, set to the rectangle's direction
		@return true if the path specifies a rectangle
		*/
		virtual bool isRect(bool* isClosed, Direction* direction) const PURE;

		/**
		*  Add a closed rectangle contour to the path
		*  @param rect The rectangle to add as a closed contour to the path
		*  @param dir  The direction to wind the rectangle's contour. Cannot be
		*              kUnknown_Direction.
		*/
		virtual void addRect(const RECT& rect, Direction dir = kCW_Direction)PURE;

		/**
		*  Add a closed rectangle contour to the path
		*
		*  @param left     The left side of a rectangle to add as a closed contour
		*                  to the path
		*  @param top      The top of a rectangle to add as a closed contour to the
		*                  path
		*  @param right    The right side of a rectangle to add as a closed contour
		*                  to the path
		*  @param bottom   The bottom of a rectangle to add as a closed contour to
		*                  the path
		*  @param dir  The direction to wind the rectangle's contour. Cannot be
		*              kUnknown_Direction.
		*/
		virtual void addRect(float left, float top, float right, float bottom,
			Direction dir = kCW_Direction)PURE;

		/**
		*  Add a closed oval contour to the path
		*
		*  @param oval The bounding oval to add as a closed contour to the path
		*  @param dir  The direction to wind the oval's contour. Cannot be
		*              kUnknown_Direction.
		*/
		virtual void addOval(const RECT& oval, Direction dir = kCW_Direction)PURE;

		/**
		*  Add a closed circle contour to the path
		*
		*  @param x        The x-coordinate of the center of a circle to add as a
		*                  closed contour to the path
		*  @param y        The y-coordinate of the center of a circle to add as a
		*                  closed contour to the path
		*  @param radius   The radius of a circle to add as a closed contour to the
		*                  path
		*  @param dir  The direction to wind the circle's contour. Cannot be
		*              kUnknown_Direction.
		*/
		virtual void addCircle(float x, float y, float radius,
			Direction dir = kCW_Direction)PURE;

		/** Add the specified arc to the path as a new contour.

		@param oval The bounds of oval used to define the size of the arc
		@param startAngle Starting angle (in degrees) where the arc begins
		@param sweepAngle Sweep angle (in degrees) measured clockwise
		*/
		virtual void addArc(const RECT& oval, float startAngle, float sweepAngle)PURE;

		/**
		*  Add a closed round-rectangle contour to the path
		*  @param rect The bounds of a round-rectangle to add as a closed contour
		*  @param rx   The x-radius of the rounded corners on the round-rectangle
		*  @param ry   The y-radius of the rounded corners on the round-rectangle
		*  @param dir  The direction to wind the rectangle's contour. Cannot be
		*              kUnknown_Direction.
		*/
		virtual void addRoundRect(const RECT& rect, float rx, float ry,
			Direction dir = kCW_Direction)PURE;

		/**
		*  Add a closed round-rectangle contour to the path. Each corner receives
		*  two radius values [X, Y]. The corners are ordered top-left, top-right,
		*  bottom-right, bottom-left.
		*  @param rect The bounds of a round-rectangle to add as a closed contour
		*  @param radii Array of 8 scalars, 4 [X,Y] pairs for each corner
		*  @param dir  The direction to wind the rectangle's contour. Cannot be
		*              kUnknown_Direction.
		* Note: The radii here now go through the same constraint handling as the
		*       SkRRect radii (i.e., either radii at a corner being 0 implies a
		*       sqaure corner and oversized radii are proportionally scaled down).
		*/
		virtual void addRoundRect(const RECT& rect, const float radii[],
			Direction dir = kCW_Direction)PURE;


		/**
		*  Add a new contour made of just lines. This is just a fast version of
		*  the following:
		*      this->moveTo(pts[0]);
		*      for (int i = 1; i < count; ++i) {
		*          this->lineTo(pts[i]);
		*      }
		*      if (close) {
		*          this->close();
		*      }
		*/
		virtual void addPoly(const POINT pts[], int count, bool close) PURE;

		enum AddPathMode {
			/** Source path contours are added as new contours.
			*/
			kAppend_AddPathMode,
			/** Path is added by extending the last contour of the destination path
			with the first contour of the source path. If the last contour of
			the destination path is closed, then it will not be extended.
			Instead, the start of source path will be extended by a straight
			line to the end point of the destination path.
			*/
			kExtend_AddPathMode
		};

		/** Add a copy of src to the path, offset by (dx,dy)
		@param src  The path to add as a new contour
		@param dx   The amount to translate the path in X as it is added
		@param dx   The amount to translate the path in Y as it is added
		*/
		virtual void addPath(const IPath * src, float dx, float dy,
			AddPathMode mode = kAppend_AddPathMode) PURE;


		/**
		*  Same as addPath(), but reverses the src input
		*/
		virtual void reverseAddPath(const IPath* src) PURE;


		/** Offset the path by (dx,dy), returning true on success

		@param dx   The amount in the X direction to offset the entire path
		@param dy   The amount in the Y direction to offset the entire path
		*/
		virtual void offset(float dx, float dy) PURE;


		/** Return the last point on the path. If no points have been added, (0,0)
		is returned. If there are no points, this returns false, otherwise it
		returns true.

		@param lastPt   The last point on the path is returned here
		*/
		virtual bool getLastPt(POINT* lastPt) const PURE;

		/** Set the last point on the path. If no points have been added,
		moveTo(x,y) is automatically called.

		@param x    The new x-coordinate for the last point
		@param y    The new y-coordinate for the last point
		*/
		virtual void setLastPt(float x, float y) PURE;

	};
    
	struct IPathMeasure : IObjRef
	{
		/**
		* Assign a new path, or null to have none.
		*/
		virtual void setPath(IPath * path, bool forceClosed) =0;

		/**
		* Return the total length of the current contour, or 0 if no path is
		* associated with this measure object.
		*/
		virtual float getLength()  = 0;

		/**
		* Pins distance to 0 <= distance <= getLength(), and then computes the
		* corresponding position and tangent. Returns false if there is no path,
		* or a zero-length path was specified, in which case position and tangent
		* are unchanged.
		*
		* @param distance The distance along the current contour to sample
		* @param pos If not null, eturns the sampled position (x==[0], y==[1])
		* @param tan If not null, returns the sampled tangent (x==[0], y==[1])
		* @return false if there was no path associated with this measure object
		*/
		virtual bool getPosTan(float distance, float pos[], float tan[])  = 0;

		/**
		* Given a start and stop distance, return in dst the intervening
		* segment(s). If the segment is zero-length, return false, else return
		* true. startD and stopD are pinned to legal values (0..getLength()).
		* If startD >= stopD then return false (and leave dst untouched).
		* Begin the segment with a moveTo if startWithMoveTo is true.
		*
		* <p>On {@link android.os.Build.VERSION_CODES#KITKAT} and earlier
		* releases, the resulting path may not display on a hardware-accelerated
		* Canvas. A simple workaround is to add a single operation to this path,
		* such as <code>dst.rLineTo(0, 0)</code>.</p>
		*/
		virtual bool getSegment(float startD, float stopD, IPath * dst, bool startWithMoveTo)  = 0;
	};

    struct IxForm
    {
        FLOAT eM11; 
        FLOAT eM12; 
        FLOAT eM21; 
        FLOAT eM22; 
        FLOAT eDx; 
        FLOAT eDy; 
    };


    /**
    * @struct     IRenderTarget
    * @brief      RenderTarget对象
    * 
    * Describe    实现各位渲染接口并创建设备相关资源
    */
    struct IRenderTarget: public IObjRef, public SObject
    {
        virtual HRESULT CreateCompatibleRenderTarget(SIZE szTarget,IRenderTarget **ppRenderTarget)=0;
        virtual HRESULT CreatePen(int iStyle,COLORREF cr,int cWidth,IPen ** ppPen)=0;
        virtual HRESULT CreateSolidColorBrush(COLORREF cr,IBrush ** ppBrush)=0;
        virtual HRESULT CreateBitmapBrush( IBitmap *pBmp,IBrush ** ppBrush )=0;
		virtual HRESULT CreateRegion( IRegion ** ppRegion )=0;

        virtual HRESULT Resize(SIZE sz)=0;

        virtual HRESULT OffsetViewportOrg(int xOff, int yOff, LPPOINT lpPoint=NULL)=0;
        virtual HRESULT GetViewportOrg(LPPOINT lpPoint) =0;
        virtual HRESULT SetViewportOrg(POINT pt) =0;

        virtual HRESULT PushClipRect(LPCRECT pRect,UINT mode=RGN_AND)=0;
        virtual HRESULT PushClipRegion(IRegion *pRegion,UINT mode=RGN_AND)=0;
        virtual HRESULT PopClip()=0;

        virtual HRESULT ExcludeClipRect(LPCRECT pRc)=0;
        virtual HRESULT IntersectClipRect(LPCRECT pRc)=0;

        virtual HRESULT SaveClip(int *pnState)=0;
        virtual HRESULT RestoreClip(int nState=-1)=0;

        virtual HRESULT GetClipRegion(IRegion **ppRegion)=0;
        virtual HRESULT GetClipBox(LPRECT prc)=0;

        virtual HRESULT DrawText(LPCTSTR pszText,int cchLen,LPRECT pRc,UINT uFormat)=0;
        virtual HRESULT MeasureText(LPCTSTR pszText,int cchLen, SIZE *psz) =0;
        virtual HRESULT TextOut(int x,int y, LPCTSTR lpszString,int nCount) =0;

        virtual HRESULT DrawRectangle(LPCRECT pRect)=0;
        virtual HRESULT FillRectangle(LPCRECT pRect)=0;
        virtual HRESULT FillSolidRect(LPCRECT pRect,COLORREF cr)=0;
        virtual HRESULT DrawRoundRect(LPCRECT pRect,POINT pt)=0;
        virtual HRESULT FillRoundRect(LPCRECT pRect,POINT pt)=0;
        virtual HRESULT FillSolidRoundRect(LPCRECT pRect,POINT pt,COLORREF cr)=0;
        virtual HRESULT ClearRect(LPCRECT pRect,COLORREF cr)=0;
        virtual HRESULT InvertRect(LPCRECT pRect) = 0;
        virtual HRESULT DrawEllipse(LPCRECT pRect)=0;
        virtual HRESULT FillEllipse(LPCRECT pRect)=0;
        virtual HRESULT FillSolidEllipse(LPCRECT pRect,COLORREF cr)=0;

        virtual HRESULT DrawArc(LPCRECT pRect,float startAngle,float sweepAngle,bool useCenter) =0;
        virtual HRESULT FillArc(LPCRECT pRect,float startAngle,float sweepAngle) =0;

        virtual HRESULT DrawLines(LPPOINT pPt,size_t nCount) =0;
        virtual HRESULT GradientFill(LPCRECT pRect,BOOL bVert,COLORREF crBegin,COLORREF crEnd,BYTE byAlpha=0xFF)=0;
        virtual HRESULT GradientFillEx( LPCRECT pRect,const POINT* pts,COLORREF *colors,float *pos,int nCount,BYTE byAlpha=0xFF )=0;
		virtual HRESULT GradientFill2(LPCRECT pRect,GradientType type,COLORREF crStart,COLORREF crCenter,COLORREF crEnd,float fLinearAngle,float fCenterX,float fCenterY,int nRadius,BYTE byAlpha=0xff) = 0;
        virtual HRESULT DrawIconEx(int xLeft, int yTop, HICON hIcon, int cxWidth,int cyWidth,UINT diFlags)=0;
        virtual HRESULT DrawBitmap(LPCRECT pRcDest,IBitmap *pBitmap,int xSrc,int ySrc,BYTE byAlpha=0xFF)=0;
        virtual HRESULT DrawBitmapEx(LPCRECT pRcDest,IBitmap *pBitmap,LPCRECT pRcSrc,UINT expendMode, BYTE byAlpha=0xFF)=0;
        virtual HRESULT DrawBitmap9Patch(LPCRECT pRcDest,IBitmap *pBitmap,LPCRECT pRcSrc,LPCRECT pRcSourMargin,UINT expendMode,BYTE byAlpha=0xFF) =0;
        virtual HRESULT BitBlt(LPCRECT pRcDest,IRenderTarget *pRTSour,int xSrc,int ySrc,DWORD dwRop=SRCCOPY)=0;
        virtual HRESULT AlphaBlend(LPCRECT pRcDest,IRenderTarget *pRTSrc,LPCRECT pRcSrc,BYTE byAlpha) =0;
        virtual IRenderObj * GetCurrentObject(OBJTYPE uType) =0;
        //将指定的RenderObj恢复为默认状态
        virtual HRESULT SelectDefaultObject(OBJTYPE objType, IRenderObj ** pOldObj = NULL) =0;
        virtual HRESULT SelectObject(IRenderObj *pObj,IRenderObj ** pOldObj = NULL) =0;
        virtual COLORREF GetTextColor() =0;
        virtual COLORREF SetTextColor(COLORREF color)=0;

        //两个兼容GDI操作的接口
        virtual HDC GetDC(UINT uFlag=0)=0;
        virtual void ReleaseDC(HDC hdc) =0;
        
        /**
         * SetTransform
         * @brief    设置坐标变换矩阵
         * @param    const IxForm * pXForm --  2*3变换矩阵
         * @param    IxForm * pOldXFrom --  原变换矩阵
         * @return   HRESULT -- 成功返回S_OK
         *
         * Describe  
         */
        virtual HRESULT SetTransform(const IxForm * pXForm,IxForm *pOldXFrom=NULL) = 0;

        /**
         * SetTransform
         * @brief    获取当前坐标变换矩阵
         * @param    IxForm * pXForm --  2*3变换矩阵
         * @return   HRESULT -- 成功返回S_OK
         *
         * Describe  
         */        
        virtual HRESULT GetTransform(IxForm * pXForm) const = 0;
        
        /**
         * QueryInterface
         * @brief    提供接口扩展的用的方法
         * @param    REFGUID iid --  待扩展接口ID
         * @param    IObjRef * * ppObj --  接口输出
         * @return   HRESULT -- 成功返回S_OK
         *
         * Describe  具体能获取什么接口依赖于不同的渲染引擎
         */
        virtual HRESULT QueryInterface(REFGUID iid,IObjRef ** ppObj) =0;

        /**
         * GetPixel
         * @brief    获取指定像素颜色值
         * @param    int x -- x
         * @param    int y -- y
         * @return   COLORREF -- 像素颜色值
         *
         * Describe  和::GetPixel一致
         */
		virtual COLORREF GetPixel(int x, int y) = 0;

        /**
         * SetPixel
         * @brief    设置指定像素颜色值
         * @param    int x -- x
         * @param    int y -- y
		 * @param    COLORREF cr -- 设置的颜色值
         * @return   COLORREF -- 原像素颜色值
         *
         * Describe  和::SetPixel一致
         */
		virtual COLORREF SetPixel(int x, int y, COLORREF cr) = 0;

		/**
		 *  Modify the current clip with the specified path.
		 *  @param path The path to combine with the current clip
		 *  @param mode The region op to apply to the current clip
		 *  @param doAntiAlias true if the clip should be antialiased
		 */
		virtual HRESULT ClipPath(const IPath * path, UINT mode, bool doAntiAlias = false) = 0;

		/** Draw the specified path using the specified paint. The path will be
		filled or framed based on the Style in the paint.
		@param path     The path to be drawn
		*/
		virtual HRESULT DrawPath(const IPath * path,IPathEffect * pathEffect=NULL) = 0;

	};


	inline HRESULT IBitmap::Clone(IBitmap **ppClone) const 
	{
		HRESULT hr = E_UNEXPECTED;
		BOOL bOK = GetRenderFactory()->CreateBitmap(ppClone);
		if(bOK)
		{
			hr=(*ppClone)->Init(Width(),Height(),GetPixelBits());
			if(S_OK != hr)
			{
				(*ppClone)->Release();
				(*ppClone) = NULL;
			}
		}
		return hr;
	}

	inline HRESULT IBitmap::Scale(IBitmap **ppOutput,int nScale,FilterLevel filterLevel)
	{
		int wid = MulDiv(Width(),nScale,100);
		int hei = MulDiv(Height(),nScale,100);
		return Scale(ppOutput,wid,hei,filterLevel);
	}

	inline HRESULT IBitmap::Scale(IBitmap **pOutput,int nWid,int nHei,FilterLevel filterLevel)
	{
		if(nWid == Width() && nHei == Height())
		{
			return Clone(pOutput);
		}
		HRESULT hr = E_UNEXPECTED;
		BOOL bOK = GetRenderFactory()->CreateBitmap(pOutput);
		if(bOK)
		{
			IRenderTarget *pRT=NULL;
			if(GetRenderFactory()->CreateRenderTarget(&pRT,nWid,nHei))
			{
				RECT rcSrc = {0,0,(long)Width(),(long)Height()};
				RECT rcDst ={0,0,nWid,nHei};
				hr = pRT->DrawBitmapEx(&rcDst,this,&rcSrc,MAKELONG(EM_STRETCH,filterLevel));
				if(hr == S_OK)
				{
					*pOutput = (IBitmap*)pRT->GetCurrentObject(OT_BITMAP);
					(*pOutput)->AddRef();
				}
				pRT->Release();
			}else
			{
				hr = E_OUTOFMEMORY;
			}
		}
		return hr;
	}

	inline HRESULT IBitmap::Save(LPCWSTR pszFileName,const LPVOID pFormat)
	{
		return GetRenderFactory()->GetImgDecoderFactory()->SaveImage(this,pszFileName,pFormat);
	}
}//end of namespace SOUI
