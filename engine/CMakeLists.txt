set(HEADER_LIST
    "${SmallGames_SOURCE_DIR}/engine/include"
)

set(SOURCE_LIST
    "${SmallGames_SOURCE_DIR}/engine/src/StateManager.c"
)

add_library(Engine ${SOURCE_LIST} ${HEADER_LIST})

set_target_properties(Engine PROPERTIES C_STANDARD 23)

target_include_directories(Engine PUBLIC include)
target_link_libraries(Engine PUBLIC SDL3::SDL3)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
