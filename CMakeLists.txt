project(vc4top CXX)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})
add_definitions("-Wl,--export-dynamic")# For automatic signal connection
add_executable(vc4top main.cpp gtk_window.xml.h)
target_link_libraries(vc4top ${GTK3_LIBRARIES})

add_custom_command(
	OUTPUT gtk_window.xml.h
	COMMAND xxd -i gtk_window.xml > gtk_window.xml.h
	DEPENDS gtk_window.xml
)