
set(COMPONENT_REQUIRES
    graphics)

file(GLOB_RECURSE SRCS
    RELATIVE ${COMPONENT_PATH}
    src/*.c)
set(COMPONENT_SRCS ${SRCS})

set(COMPONENT_ADD_INCLUDEDIRS src)

register_component()
