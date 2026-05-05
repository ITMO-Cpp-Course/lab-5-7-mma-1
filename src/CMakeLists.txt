add_library(resource_core
        Document_core/src/DocumentBuilder.cpp
        Document_core/src/InvertedIndex.cpp
)

target_include_directories(resource_core PUBLIC Document_core/include)
target_link_libraries(resource_core PUBLIC project_warnings)

if (NOT WIN32)
    target_link_libraries(resource_core PRIVATE project_sanitizers)
endif ()