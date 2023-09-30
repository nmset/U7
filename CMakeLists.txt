cmake_minimum_required(VERSION 3.5)

project(u7)

add_executable(utf8util main.cpp)

install(TARGETS utf8util RUNTIME DESTINATION bin)

target_link_libraries(utf8util utf8proc)

