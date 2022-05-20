# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(ABI ${ANDROID_ABI})
if (${ANDROID_ABI} STREQUAL armeabi-v7a)
    set(ABI "armv7a")
elseif (${ANDROID_ABI} STREQUAL arm64-v8a)
    set(ABI "arm64")            
endif ()

set(FFMPEG_ABI_PATH ${CMAKE_SOURCE_DIR}/../../android/contrib/build/ffmpeg-${ABI}/output)

#ffmpeg
add_library(ijkffmpeg
        SHARED
        IMPORTED)

set_target_properties(ijkffmpeg
        PROPERTIES IMPORTED_LOCATION
        ${FFMPEG_ABI_PATH}/libijkffmpeg.so)

set_target_properties(ijkffmpeg
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
        ${FFMPEG_ABI_PATH}/include
        )

ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src/main/cpp/)

