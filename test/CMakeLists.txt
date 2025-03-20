cmake_minimum_required(VERSION 3.20)

project(testProjectMgr LANGUAGES CXX)

enable_testing()
set(CMAKE_AUTOMOC ON)

# Test application for projectMgr library
add_executable(testProjectMgr
    main.cpp
)
target_include_directories(testProjectMgr PUBLIC ${PROJECT_MGR_INCLUDE_DIR})
target_link_libraries(testProjectMgr PRIVATE
    projectMgr
    Qt6::Core
    Qt6::Xml
)

