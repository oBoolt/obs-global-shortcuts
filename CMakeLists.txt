cmake_minimum_required(VERSION 4.4.2)
project(obs-global-shortcuts)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(project_SOURCES
    src/main.c
    src/portal.c
    src/shortcuts.c
    src/binds.c)

add_library(${CMAKE_PROJECT_NAME} MODULE
    ${project_SOURCES})

find_package(libobs REQUIRED)
find_package(obs-frontend-api REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GIO REQUIRED IMPORTED_TARGET gio-2.0)

target_link_libraries(${CMAKE_PROJECT_NAME}
    OBS::libobs
    OBS::obs-frontend-api
    PkgConfig::GIO)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PREFIX "")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(LIB_OUT_DIR "/lib/obs-plugins")
    set(DATA_OUT_DIR "/usr/share/obs/obs-plugins/${CMAKE_PROJECT_NAME}")
else()
    set(LIB_OUT_DIR "${CMAKE_PROJECT_NAME}/bin/64bit")
    set(DATA_OUT_DIR "${CMAKE_PROJECT_NAME}/data")
endif()


install(TARGETS ${CMAKE_PROJECT_NAME} LIBRARY DESTINATION ${LIB_OUT_DIR})
install(DIRECTORY data/locale DESTINATION ${DATA_OUT_DIR})
