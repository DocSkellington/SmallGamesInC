set(SOURCE_LIST
    "src/main.c"
    "src/states/startState.c"
    "src/states/optionsState.c"
    "src/states/gameState.c"
    "src/Level.c"
    "src/entities/entities.c"
    "src/entities/player.c"
)

add_executable(CrossingRoads EXCLUDE_FROM_ALL ${SOURCE_LIST})

set_target_properties(CrossingRoads PROPERTIES C_STANDARD 23)
if (MSVC)
  target_compile_options(CrossingRoads PRIVATE /W4 /WERROR)
else()
  target_compile_options(CrossingRoads PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(CrossingRoads PRIVATE include)
target_link_libraries(CrossingRoads PRIVATE Engine)
target_link_libraries(CrossingRoads PRIVATE SDL3_ttf::SDL3_ttf SDL3::SDL3)
target_link_libraries(CrossingRoads PRIVATE m)

add_custom_target(RunCrossingRoads
  COMMAND CrossingRoads
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  VERBATIM
)
