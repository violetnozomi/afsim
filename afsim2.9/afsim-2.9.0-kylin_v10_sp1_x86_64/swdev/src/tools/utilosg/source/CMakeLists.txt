# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(utilosg)
include(swdev_project)
include(GenerateExportHeader)

FILE(GLOB CME_SRCS CME/*.cpp CME/*.hpp CME/*.h)
FILE(GLOB CORE_SRCS Core/*.cpp Core/*.hpp Core/*.h)
FILE(GLOB ENTITY_SRCS Entity/*.cpp Entity/*.hpp Entity/*.h)
FILE(GLOB INCLUDE_SRCS include/*.h include/*.hpp)
FILE(GLOB RESOURCE_SRCS Resource/*.cpp Resource/*.hpp Resource/*.h)
FILE(GLOB SHAPE_SRCS Shape/*.cpp Shape/*.hpp Shape/*.h)
FILE(GLOB TERRAIN_SRCS Terrain/*.cpp Terrain/*.hpp Terrain/*.h)
FILE(GLOB UTILITY_SRCS Utility/*.cpp Utility/*.hpp Utility/*.h)
FILE(GLOB VIEWER_SRCS Viewer/*.cpp Viewer/*.hpp Viewer/*.h)

FILE(GLOB SRCS ../uto/*.hpp ../uto/*.h)

find_package(OpenGL)

source_group(CME FILES ${CME_SRCS})
source_group(Core FILES ${CORE_SRCS})
source_group(Entity FILES ${ENTITY_SRCS})
source_group(include FILES ${INCLUDE_SRCS})
source_group(Resource FILES ${RESOURCE_SRCS})
source_group(Shape FILES ${SHAPE_SRCS})
source_group(Terrain FILES ${TERRAIN_SRCS})
source_group(Utility FILES ${UTILITY_SRCS})
source_group(Viewer FILES ${VIEWER_SRCS})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ReaderWriterOGL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ReaderWriterMod)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/osgdb_osgearth_dted_tms)

add_definitions(-DDBG_HAS_DLL=0)

include_directories(.  include ../uto  Shape
                    ${OSGEARTHINC} ${OSGINC})
add_library(${PROJECT_NAME} ${CME_SRCS} ${CORE_SRCS} ${ENTITY_SRCS} ${INCLUDE_SRCS}
            ${RESOURCE_SRCS} ${SHAPE_SRCS} ${TERRAIN_SRCS} ${UTILITY_SRCS} ${VIEWER_SRCS} ${SRCS})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools")

generate_export_header(${PROJECT_NAME})
if(MSVC)
   target_compile_options(${PROJECT_NAME} PUBLIC -wd4251 -wd4286 -wd4275)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}=
      ${CMAKE_CURRENT_SOURCE_DIR}/CME
      ${CMAKE_CURRENT_SOURCE_DIR}/Core
      ${CMAKE_CURRENT_SOURCE_DIR}/Entity
      ${CMAKE_CURRENT_SOURCE_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/Resource
      ${CMAKE_CURRENT_SOURCE_DIR}/Shape
      ${CMAKE_CURRENT_SOURCE_DIR}/Terrain
      ${CMAKE_CURRENT_SOURCE_DIR}/Utility
      ${CMAKE_CURRENT_SOURCE_DIR}/Viewer
      ${CMAKE_CURRENT_SOURCE_DIR}/../uto
      ${PROJECT_BINARY_DIR}
)

#SWDEV_PACKAGE_LINK_LIBRARIES(${PROJECT_NAME} osg)

swdev_package_link_libraries(${PROJECT_NAME} osg osgEarth geos)

target_link_libraries(${PROJECT_NAME} util ${OPENGL_LIBRARIES})
IF(WIN32)
target_link_libraries(${PROJECT_NAME} shlwapi.lib)
ENDIF(WIN32)

FILE(GLOB UTO_SHADER_FILES ../data/shaders/*.h ../data/shaders/*.frag ../data/shaders/*.geom ../data/shaders/*.vert)
SET(UTO_SHADER_FILES "${UTO_SHADER_FILES}" PARENT_SCOPE)

swdev_lib_install(${PROJECT_NAME})
swdev_warning_level(${PROJECT_NAME})

install_sources_all_files(../ tools/${PROJECT_NAME})
