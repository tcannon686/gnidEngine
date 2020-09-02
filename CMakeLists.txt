cmake_minimum_required(VERSION 3.18)
project(
    gnid
    LANGUAGES                          C CXX
    DESCRIPTION                        "A simple game engine")

set(CMAKE_CXX_STANDARD                 17)
set(CMAKE_CXX_STANDARD_REQUIRED        True)

################################### SOURCES ####################################

file(
    GLOB                               ${PROJECT_NAME}_SOURCES
    LIST_DIRECTORIES                   false
    src/gnid/*.cpp)

file(
    GLOB                               ${PROJECT_NAME}_HEADERS
    LIST_DIRECTORIES                   false
    include/gnid/*.hpp)

################################# DEPENDENCIES #################################

file(
    GLOB                               glad_SOURCES
    LIST_DIRECTORIES                   false
    src/gnid/glad/*.c)

file(
    GLOB                               glad_HEADERS
    LIST_DIRECTORIES                   false
    include/gnid/glad/*.h)

file(
    GLOB                               matrix_SOURCES
    LIST_DIRECTORIES                   false
    src/gnid/matrix/*.cpp)

file(
    GLOB                               matrix_HEADERS
    LIST_DIRECTORIES                   false
    include/gnid/matrix/*.hpp)

################################### TARGETS ####################################

add_library(${PROJECT_NAME}            STATIC
                                       ${${PROJECT_NAME}_SOURCES}
                                       ${glad_SOURCES}
                                       ${matrix_SOURCES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC                             include)

################################### INSTALL ####################################

install(
    TARGETS                            ${PROJECT_NAME}
    DESTINATION                        lib)

install(
    FILES                              ${${PROJECT_NAME}_HEADERS}
    DESTINATION                        include/gnid)

install(
    FILES                              ${glad_HEADERS}
    DESTINATION                        include/gnid/glad)

install(
    FILES                              ${matrix_HEADERS}
    DESTINATION                        include/gnid/matrix)
