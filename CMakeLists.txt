project(ken_attach)
cmake_minimum_required(VERSION 3.0)

add_executable(${PROJECT_NAME}
    main.cpp
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}_any
    main_any.cpp
)
target_compile_features(${PROJECT_NAME}_any PRIVATE cxx_std_17)
set_target_properties(${PROJECT_NAME}_any PROPERTIES CXX_STANDARD_REQUIRED ON)

add_executable(performance_test
    performance_test.cpp
)
target_compile_features(performance_test PRIVATE cxx_std_17)
set_target_properties(performance_test PROPERTIES CXX_STANDARD_REQUIRED ON)