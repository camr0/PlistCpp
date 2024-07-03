cmake_minimum_required(VERSION 3.28)

project(PlistCpp CXX)

include(GNUInstallDirs)

if (WIN32)
	# Build at warning level 4 on Windows.
	# Treat all warnings as errors
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else ()
	# Build with all warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-invalid-utf8 -Wno-unused-parameter -Wno-old-style-cast")
endif ()

set(SOURCES Plist.cpp PlistDate.cpp)
add_library(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
								CXX_STANDARD 17
								CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS locale)
find_package(catch2 REQUIRED)
find_package(pugixml REQUIRED)

target_link_libraries(${PROJECT_NAME} Boost::locale Catch2::Catch2WithMain pugixml::pugixml)

target_include_directories(${PROJECT_NAME} PUBLIC
				$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
				$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  # for client in install mode
)

# Install the library and its headers
install(TARGETS ${PROJECT_NAME}
		EXPORT ${PROJECT_NAME}_targets
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(DIRECTORY src/ DESTINATION include/plistcpp FILES_MATCHING PATTERN "*.h")

# Generate and install *-targets.cmake
install(EXPORT ${PROJECT_NAME}_targets
		NAMESPACE ${PROJECT_NAME}::
		FILE ${PROJECT_NAME}-targets.cmake
		DESTINATION share/${PROJECT_NAME})

# Generate the config file in the current binary dir (this ensures it's not placed directly in source)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
"include(CMakeFindDependencyMacro)\n"
"find_dependency(Boost REQUIRED COMPONENTS filesystem)\n"
"find_dependency(pugixml::pugixml REQUIRED)\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}-targets.cmake\")\n"
)

# Install the generated config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
								DESTINATION share/${PROJECT_NAME})
