cmake_minimum_required(VERSION 3.2.0)

project (beryl)

set(include_path "src/includes")

file(GLOB input_files src/*.cpp)
file(GLOB header_files "${include_path}/*")



add_library(beryl SHARED ${input_files})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-exceptions -fno-rtti")

target_include_directories(beryl PUBLIC ${include_path})



install(TARGETS beryl
LIBRARY DESTINATION /lib64
CONFIGURATIONS RELEASE
)

install(FILES ${header_files} DESTINATION /usr/include/beryl)

add_executable(demo example/main.cpp)
target_include_directories(demo PRIVATE ${include_path})
target_link_libraries(demo beryl)
