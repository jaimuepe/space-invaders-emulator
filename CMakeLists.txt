cmake_minimum_required(VERSION 3.5.0)
project(SpaceInvaders VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

add_executable(SpaceInvaders 
    src/main.cpp 
    src/reader.h
    src/utils.h
    src/utils.cpp
    src/8080/state8080.h
    src/8080/state8080.cpp
    src/8080/mem8080.h
    src/8080/mem8080.cpp)

set(INVADERS_RES
    ${CMAKE_SOURCE_DIR}/res/invaders.h
    ${CMAKE_SOURCE_DIR}/res/invaders.g
    ${CMAKE_SOURCE_DIR}/res/invaders.f
    ${CMAKE_SOURCE_DIR}/res/invaders.e
)

foreach( file_i ${INVADERS_RES})
    add_custom_command(
        TARGET SpaceInvaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${file_i}
                ${CMAKE_CURRENT_BINARY_DIR})
endforeach( file_i )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
