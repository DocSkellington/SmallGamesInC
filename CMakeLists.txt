cmake_minimum_required(VERSION 3.28...3.30)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(SmallGames  VERSION 0.1
                    DESCRIPTION "A collection of small games written in C"
                    LANGUAGES C)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

add_subdirectory("1 - FlappyBird")

add_subdirectory(engine)
