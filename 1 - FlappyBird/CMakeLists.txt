add_executable(FlappyBird EXCLUDE_FROM_ALL src/main.c)

target_sources(FlappyBird PRIVATE src/Objects.c)

set_property(TARGET FlappyBird PROPERTY C_STANDARD 23)

target_include_directories(FlappyBird PRIVATE include)
target_link_libraries(FlappyBird PRIVATE SDL3::SDL3)
