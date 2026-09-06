# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(osgdb_ogl)
include(swdev_project)

FILE(GLOB SRCS *.?pp)

include_directories(. ${OSGINC})
add_library(${PROJECT_NAME} ${SRCS})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
swdev_package_link_libraries(${PROJECT_NAME} osg)
target_link_libraries(${PROJECT_NAME})
swdev_lib_install(${PROJECT_NAME})
swdev_warning_level(${PROJECT_NAME})
