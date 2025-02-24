option(BUILD_DOCUMENTATION "Do you want to build the engine's documentation?")

set(HEADER_LIST
  "${SmallGames_SOURCE_DIR}/engine/include"
)

set(SOURCE_LIST
  "${SmallGames_SOURCE_DIR}/engine/src/StateManager.c"
  "${SmallGames_SOURCE_DIR}/engine/src/Bindings.c"
  "${SmallGames_SOURCE_DIR}/engine/src/Options.c"
)

add_library(Engine ${SOURCE_LIST} ${HEADER_LIST})

set_target_properties(Engine PROPERTIES C_STANDARD 23)
if (MSVC)
  target_compile_options(Engine PRIVATE /W4 /WERROR)
else()
  target_compile_options(Engine PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_include_directories(Engine PUBLIC include)
target_link_libraries(Engine PUBLIC SDL3::SDL3 PUBLIC PkgConfig::glib)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_DOCUMENTATION)
  find_package(Doxygen REQUIRED dot)
  doxygen_add_docs(EngineDoc ALL CONFIG_FILE "${SmallGames_SOURCE_DIR}/engine/doxyfile")
endif()
