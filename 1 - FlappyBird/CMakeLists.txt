add_executable(FlappyBird main.c)

target_compile_features(FlappyBird PUBLIC c_std_23)

target_include_directories(FlappyBird PRIVATE include)
target_link_libraries(FlappyBird PRIVATE SDL3::SDL3)
