FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.* "main.c" )

idf_component_register(
    SRCS ${app_sources}
    INCLUDE_DIRS . ../include
    REQUIRES esp32-button robusto-base robusto-network robusto-ui
)