cmake_minimum_required(VERSION 3.28)
project(PlistMain)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(test)

add_dependencies(test plistcpp)
