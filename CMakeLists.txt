cmake_minimum_required(VERSION 2.8)

project(dedup)

find_package(Boost REQUIRED COMPONENTS filesystem regex)

add_executable(${PROJECT_NAME} "main.cpp" "levenshtein_distance.hpp")
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
