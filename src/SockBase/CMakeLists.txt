file(GLOB_RECURSE project_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
#file(GLOB_RECURSE project_sources *.cpp)

#上面两个语句等效  
add_library(sockbase_lib SHARED ${project_sources})	
