## 相关代码说明

1.依据字体描述字符串fontStyle创建字体：D2UI::CFontPool::getSingleton().GetFont(fontStyle, 100);
2.依据文字和字体计算显示宽度：pRT->MeasureText(pszText, _tcslen(pszText), &sz);
