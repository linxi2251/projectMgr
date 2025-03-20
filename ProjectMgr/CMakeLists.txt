qt_standard_project_setup()

# Create library instead of executable
add_library(projectMgr STATIC
        ProjectMgr.cpp
        ProjectMgr.h
)

target_link_libraries(projectMgr PRIVATE
    Qt6::Core
    Qt6::Xml
)

# Export include directories to other targets that link this library
set_target_properties(projectMgr PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
