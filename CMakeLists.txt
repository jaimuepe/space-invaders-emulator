cmake_minimum_required(VERSION 3.5.0)
project(SpaceInvaders VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)

add_executable(SpaceInvaders 
    src/main.cpp 
    src/reader.h
    src/reader.cpp
    src/utils.h
    src/utils.cpp
    src/SpaceInvaders/space_invaders.h
    src/SpaceInvaders/space_invaders.cpp
    src/SpaceInvaders/space_invaders_view.h
    src/SpaceInvaders/space_invaders_view_sdl.h
    src/SpaceInvaders/space_invaders_view_sdl.cpp
    src/8080/emulator8080.h
    src/8080/emulator8080.cpp
    src/8080/state8080.h
    src/8080/state8080.cpp
    src/8080/mem8080.h
    src/8080/mem8080.cpp)

target_link_libraries(SpaceInvaders PRIVATE SDL3::SDL3)

if (WIN32)
    add_custom_command(
        TARGET SpaceInvaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "$<TARGET_FILE:SDL3::SDL3>" 
            "$<TARGET_FILE_DIR:SpaceInvaders>"
        VERBATIM)
endif()

add_custom_command(
    TARGET SpaceInvaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res/
        $<TARGET_FILE_DIR:SpaceInvaders>/res)