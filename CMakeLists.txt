cmake_minimum_required(VERSION 3.13)
project(ubimet_vs)

set(CMAKE_CXX_STANDARD 17)
#set(LDLIBS "-lstdc++fs")
find_package( OpenCV REQUIRED )

include_directories(${OpenCV_INCLUDE_DIRS})

# search for pkg-config
include (FindPkgConfig)
if (NOT PKG_CONFIG_FOUND)
    message (FATAL_ERROR "pkg-config not found")
endif ()

# check for libpng
pkg_check_modules (LIBPNG libpng16 REQUIRED)
if (NOT LIBPNG_FOUND)
    message(FATAL_ERROR "You don't seem to have libpng16 development libraries installed")
else ()
    include_directories (${LIBPNG_INCLUDE_DIRS})
    link_directories (${LIBPNG_LIBRARY_DIRS})
    link_libraries (${LIBPNG_LIBRARIES})
endif ()

add_executable(ubimet main.cpp data.cpp 3rd_party/lodepng.cpp 3rd_party/heatmap.c ${LIBPNG_LINK_FLAGS})
set_property(TARGET ubimet PROPERTY CXX_STANDARD 17)