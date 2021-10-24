cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS
    "../common_components/display_driver"
    "../common_components/graphics"
    "../..")
project(gui-example_basic)
