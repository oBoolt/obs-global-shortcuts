cmake_minimum_required(VERSION 4.4.2)
project(xdg-global-shortcuts)

set(project_SOURCES
    src/main.c)

add_library(${CMAKE_PROJECT_NAME} MODULE
    ${project_SOURCES})

find_package(libobs REQUIRED)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE OBS::libobs)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PREFIX "")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(LIB_OUT_DIR "/lib/obs-plugins")
    set(DATA_OUT_DIR "/share/obs/obs-plugins/${CMAKE_PROJECT_NAME}")
else()
    set(LIB_OUT_DIR "${CMAKE_PROJECT_NAME}/bin/64bit")
    set(DATA_OUT_DIR "${CMAKE_PROJECT_NAME}/data")
endif()


install(TARGETS ${CMAKE_PROJECT_NAME} LIBRARY DESTINATION ${LIB_OUT_DIR})
install(DIRECTORY data/locale DESTINATION ${DATA_OUT_DIR})
