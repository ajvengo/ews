cmake_minimum_required(VERSION 3.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin")

project(ews
    VERSION 0.1
    LANGUAGES CXX C
)

find_package(Threads REQUIRED)
find_package(Boost 1.51 REQUIRED COMPONENTS
    system
    thread
    program_options
    date_time
)

add_library(common INTERFACE)
target_link_libraries(common INTERFACE
    Boost::date_time
    Boost::program_options
    Boost::system
    Boost::thread
    Threads::Threads
)
target_include_directories(common INTERFACE "${CMAKE_SOURCE_DIR}/include")
if(MSVC)
    target_compile_definitions(common INTERFACE
        "_WIN32_WINNT=0x0601"
        "WINVER=0x0601"
    )
endif()

add_executable(${PROJECT_NAME}
    connection.cpp
    json_data.cpp
    main.cpp
    reply.cpp
    request_handler.cpp
    request_parser.cpp
    server.cpp
)
target_link_libraries(${PROJECT_NAME} common)

add_executable(load_test
    stress_test.cpp
)
target_link_libraries(load_test common)
