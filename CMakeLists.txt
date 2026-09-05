cmake_minimum_required(VERSION 3.15)
project(Chugeqi LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

# 主游戏（剧情 + 战斗）
add_executable(Chugeqi
    src/main.cpp
    # 剧情/场景系统（story）
    src/story/Game.cpp
    src/story/SceneManager.cpp
    src/story/TalkManager.cpp
    src/story/map.cpp
    src/story/BackGround.cpp
    src/story/ForgeManager.cpp
    src/story/PharManager.cpp
    # 战斗系统（combat）
    src/combat/battleLog.cpp
    src/combat/character.cpp
    src/combat/combatSystem.cpp
    src/combat/item.cpp
    src/combat/skill.cpp
    src/combat/saveManager.cpp
    # 平台抽象层 / 数据加载（core）
    src/core/console.cpp
    src/core/dataLoader.cpp
    # 第三方
    third_party/sqlite/sqlite3.c
)

# 战斗测试（独立入口）
add_executable(combatTest
    src/combat/combatTestMain.cpp
    src/combat/battleLog.cpp
    src/combat/character.cpp
    src/combat/combatSystem.cpp
    src/combat/item.cpp
    src/combat/skill.cpp
    src/combat/saveManager.cpp
    src/core/console.cpp
    src/core/dataLoader.cpp
    third_party/sqlite/sqlite3.c
)

foreach(TGT Chugeqi combatTest)
    target_include_directories(${TGT} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite"
    )
    add_custom_command(TARGET ${TGT} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/data"
            "$<TARGET_FILE_DIR:${TGT}>/data"
    )
endforeach()
