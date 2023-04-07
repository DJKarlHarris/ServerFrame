/*
 * @Descripttion: 
 * @version: 
 * @@Company: DCIT-SH
 * @Author: KarlHarris
 * @Date: 2023-01-21 14:48:13
 * @LastEditors: KarlHarris
 * @LastEditTime: 2023-01-30 23:56:07
 */


#ifndef __SERVER_LOG_H__
#define __SERVER_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <fstream>
#include <list>
#include <sstream>
#include <sstream>
#include <vector>

namespace harris {   

class Logger;

//日志类型
class LogEvent {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(const char* file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, uint64_t time);

        const char* getFile() const {  return m_file;}
        int32_t getLine() const { return m_line;}
        uint32_t getElapse() const { return m_elapse;}
        uint32_t getThreadId() const { return m_threadId;}
        uint32_t getFiberId() const { return m_fiberId;}
        uint64_t getTime() const { return m_time;}
        const std::string getContext() const { return m_ss.str();}
        std::stringstream& getSs() { return m_ss;}

    private:
        const char* m_file = nullptr;   //文件名
        int32_t m_line = 0;            //行号
        uint32_t m_elapse = 0;          //程序启动到现在的毫秒数
        uint32_t m_threadId = 0 ;        //线程ID
        uint32_t m_fiberId = 0;         //协程ID
        uint64_t m_time;                //时间戳
        std::stringstream m_ss;         //消息
};  

//日志级别
class LogLevel {
public:
    enum level{
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5,
    };
    //将level转化为字符串
    static const char* ToString(LogLevel::level level);
};

//日志格式器
class LogFormatter {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string& pattern);

        std::string format(std::shared_ptr<Logger> ptr, LogLevel::level,LogEvent::ptr event);
    public:
        class FormatItem {
            public:
                typedef std::shared_ptr<FormatItem> ptr;
                virtual ~FormatItem() {};
                virtual void format(std::ostream& os,std::shared_ptr<Logger> ptr, LogLevel::level level,LogEvent::ptr event) = 0;
        };

        void init();//解析pattern
    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
};

    //日志输出地
class LogAppender {                                                        
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender(){}                                        //虚析构函数:解决不同的子类的内存释放问题

        //纯虚函数
        virtual void log(std::shared_ptr<Logger> ptr, LogLevel::level level,LogEvent::ptr event) = 0;   

        void setFormatter(LogFormatter::ptr val) {m_formatter = val;}
        LogFormatter::ptr getFormatter() const { return m_formatter; }
    protected:
        LogLevel::level m_level = LogLevel::DEBUG;                                        //protected:子类有可能使用到
        LogFormatter::ptr m_formatter;
};

//日志器
class Logger : public std::enable_shared_from_this<Logger> {
    public:
        typedef std::shared_ptr<Logger> ptr;
        Logger(const std::string& name = "root");

        void log(LogLevel::level level,LogEvent::ptr event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        //添加删除appender
        void addAppender(LogAppender::ptr logappender);
        void delAppender(LogAppender:: ptr logappender);

        //设置获取日志级别
        LogLevel::level getLevel() const {return m_level;}
        void setLevel(LogLevel::level val) {m_level = val;}

        //获取日志名称
        const std::string& getName() const { return m_name; }

    private:
        std::string m_name;                         //日志名称
        LogLevel::level m_level;                    //日志级别
        std::list<LogAppender::ptr> m_appenders;    //Appender集合
        LogFormatter::ptr m_formatter;
};

//输出到控制台
class StdoutLogAppender:public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;

    virtual void log(Logger::ptr, LogLevel::level level,LogEvent::ptr event) override;
}; 

//输出到文件
class FileLogAppender:public LogAppender {  
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);

    virtual void log(Logger::ptr ptr, LogLevel::level level,LogEvent::ptr event) override;

    //重新打开文件，打开成功返回true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

}

#endif