cmake_minimum_required(VERSION 3.15)
project(Chugeqi LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(Chugeqi
    # 剧情/场景系统（TheFall）
    main.cpp
    Game.cpp
    SceneManager.cpp
    TalkManager.cpp
    map.cpp
    BackGround.cpp
    ForgeManager.cpp
    PharManager.cpp
    story.cpp
    # 战斗系统（Chugeqi）
    src/battleLog.cpp
    src/character.cpp
    src/combatSystem.cpp
    src/dataLoader.cpp
    src/item.cpp
    src/skill.cpp
    src/saveManager.cpp
    # 平台抽象层
    src/console.cpp
    # 第三方
    third_party/sqlite/sqlite3.c
)

target_include_directories(Chugeqi PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite"
)

add_custom_command(TARGET Chugeqi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:Chugeqi>/data"
)
