cmake_minimum_required(VERSION 3.5)

project(trial LANGUAGES C CXX)
set(ANDROID_NDK_ROOT "/Users/hectoresteban/Documents/C++/Qt/android-ndk-r21d")
set(CMAKE_PREFIX_PATH "/Users/hectoresteban/Qt/5.15.0/clang_64/lib/cmake/Qt5")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

find_package(Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED)

#set(TS_FILES trial_en_DE.ts)

add_executable(ubimet
  main.cpp
  #${TS_FILES}
  data.cpp
  ImageViewer.cpp
  3rd_party/lodepng.cpp
  3rd_party/heatmap.c
  ${LIBPNG_LINK_FLAGS}
)
target_link_libraries(ubimet Qt5::Core Qt5::Widgets)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

#set_property(TARGET ubimet PROPERTY CXX_STANDARD 17)
