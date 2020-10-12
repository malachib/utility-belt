cmake_minimum_required(VERSION 3.5)

project(utility-belt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)

# Somehow, adding anything other than 'main.cpp qml.qrc' creates an absolute path UI element
# in QT Creator for source files.  More obnoxious behaviors
set(PROJECT_SOURCE_FILES main.cpp boilerplate.cpp service.cpp qml.qrc)

if(ANDROID)
    add_library(utility-belt SHARED ${PROJECT_SOURCE_FILES})
else()
    add_executable(utility-belt ${PROJECT_SOURCE_FILES})
endif()

set(CONAN_DIR ${CMAKE_SOURCE_DIR}/../../conan)
include(${CONAN_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

target_compile_definitions(utility-belt
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(utility-belt
  PRIVATE Qt5::Core Qt5::Quick CONAN_PKG::entt)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# These variables must use CACHE, otherwise QtCreator won't see them.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android" CACHE INTERNAL "")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so CACHE INTERNAL "")
#    endif()
#endif()
