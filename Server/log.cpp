/*
 * @Descripttion: 
 * @version: 
 * @@Company: DCIT-SH
 * @Author: KarlHarris
 * @Date: 2023-01-23 13:43:11
 * @LastEditors: KarlHarris
 * @LastEditTime: 2023-01-30 23:55:51
 */
#include "log.h"
#include <iostream>
#include <utility>
#include <tuple>

namespace harris { 

    const char* LogLevel::ToString(LogLevel::level level) {
        switch(level) {

            //知识点：宏函数简化代码
            #define CASE(name)\
            case LogLevel::name: \
                return #name;\
                break;

            CASE(DEBUG);
            CASE(INFO);
            CASE(WARN);
            CASE(ERROR);
            CASE(FATAL);
            #undef CASE

            default:
                return "UNKNOW";
        }
    }

    Logger::Logger(const std::string& name){
        m_name = name;
    }

    void Logger::log(LogLevel::level level,LogEvent::ptr event){
        if(level>=m_level){
            for(auto& i:m_appenders){
                i->log(level,event);
            }
        }
    }
    
    void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG,event);
    }

    void Logger::info(LogEvent::ptr event) {
        log(LogLevel::INFO,event);
    }

    void Logger::warn(LogEvent::ptr event) {
        log(LogLevel::WARN,event);
    }

    void Logger::error(LogEvent::ptr event) {
        log(LogLevel::ERROR,event);
    }

    void Logger::fatal(LogEvent::ptr event) {
        log(LogLevel::FATAL,event);
    }

    void Logger::addAppender(LogAppender::ptr logappender) {
        m_appenders.push_back(logappender);
    }

    void Logger::delAppender(LogAppender::ptr logappender) {
        for(auto it=m_appenders.begin();
            it!=m_appenders.end();it++) {
                if(*it == logappender) {
                    m_appenders.erase(it);
                    break;
                }
        }
    }
    
    FileLogAppender::FileLogAppender(const std::string& filename)
        :m_filename(filename) {
        }

    bool FileLogAppender::reopen() {
        if(m_filestream) {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        return !!m_filestream;
    }
    
    void FileLogAppender::log(LogLevel::level level, LogEvent::ptr event) {
        if(level >= m_level) {
            m_filestream << m_formatter->format(level,event);
        }
    }

    void StdoutLogAppender::log(LogLevel::level level,LogEvent::ptr event) {
        if(level >= m_level) {
            std::cout << m_formatter->format(level,event);
        }
    }

    LogFormatter::LogFormatter(const std::string& pattern)
        :m_pattern(pattern) {

        }
    
    std::string LogFormatter::format(LogLevel::level level,LogEvent::ptr event) {
        std::stringstream ss;
        for(auto& i:m_items) {
            i->format(ss,level,event);
        }
        return ss.str();
    }

    //格式解析，将格式解析成三元组的形式存储在数组中
    //%xxx %xxx{xxx} %%
    void LogFormatter::init() {
        //三元组格式 str, format, type
        std::vector<std::tuple<std::string,std::string,int>> vec;
        std::string nstr;
        for(size_t i = 0;i < m_pattern.size(); i++) {
            //m_pattern[i]!=%时
            if(m_pattern[i]!='%') {
                nstr.append(1,m_pattern[i]);
                continue;
            }


            //m_pattern[i]==%时开始解析
            size_t n = i + 1;
            std::string str;
            std::string fmt;
            int fmt_status = 0; // 状态0:解析str 状态1:解析format 状态2. 解析完毕
            size_t fmt_begin;
            while(n < m_pattern.size()) {
                if(isspace(m_pattern[n])) {//情况为 %xxx或%
                    break;
                }
                if(fmt_status == 0) {
                    if(m_pattern[n] == '{') { //（m_pattern[i],m_pattern[n]）为str
                        str = m_pattern.substr(i + 1,n - i - 1);
                        fmt_status = 1;
                        fmt_begin = n;
                        n++;        //是否可以删去优化掉呢？
                        continue;
                    }
                }
                if(fmt_status == 1) {
                    if(m_pattern[n] == '}') { //（m_pattern[for_begin],m_pattern[n]）为format
                        fmt = m_pattern.substr(fmt_begin + 1,n - fmt_begin - 1);
                        fmt_status = 2;
                        //%xxx{xxx}解析完成
                        break;
                    }
                }
                
                //n++; 自增？
            }

            if(fmt_status == 0) { //%xxx
                if(!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr,"",0));
                }
                str = m_pattern.substr(i + 1,n - i - 1);
                vec.push_back(std::make_tuple(str,fmt,1));
                i = n;
            } else if(fmt_status == 1) { //%xxx{xx 未解析到}就退出循环了
                std::cout << "pattern parse error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;
                vec.push_back(make_tuple("<<pattern error>>",fmt,0));
            } else if(fmt_status == 2) { //%xxx{xxx}        
                if(!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr,"",0));
                }
                vec.push_back(make_tuple(str,fmt,1));
                i=n;
            }
        }   
        if(!nstr.empty()) { //对nstr操作是否冗余？
            vec.push_back(std::make_tuple(nstr,"",0));
        }
    }

    class MessageFormatItem : public LogFormatter::FormateItem {
        public:
            void format(std::ostream& os,LogLevel::level level, LogEvent::ptr event) override { 
                os << event->getContext();
            }
    };

    class LevelFormatItem : public LogFormatter::FormateItem {
        public:
            void format(std::ostream& os,LogLevel::level level, LogEvent::ptr event) override { 
                os << LogLevel::ToString(level);
            }
    };



}