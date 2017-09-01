project(vc4top CXX)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PKG REQUIRED gtk+-3.0 gmodule-2.0)
include_directories(${PKG_INCLUDE_DIRS})
link_directories(${PKG_LIBRARY_DIRS})
add_definitions(${PKG_CFLAGS_OTHER})
add_definitions("-Wl,--export-dynamic -Wno-psabi -ggdb3")# For automatic signal connection
add_executable(vc4top main.cpp drawing.cpp gtk_window.xml.h)
target_link_libraries(vc4top ${GTK3_LIBRARIES} pthread)

add_custom_command(
	OUTPUT gtk_window.xml.h
	COMMAND xxd -i gtk_window.xml > gtk_window.xml.h
	DEPENDS gtk_window.xml
)