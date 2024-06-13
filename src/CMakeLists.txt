#set(PINE_SRC_SOCKBASE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SockBase/include)
#include_directories(${PINE_SRC_SOCKBASE_INCLUDE_DIR})
#很明显头文件加在这里没什么用，除非要在当前目录下编译时使用这些头文件
add_subdirectory(SockBase)
add_subdirectory(HttpBase)
