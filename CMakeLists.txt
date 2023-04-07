CMAKE_MINIMUM_REQUIRED(VERSION 3.22.1)
PROJECT(server)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

#设置动态库的源文件
set(LIB_SRC logger/log.cpp)

#添加一个动态库
ADD_LIBRARY(log SHARED ${LIB_SRC})

#生成可执行文件
ADD_EXECUTABLE(test tests/test.cpp)

#连接动态库
add_dependencies(test log)
target_link_libraries(test log)

#设置可执行文件的输出维持
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#设置动态库的输出位置
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
