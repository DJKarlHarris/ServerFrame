CMAKE_MINIMUM_REQUIRED(VERSION 3.22.1)
PROJECT(server)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC logger/log.cpp)

#添加一个动态库
ADD_LIBRARY(log SHARED ${LIB_SRC})

#生成可执行文件
ADD_EXECUTABLE(test tests/test.cpp)

#连接动态库
add_dependencies(test log)
target_link_libraries(test log)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
