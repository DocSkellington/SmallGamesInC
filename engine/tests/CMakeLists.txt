find_package(Check REQUIRED)

SET(STATE_MANAGER_SOURCES
  "EngineTest_main.c"
  "EngineTest_master.c"
  "StateManager.c"
)

add_executable(EngineTest ${STATE_MANAGER_SOURCES})
set_target_properties(EngineTest PROPERTIES C_STANDARD 23)
target_link_libraries(EngineTest Engine ${CHECK_LIBRARIES})

add_test(NAME stateManager COMMAND EngineTest)
