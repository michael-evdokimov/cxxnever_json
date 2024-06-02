cmake_minimum_required(VERSION 3.5)

project(cxxnever_json)

enable_testing()

find_package(GTest REQUIRED)

set(CMAKE_CXX_FLAGS "-ggdb3")

file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/src/cxxnever_json/*)
install(FILES ${HEADERS} DESTINATION /usr/include/cxxnever_json/)

add_executable(tester_simple tests/simple.cpp ${HEADERS})
target_link_libraries(tester_simple GTest::GTest GTest::Main)
target_include_directories(tester_simple PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME tester_simple COMMAND tester_simple)

add_executable(tester tests/tester.cpp ${HEADERS})
target_link_libraries(tester GTest::GTest GTest::Main)
target_include_directories(tester PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME tester COMMAND tester)
