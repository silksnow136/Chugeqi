cmake_minimum_required(VERSION 3.15)
project(Chugeqi LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
    add_compile_options(/W0)
endif()

add_executable(Chugeqi
    src/combatTestMain.cpp
    src/battleLog.cpp
    src/character.cpp
    src/combatSystem.cpp
    src/console.cpp
    src/dataLoader.cpp
    src/item.cpp
    src/skill.cpp
    src/saveManager.cpp
    third_party/sqlite/sqlite3.c
)

target_include_directories(Chugeqi PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite"
)

add_custom_command(TARGET Chugeqi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:Chugeqi>/data"
)
