# Создаем статическую библиотеку-ядро (имя resource_core требует преподский тест)
add_library(resource_core
        Document_core/src/DocumentBuilder.cpp
        Document_core/src/InvertedIndex.cpp
)

# САМОЕ ВАЖНОЕ: говорим CMake (и CLion), где лежат .hpp файлы!
# PUBLIC означает, что папка include будет доступна и ядру, и тестам
target_include_directories(resource_core PUBLIC Document_core/include)

# Подключаем строгие проверки из корневого CMake (от препода)
target_link_libraries(resource_core PRIVATE project_warnings)

# Подключаем санитайзеры памяти, если они существуют
if (TARGET project_sanitizers)
    target_link_libraries(resource_core PRIVATE project_sanitizers)
endif()

# Собираем консольное приложение (файл main.cpp)
add_executable(Document_cli
        Document_cli/main.cpp
)

# Линкуем наше консольное приложение с библиотекой ядра
target_link_libraries(Document_cli PRIVATE resource_core project_warnings)
if (TARGET project_sanitizers)
    target_link_libraries(Document_cli PRIVATE project_sanitizers)
endif()