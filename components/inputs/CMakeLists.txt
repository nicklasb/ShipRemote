FILE(GLOB_RECURSE input_sources 
"include/*.h"
"src/*.c" 
)

idf_component_register(
    SRCS ${input_sources}
    INCLUDE_DIRS . include 
    REQUIRES esp32-button robusto-base
)
