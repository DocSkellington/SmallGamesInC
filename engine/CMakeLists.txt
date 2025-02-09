set(HEADER_LIST
    "${SmallGames_SOURCE_DIR}/engine/include"
)

set(SOURCE_LIST
    "${SmallGames_SOURCE_DIR}/engine/src/Engine/StateManager.c"
)

add_library(Engine ${SOURCE_LIST} ${HEADER_LIST})

set_property(TARGET Engine PROPERTY C_STANDARD 23)

target_include_directories(Engine PUBLIC include)
target_link_libraries(Engine PUBLIC SDL3::SDL3)
