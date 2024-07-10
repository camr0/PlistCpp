cmake_minimum_required(VERSION 3.28)

project(test CXX)

if(WIN32)
	# Build at warning level 4 on Windows.
	# Treat all warnings as errors
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /wd4267 /wd4244")
else()
	# Build with all warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-invalid-utf8 -Wno-unused-parameter -Wno-old-style-cast")
endif()

set(SOURCES PlistTests.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

find_package(Catch2 REQUIRED)

target_link_libraries(${PROJECT_NAME} plistcpp Catch2::Catch2WithMain)

add_custom_command(
				TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_directory
				${CMAKE_SOURCE_DIR}/test/test-data/
				${CMAKE_CURRENT_BINARY_DIR})
