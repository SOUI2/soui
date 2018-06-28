#pragma once

#include <unknown/obj-ref-i.h>
#include <time.h>

namespace SOUI{

//! the max log content length.
const int LOG4Z_LOG_BUF_SIZE = 2048;


const int LOG4Z_MAIN_LOGGER_ID = 0;


typedef int LoggerId;

//! LOG Level
enum ENUM_LOG_LEVEL
{
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_ALARM,
    LOG_LEVEL_FATAL,
};

struct IOutputFileBuilder
{
    //每个月创建log文件夹
    virtual bool monthDir() const = 0;
    
    //每天创建log文件
    virtual bool dayLog() const = 0;
    
    //生成LOG文件名
    //至少应该包含pszLogName，及curFileIndex这两个参数
    virtual bool buildOutputFile(char *pszFileName,int nLen,tm time,const char * pszLogName,unsigned long pid,int curFileIndex) const =0;
};

//! log4z class
struct  ILog4zManager : public IObjRef
{
    virtual ~ILog4zManager(){};

    //! Config or overwrite configure
    //! Needs to be called before ILog4zManager::Start,, OR Do not call.
    virtual bool config(const char * configPath) = 0;
    virtual bool configFromString(const char * configContent) = 0;

    //! Create or overwrite logger.
    //! Needs to be called before ILog4zManager::Start, OR Do not call.
    virtual LoggerId createLogger(const char* key) = 0;

    //! Start Log Thread. This method can only be called once by one process.
    virtual bool start() = 0;

    //! Default the method will be calling at process exit auto.
    //! Default no need to call and no recommended.
    virtual bool stop() = 0;

    //! Find logger. thread safe.
    virtual LoggerId findLogger(const char* key) =0;

    //pre-check the log filter. if filter out return false. 
    virtual bool prePushLog(LoggerId id, int level) = 0;
    virtual bool prePushLog(const char * name, int level) = 0;
    
    //! Push log, thread safe.
    virtual bool pushLog(LoggerId id, int level, const char * filter, const char * log, const char * file , int line , const char * func , const void *pRetAddr) = 0;

    virtual bool pushLog(const char * name, int level, const char * filter, const char * log, const char * file , int line , const char * func , const void *pRetAddr) = 0;

    //! set logger's attribute, thread safe.
    virtual bool enableLogger(LoggerId id, bool enable) = 0;
    virtual bool setLoggerName(LoggerId id, const char * name) = 0;
    virtual bool setLoggerPath(LoggerId id, const char * path) = 0;
    virtual bool setLoggerLevel(LoggerId id, int nLevel) = 0;
    virtual bool setLoggerFileLine(LoggerId id, bool enable) = 0;
    virtual bool setLoggerDisplay(LoggerId id, bool enable) = 0;
    virtual bool setLoggerOutFile(LoggerId id, bool enable) = 0;
    virtual bool setLoggerLimitsize(LoggerId id, unsigned int limitsize) = 0;
    
    //设置LOG输出到文件的规则
    virtual void setOutputFileBuilder(IOutputFileBuilder *pOutputFileBuilder) = 0;

    //! Update logger's attribute from config file, thread safe.
    virtual bool setAutoUpdate(int interval/*per second, 0 is disable auto update*/) = 0;
    virtual bool updateConfig() = 0;

    //! Log4z status statistics, thread safe.
    virtual bool isLoggerEnable(LoggerId id) = 0;
    virtual unsigned long long getStatusTotalWriteCount() = 0;
    virtual unsigned long long getStatusTotalWriteBytes() = 0;
    virtual unsigned long long getStatusWaitingCount() = 0;
    virtual unsigned int getStatusActiveLoggers() = 0;
};

}