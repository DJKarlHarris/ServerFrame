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
#include <memory>
#include <utility>
#include <tuple>
#include <map>
#include <functional>
#include <sstream>



namespace harris {
    LogEvent::LogEvent(const char* file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, uint64_t time) 
        : m_file(file), 
        m_line(line), 
        m_elapse(elapse), 
        m_threadId(threadId), 
        m_fiberId(fiberId), 
        m_time(time) {}
 

class MessageFormatItem : public LogFormatter::FormatItem {
public:
    MessageFormatItem(const std::string& str = "") {}
    void format(std::ostream& os,std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override { 
        os << event->getContext();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os,std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override { 
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem {
public:
    ElapseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class NameFormatItem : public LogFormatter::FormatItem {
public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override {
        os << ptr->getName();
    }
};

class ThreadFormatItem : public LogFormatter::FormatItem {
public:
    ThreadFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override {
        os << event->getThreadId(); 
    }
};

class FiberFormatItem : public LogFormatter::FormatItem {
public:
    FiberFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override {
        os << event->getFiberId(); 
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override {
        os << event->getLine(); 
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y:%m:%d %H:%M:%S")
        :m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y:%m:%d %H:%M:%S";
        }
    } 
    void format(std::ostream& os, std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override {
        struct tm tm; 
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
     
    }
private:
    std::string m_format;
};

class FileFormatItem : public LogFormatter::FormatItem {
public:
    FileFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override {
        os << event->getFile(); 
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override {
        os << std::endl; 
    }
};

class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str)
        :m_string(str) {}
    void format(std::ostream& os, std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) override {
        os << m_string; 
    }
private:
    std::string m_string;
};


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

Logger::Logger(const std::string& name) 
    : m_name(name)
    , m_level(LogLevel::DEBUG) {
        m_formatter.reset(new LogFormatter("%d [%p] <%f:%l> %m %n"));
    }

void Logger::log(LogLevel::level level,LogEvent::ptr event){
    //init?
    if(level>=m_level){
        //使用shared_from_this(),类要继承std::enable_shared_from_this<T>
        auto self = shared_from_this();
        for(auto& i:m_appenders){
            i->log(self, level, event);
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

void Logger::addAppender(LogAppender::ptr appender) {
    //没有设置设置格式器导致段错误
    if(!appender->getFormatter()) {
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
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

//文件输出构造函数
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

void FileLogAppender::log(std::shared_ptr<Logger> ptr, LogLevel::level level, LogEvent::ptr event) {
    if(level >= m_level) {
        m_filestream << m_formatter->format(ptr, level, event);
    }
}

void StdoutLogAppender::log(std::shared_ptr<Logger> ptr, LogLevel::level level,LogEvent::ptr event) {
    if(level >= m_level) {
        std::cout << m_formatter->format(ptr, level, event);
    }
}

LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern) {
        init();
    }

std::string LogFormatter::format(std::shared_ptr<Logger> ptr, LogLevel::level level,LogEvent::ptr event) {
    std::stringstream ss;
    for(auto& m_item:m_items) {
        m_item->format(ss, ptr, level, event);
    }
    return ss.str();
}

//格式解析，将格式解析成三元组的形式存储在数组中
//%xxx %xxx{xxx} %%
void LogFormatter::init() {
    //三元组格式 str, format, type
    //%d [%p] %f %l %m %n
    std::vector<std::tuple<std::string,std::string,int>> vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); i++) {
        //m_pattern[i]!=%时
        if(m_pattern[i] != '%') {
            nstr.append(1,m_pattern[i]);
            continue;
        }

        // %%
        if((i+1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        //m_pattern[i]==%时开始解析
        size_t n = i + 1;
        std::string str;
        std::string fmt;
        int fmt_status = 0; // 状态0:解析str 状态1:解析format 状态2. 解析完毕
        size_t fmt_begin = 0;

        //解析带有格式的token
        while(n < m_pattern.size()) {
            if(!isalpha(m_pattern[n]) && m_pattern[n]!='{' 
            && m_pattern[n] != '}') {//情况为 %xxx或%
                break;
            }
            //%xxx{ 取出xxx
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') { //（m_pattern[i],m_pattern[n]）为str
                    str = m_pattern.substr(i + 1,n - i - 1);
                    fmt_status = 1;
                    fmt_begin = n;
                    n++;        //是否可以删去优化掉呢？
                    continue;
                }
            }
            // %xxx{fmt} 取出fmt
            if(fmt_status == 1) { 
                if(m_pattern[n] == '}') { //（m_pattern[for_begin],m_pattern[n]）为format
                    fmt = m_pattern.substr(fmt_begin + 1,n - fmt_begin - 1);
                    fmt_status = 2;
                    //%xxx{xxx}解析完成
                    break;
                }
            }
            n++;
        }

        //插入vector
        if(fmt_status == 0) { //%xxx
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr ,std::string(), 0));
                nstr.clear();
            }
            str = m_pattern.substr(i + 1, n - i - 1);
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) { //%xxx{xx 未解析到}就退出循环了 格式解析错误
            std::cout << "pattern parse error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;
            vec.push_back(make_tuple("<<pattern error>>",fmt,0));
        } else if(fmt_status == 2) { //%xxx{xxx}        
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr,"",0));
               nstr.clear();
            }
            vec.push_back(make_tuple(str,fmt,1));
            i = n - 1;
        }
    }   

    if(!nstr.empty()) { //对nstr操作是否冗余？
        vec.push_back(std::make_tuple(nstr,"",0));
    }

    //神奇的宏定义，值得学习
    //string 映射 函数 返回对应的formatitem
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> m_format_item = {
        //{m, [](const std::string& fmt) {return FormatItem::ptr(new MessageFormatItem(fmt)); }}
#define xx(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt)); }}
        xx(m, MessageFormatItem),
        xx(p, LevelFormatItem),
        xx(r, ElapseFormatItem),
        xx(c, NameFormatItem),
        xx(t, ThreadFormatItem),
        xx(n, NewLineFormatItem),
        xx(d, DateTimeFormatItem),
        xx(f, FileFormatItem),
        xx(l, LineFormatItem),
#undef xx
    };

    //将需要输出的对象加入m_items
    for(auto& i : vec) {
        //stringFormatItem
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            //map映射
            auto it = m_format_item.find(std::get<0>(i));
            if(it == m_format_item.end()) {
            //error
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
            } else {
                //xxxFormatItem
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    std::cout << m_items.size() <<std::endl;
    }

}