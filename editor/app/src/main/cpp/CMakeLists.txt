# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_CRYPTO")
set(PATH_TO_CPP ${CMAKE_SOURCE_DIR}/src/main/cpp)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

file(GLOB PATH_TO_CPP_FILE
        ${PATH_TO_CPP}/*.cpp
        ${PATH_TO_CPP}/filter/*.cpp)

add_library(
        media_editor

        SHARED

        ${PATH_TO_CPP_FILE}
        ${PATH_TO_RTMP_C_FILE}
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        media_editor
        OpenSLES
        # Links the target library to the log library
        # included in the NDK.
        ijkffmpeg
        ${log-lib})