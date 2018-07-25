#おまじない　最低限必要なCmakeのバージョン指定
cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#project name ${PROJECT_NAME}で参照可能
project(sfmDR_footPrint)

#出力情報を増やして cmake デバッグしやすくします。
set(CMAKE_VERBOSE_MAKEFILE 1)

#コンパイルで使用するc++のバージョン指定
set(CMAKE_CXX_STANDARD 11)

#.cmakeファイル = module へのパス
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# release か debugかを指定
set(CMAKE_BUILD_TYPE Debug)

# ちゃんとビルドして生成されたものは
# /build/ディレクトリにしまいましょうという指示
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR
            "CMake generation is not possible within the source directory!"
            "\n Remove the CMakeCache.txt file and try again from another folder, e.g.:"
            "\n "
            "\n rm CMakeCache.txt"
            "\n mkdir build"
            "\n cd build"
            "\n cmake .."
            )
endif()

include(OpenCV)
add_subdirectory(basicFunction)
set(SOURCE_FILES main.cpp Model.cpp Model.h Camera.cpp Camera.h FootPrint.cpp FootPrint.h OpenPosePerson.cpp OpenPosePerson.h ImageInfo.cpp ImageInfo.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} basicFunc)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
