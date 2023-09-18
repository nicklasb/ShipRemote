FILE(GLOB_RECURSE comm_sources 
"include/*.h"
"src/*.c" 
)

idf_component_register(
    SRCS ${comm_sources}
    INCLUDE_DIRS . include 
)

