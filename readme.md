<!--
 * @Descripttion: 
 * @version: 
 * @@Company: DCIT-SH
 * @Author: KarlHarris
 * @Date: 2023-01-22 16:31:47
 * @LastEditors: KarlHarris
 * @LastEditTime: 2023-01-29 21:43:10
-->
## 开发环境
    Centos7
    gcc 9.1
    cmake
## 日志系统

1.仿照log4j实现

    Logger(定义日志类别）
        |
        | --------------Formatter（日志格式）
        |
    Appender(日志输出地方）
    
    //框架级和业务级产生的debug信息分开
    

## 协程库封装

## socket函数库

## http协议开发

## 分布协议

## 推荐系统

例如，%d - %m%n或%d{yyyy-MM-dd HH:mm:ss} %p [%c] %m%n
%c 输出日志信息所属的类的全名
%d 输出日志时间点的日期或时间，默认格式为ISO8601，也可以在其后指定格式，比如：%d{yyy-M-dd HH:mm:ss }，输出类似：2002-10-18- 22：10：28
%f 输出日志信息所属的类的类名
%l 输出日志事件的发生位置，即输出日志信息的语句处于它所在的类的第几行
%m 输出代码中指定的信息，如log(message)中的message
%n 输出一个回车换行符，Windows平台为“rn”，Unix平台为“n”
%p 输出优先级，即DEBUG，INFO，WARN，ERROR，FATAL。如果是调用debug()输出的，则为DEBUG，依此类推
%r 输出自应用启动到输出该日志信息所耗费的毫秒数
%t 输出产生该日志事件的线程名
