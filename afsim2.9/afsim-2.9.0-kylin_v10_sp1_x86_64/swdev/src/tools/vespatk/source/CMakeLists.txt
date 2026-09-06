# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(vespatk)
include(swdev_project)
include(GenerateExportHeader)


FILE(GLOB SRCS *.cpp *.hpp)

FILE(GLOB VTK_SHADER_FILES ../shaders/*.vert ../shaders/*.frag)
source_group(shaders FILES ${VTK_SHADER_FILES})
SET(VTK_SHADER_FILES "${VTK_SHADER_FILES}" PARENT_SCOPE)

# *****************************************************************************
# Acquire resource files
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/resources.cmake)
swdev_acquire_resources()

add_library(${PROJECT_NAME} ${SRCS} ${VTK_SHADER_FILES})

swdev_package_include_directories(${PROJECT_NAME} osg osgEarth gdal)

generate_export_header(${PROJECT_NAME})
if (MSVC)
   target_compile_options(${PROJECT_NAME} PUBLIC -wd4251 -wd4286 -wd4275)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools")
swdev_warning_level(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/../shaders
      ${PROJECT_BINARY_DIR}
)

swdev_package_link_libraries(${PROJECT_NAME} gdal geos)

target_link_libraries(${PROJECT_NAME} util geodata utilosg genio osgdb_osgearth_dted_tms)
if (NOT WIN32)
   find_package(PROJ)
   target_link_libraries(${PROJECT_NAME} PROJ::proj)
endif()

swdev_lib_install(${PROJECT_NAME})

install_sources_all_files(../ tools/${PROJECT_NAME})
install_source_files(../data/starcatalog tools/${PROJECT_NAME}/data)
