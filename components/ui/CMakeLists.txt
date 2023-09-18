FILE(GLOB_RECURSE app_sources src/*.* include/*.*)

idf_component_register(SRCS ${app_sources})
