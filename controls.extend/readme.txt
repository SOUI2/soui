这里提供扩展控件。所有扩展控件在使用前先注册到系统。
假定一个控件名为CTestCtrl
控件注册:
	SApplication::getSingleton().RegisterWndFactory(TplSWindowFactory<CTestCtrl>());
反注册:
	SApplication::getSingleton().UnregisterWndFactory(CTestCtrl::GetClassName());
