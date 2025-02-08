cmake_minimum_required(VERSION 3.28...3.30)

project(SmallGames  VERSION 0.1
                    DESCRIPTION "A collection of small games written in C"
                    LANGUAGES C)

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_subdirectory("1 - FlappyBird")
