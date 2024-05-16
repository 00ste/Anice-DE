cmake_minimum_required(VERSION 3.28)

set(PROJECT_NAME anice_cpp)

project(${PROJECT_NAME})

find_package(X11 REQUIRED)

set(SOURCES AAtoms.cpp
        AAtoms.h
        AMonitor.cpp
        AMonitor.h
        AUtil.h
        AWindow.cpp
        AWindow.h
        AWindowManager.cpp
        AWindowManager.h
        AWorkspace.h
        main.cpp
        AWorkspace.cpp
        AUtil.cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
