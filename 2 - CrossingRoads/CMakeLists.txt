set(SOURCE_LIST
    "src/main.c"
    "src/states/startState.c"
    "src/states/optionsState.c"
)

add_executable(CrossingRoads EXCLUDE_FROM_ALL ${SOURCE_LIST})

set_property(TARGET CrossingRoads PROPERTY C_STANDARD 23)
if (MSVC)
  target_compile_options(Engine PRIVATE /W4 /WERROR)
else()
  target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(CrossingRoads PRIVATE include)
target_link_libraries(CrossingRoads PRIVATE Engine)
target_link_libraries(CrossingRoads PRIVATE SDL3_ttf::SDL3_ttf SDL3::SDL3)

add_custom_target(RunCrossingRoads
  COMMAND CrossingRoads
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  VERBATIM
)
