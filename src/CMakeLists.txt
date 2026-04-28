
add_library(resource_core
        Document_core/src/DocumentBuilder.cpp
        Document_core/src/InvertedIndex.cpp
)


target_include_directories(resource_core PUBLIC Document_core/include)


target_link_libraries(resource_core PRIVATE project_warnings)


if (TARGET project_sanitizers)
    target_link_libraries(resource_core PRIVATE project_sanitizers)
endif()


add_executable(Document_cli
        Document_cli/main.cpp
)


target_link_libraries(Document_cli PRIVATE resource_core project_warnings)
if (TARGET project_sanitizers)
    target_link_libraries(Document_cli PRIVATE project_sanitizers)
endif()
