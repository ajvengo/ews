cmake_minimum_required(VERSION 3.0.0)
project(ews)

find_package(Threads REQUIRED)
find_package(Boost 1.51 REQUIRED COMPONENTS system thread program_options date_time)

if(MSVC)
 add_definitions(/D_WIN32_WINNT=0x0501 /DBOOST_ALL_DYN_LINK)
 set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LIBPATH:${Boost_LIBRARY_DIRS}")
endif(MSVC)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(include ${Boost_INCLUDE_DIRS})

add_executable(ews main.cpp connection.cpp reply.cpp json_data.cpp request_handler.cpp request_parser.cpp server.cpp)
target_link_libraries(ews ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(stress_test stress_test.cpp)
target_link_libraries(stress_test ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
