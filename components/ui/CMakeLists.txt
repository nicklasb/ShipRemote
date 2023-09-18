FILE(GLOB_RECURSE ui_sources src/*.* include/*.*)

idf_component_register(
    SRCS ${ui_sources}
    INCLUDE_DIRS . include 
)

