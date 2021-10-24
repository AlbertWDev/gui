set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_SRCDIRS ".")

#################################################
###    ADD ALL VIEW DIRECTORIES AS SOURCES    ###
#################################################
FILE(GLOB_RECURSE UI_VIEW_DIRS
    LIST_DIRECTORIES true
    RELATIVE ${COMPONENT_PATH}
    ${COMPONENT_PATH}/ui/*)
FOREACH(UI_VIEW_DIR ${UI_VIEW_DIRS})
    # Check if this directory contains SRC files
    FILE(GLOB src_files "${COMPONENT_PATH}/${UI_VIEW_DIR}/*.c")
    IF(IS_DIRECTORY "${COMPONENT_PATH}/${UI_VIEW_DIR}" AND src_files)
        #message(STATUS "UI view: ${UI_VIEW_DIR}")
        LIST(APPEND COMPONENT_SRCDIRS ${UI_VIEW_DIR})
    ENDIF()
ENDFOREACH()

register_component()

spiffs_create_partition_image(data ./data FLASH_IN_PROJECT)
#spiffs_create_partition_image(data ./data)
