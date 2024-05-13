cmake_minimum_required(VERSION 3.5.0)
project(SpaceInvaders VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)

    include(FetchContent)

    Set(FETCHCONTENT_QUIET FALSE)

    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.3
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)

    FetchContent_MakeAvailable(SDL2)

    SET(SDL2_INCLUDE_DIRS ${SDL2_SOURCE_DIR}/include)
    SET(SDL2_LIBRARIES SDL2::SDL2)

else()
    find_package(SDL2 REQUIRED)
endif()

include_directories(${SDL2_INCLUDE_DIRS})

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
    src/8080/mem8080.cpp
    src/8080/CPUDiag/cpu_diag.h
    src/8080/CPUDiag/cpu_diag.cpp)

add_compile_definitions(CPU_DIAG)

target_link_libraries(SpaceInvaders PRIVATE ${SDL2_LIBRARIES})

if (WIN32)
    add_custom_command(
        TARGET SpaceInvaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            "$<TARGET_FILE:SDL2::SDL2>" 
            "$<TARGET_FILE_DIR:SpaceInvaders>"
        VERBATIM)
endif()

add_custom_command(
    TARGET SpaceInvaders POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res/
        $<TARGET_FILE_DIR:SpaceInvaders>/res)