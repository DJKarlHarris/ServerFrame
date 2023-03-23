#include "../logger/log.h"
#include <iostream>

int main(int argc, char** argv) {
    harris::Logger::ptr logger(new harris::Logger);
    logger->addAppender(harris::LogAppender::ptr(new harris::StdoutLogAppender));

    harris::LogEvent::ptr event(new harris::LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));
    logger->log(harris::LogLevel::DEBUG, event);
    
    std::cout << "hello!" <<std::endl;

}