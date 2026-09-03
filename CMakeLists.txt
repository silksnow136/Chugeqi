cmake_minimum_required(VERSION 3.15)
project(TheFalloftheOverlord LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(TheFalloftheOverlord
    main.cpp
    Game.cpp
    SceneManager.cpp
    TalkManager.cpp
    map.cpp
    BackGround.cpp
    ForgeManager.cpp
    PharManager.cpp
    console.cpp
    story.cpp
)

add_custom_command(TARGET TheFalloftheOverlord POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:TheFalloftheOverlord>/data"
)
