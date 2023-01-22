/*
 * @Descripttion: 
 * @version: 
 * @@Company: DCIT-SH
 * @Author: KarlHarris
 * @Date: 2023-01-21 14:48:13
 * @LastEditors: KarlHarris
 * @LastEditTime: 2023-01-22 00:52:20
 */
#ifdef __SERVER_LOG_H
#ifndef __SERVER_LOG_H

#include<string>
#include<stdint.h>
#include<memory>

namespace server{   

    //日志类型
    class LogEvent(){
        public:
            typedef std::shared_ptr<LogEvent> ptr;
            
            LogEvent();
        private:
            const char* m_file = nullptr;   //文件名
            uint32_t m_line = 0;            //行号
            uint32_t m_elapse = 0;          //程序启动到现在的毫秒数
            uint32_t m_threadId = 0;        //线程ID
            uint32_t m_fiberId = 0;         //协程ID
            uint64_t m_time;                //时间戳
            std::string content;            //内容
    };  

    //日志级别
    class LogLevel{
    public:
        enum level{
            DEBUG = 1;
            INFO = 2;
            WARN = 3;
            ERROR = 4;
            FATAL = 5;
        };
    };
    
    //日志输出地
    class LogAppender{
        public:
            typedef std::shared_ptr<LogAppender> ptr;

            virtual ~LogAppender(){} //不同的输出位置，需要虚析构函数
            void log(LogLevel::level level,LogEvent::ptr event)
        private:
            LogLevel::level m_level;
    };

    //日志格式器
    class LogFormatter{
        public:
            typedef std::shared_ptr<LogFormatter> ptr;

            std::string format(LogEvent::ptr event);
        private:
    };

    //日志器
    class Logger{
        public:
            typedef std::shared_ptr<Logger> ptr;

            Logger(const std::string& name = "root");
            void log(LogLevel::level level,LogEvent::ptr event);
        private:
            std::string m_name;
            LogLevel::level level;
            LogAppender::ptr appenderPtr;
    };

    //输出到控制台
    class StdoutLogAppender:public LogAppender{
        public:
        private:
    }; 

    //输出到文件
    class FileLogAppender:public LogAppender{
        public:
        private:
    };

}
#endif