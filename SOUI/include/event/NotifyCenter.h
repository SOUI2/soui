#pragma once

#include <core/SSingleton.h>
#if __cplusplus < 201103L
#include <functional>

// 将 闭包 传递到了 UI线程 
// 所以 这里 尽量 将 相同类型的 处理 放到一起 执行  而不是分开调用。

// SendMessage [&] 中的 & 是指 fn里调用的变量 都是 引用拷贝的
#define SRunOnUI(fn)		SNotifyCenter::RunOnUI([&](){fn})

// PostMessage [=] 中的 等号 是指 fn里调用的变量 都是 值拷贝的
#define SRunOnUIA(fn)		SNotifyCenter::RunOnUI([=](){fn})

#endif

namespace SOUI
{
	template<class T>
	class TAutoEventMapReg
	{
		typedef TAutoEventMapReg<T> _thisClass;
	public:
		TAutoEventMapReg()
		{
			SNotifyCenter::getSingleton().RegisterEventMap(Subscriber(&_thisClass::OnEvent,this));
		}

		~TAutoEventMapReg()
		{
			SNotifyCenter::getSingleton().UnregisterEventMap(Subscriber(&_thisClass::OnEvent,this));
		}

	protected:
		bool OnEvent(EventArgs *e){
			T * pThis = static_cast<T*>(this);
			return !!pThis->_HandleEvent(e);
		}
	};

	struct INotifyCallback{
		virtual void OnFireEvent(EventArgs *e) = 0;
	};

	class SNotifyReceiver;

	class SOUI_EXP SNotifyCenter : public SSingleton<SNotifyCenter>
						, public SEventSet
						, protected INotifyCallback
	{
	public:
		SNotifyCenter(void);
		~SNotifyCenter(void);

        /**
        * FireEventSync
        * @brief    触发一个同步通知事件
        * @param    EventArgs *e -- 事件对象
        * @return    
        *
        * Describe  只能在UI线程中调用
        */
		void FireEventSync(EventArgs *e);

        /**
        * FireEventAsync
        * @brief    触发一个异步通知事件
        * @param    EventArgs *e -- 事件对象
        * @return    
        *
        * Describe  可以在非UI线程中调用，EventArgs *e必须是从堆上分配的内存，调用后使用Release释放引用计数
        */
		void FireEventAsync(EventArgs *e);

        /**
        * RegisterEventMap
        * @brief    注册一个处理通知的对象
        * @param    const ISlotFunctor &slot -- 事件处理对象
        * @return    
        *
        * Describe 
        */
		bool RegisterEventMap(const ISlotFunctor &slot);

        /**
        * RegisterEventMap
        * @brief    注销一个处理通知的对象
        * @param    const ISlotFunctor &slot -- 事件处理对象
        * @return    
        *
        * Describe 
        */
		bool UnregisterEventMap(const ISlotFunctor & slot);
	protected:
		virtual void OnFireEvent(EventArgs *e);


		DWORD				m_dwMainTrdID;//主线程ID

		SList<ISlotFunctor*>	m_evtHandlerMap;

		SNotifyReceiver	 *  m_pReceiver;

#if __cplusplus < 201103L
	public:
		static void RunOnUI(std::function<void(void)> fn);
		static void RunOnUIAsync(std::function<void(void)> fn);
#endif
	};
}
