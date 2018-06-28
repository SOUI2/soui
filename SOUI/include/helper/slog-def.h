#pragma once

//定义一组使用SOUI的log模块的宏
//如果不定义LOG_FILTER，则使用soui作为filter
//可以为不同的CPP定义不同的filter
//例如在demos/demo的demo.cpp中使用下面代码来定义一个demo的filter
/*
#define LOG_FILTER "demo"
#include <helper/slog-def.h>
*/
//经过这样定义，就可以直接使用SLOG_INFO("a="+100)这样的形式来在LOG中输出以demo为filter的LOG了。

#ifndef LOG_FILTER
#define LOG_FILTER "soui"
#endif

#define SLOG_TRACE(log) LOGT(LOG_FILTER, log) 
#define SLOG_DEBUG(log) LOGD(LOG_FILTER, log) 
#define SLOG_INFO(log) LOGI(LOG_FILTER, log)  
#define SLOG_WARN(log) LOGW(LOG_FILTER, log)  
#define SLOG_ERROR(log) LOGE(LOG_FILTER, log) 
#define SLOG_ALARM(log) LOGA(LOG_FILTER, log) 
#define SLOG_FATAL(log) LOGF(LOG_FILTER, log) 


#define SLOGFMTT( fmt, ...) LOGFMTT(LOG_FILTER,  fmt,  ##__VA_ARGS__)
#define SLOGFMTD( fmt, ...) LOGFMTD(LOG_FILTER,  fmt,  ##__VA_ARGS__)
#define SLOGFMTI( fmt, ...) LOGFMTI(LOG_FILTER,  fmt,  ##__VA_ARGS__)
#define SLOGFMTW( fmt, ...) LOGFMTW(LOG_FILTER,  fmt,  ##__VA_ARGS__)
#define SLOGFMTE( fmt, ...) LOGFMTE(LOG_FILTER,  fmt,  ##__VA_ARGS__)
#define SLOGFMTA( fmt, ...) LOGFMTA(LOG_FILTER,  fmt,  ##__VA_ARGS__)
#define SLOGFMTF( fmt, ...) LOGFMTF(LOG_FILTER,  fmt,  ##__VA_ARGS__)
