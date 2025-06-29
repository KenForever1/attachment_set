project(ken_attach)
cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
    main.cpp
)

add_executable(${PROJECT_NAME}_any
    main_any.cpp
)

add_executable(performance_test
    performance_test.cpp
)