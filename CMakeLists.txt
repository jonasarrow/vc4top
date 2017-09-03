project(vc4top CXX)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PKG REQUIRED gtk+-3.0 gmodule-2.0)
include_directories(${PKG_INCLUDE_DIRS})
link_directories(${PKG_LIBRARY_DIRS})
add_definitions(${PKG_CFLAGS_OTHER})
add_definitions("-Wl,--export-dynamic -Wno-psabi")# For automatic signal connection
add_executable(vc4top main.cpp drawing.cpp gtk_window.xml.h)
target_link_libraries(vc4top ${PKG_LIBRARIES} pthread)

install(TARGETS vc4top RUNTIME DESTINATION bin)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jonas Pfeil <pfeiljonas@gmx.de>")
set(CPACK_PACKAGE_NAME "vc4top")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libgtk-3-0 (>= 3.12)")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Performance monitoring tool for the VC4 QPUs")
set(CPACK_PACKAGE_DESCRIPTION "Shows a usage graph of the VC4 QPU processors in the VC4 processor family. This tool needs to be run as root, as it maps the registers into its address space")
set(CPACK_DEBIAN_PACKAGE_SECTION "main")

include(CPack)

add_custom_command(
	OUTPUT gtk_window.xml.h
	COMMAND xxd -i ${CMAKE_CURRENT_SOURCE_DIR}/gtk_window.xml > gtk_window.xml.h
	DEPENDS gtk_window.xml
)
