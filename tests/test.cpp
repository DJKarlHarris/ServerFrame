#include "../logger/log.h"
#include <iostream>

int main(int argc, char** argv) {
    //初始化日志器
    harris::Logger::ptr logger(new harris::Logger);
    //加入std输出器
    logger->addAppender(harris::LogAppender::ptr(new harris::StdoutLogAppender));

    harris::LogEvent::ptr event;
    
    event.reset(new harris::LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));
    logger->debug(event);

    event.reset(new harris::LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));
    logger->info(event);

}