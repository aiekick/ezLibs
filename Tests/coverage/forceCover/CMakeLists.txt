cmake_minimum_required(VERSION 3.10)

project(forceCoverProject LANGUAGES CXX)

add_custom_target(forceCover ALL
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)