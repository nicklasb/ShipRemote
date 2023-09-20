FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.* "main.c" )

idf_component_register(
    SRCS ${app_sources}
    REQUIRES inputs communication robusto-base
)
