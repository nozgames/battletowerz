cmake_minimum_required(VERSION 3.15)

project(battletowerz)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory(noz noz)
    
set(SOURCE_FILES
    src/game.cpp
    src/battle.cpp
    src/main.cpp
    src/entity.cpp
    src/unit.cpp
    src/unit_database.cpp
    src/menu.cpp
    src/world.cpp
    src/editor.cpp
    src/units/archer.cpp
    src/units/tower.cpp
    src/units/knight.cpp
    src/game_assets.cpp
)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")
endif()

add_executable(battletowerz ${SOURCE_FILES} res/windows/battletowerz.rc)
set_target_properties(battletowerz PROPERTIES WIN32_EXECUTABLE TRUE)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT battletowerz)
target_precompile_headers(battletowerz PUBLIC src/pch.h)
target_link_libraries(battletowerz noz)
target_include_directories(battletowerz PRIVATE
        src
        res/windows
)
target_compile_definitions(battletowerz PRIVATE GLM_ENABLE_EXPERIMENTAL)

