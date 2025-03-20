cmake_minimum_required(VERSION 3.16)

project(project-mgr VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 REQUIRED COMPONENTS Core Xml)

add_subdirectory(ProjectMgr)

# Add test directory
set(PROJECT_MGR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ProjectMgr)
add_subdirectory(test)