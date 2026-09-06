# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(argo8)
include (GenerateExportHeader)
include(swdev_project)

FILE(GLOB SRCS *.?pp *.h *.?xx)
FILE(GLOB MODEL_SRCS ../models/*.?pp ../models/*.h ../models/*.?xx)
source_group(Models FILES ${MODEL_SRCS})
set(SRCS ${SRCS} ${MODEL_SRCS})

set(SUBDIRS standard)
foreach(SUBDIR ${SUBDIRS})
   FILE(GLOB MODELINC ../models/${SUBDIR}/*.?pp ../models/${SUBDIR}/*.h)
   source_group(Models\\${SUBDIR} FILES ${MODELINC})
   set(SRCS ${SRCS} ${MODELINC}) #set(SRCS ${SRCS} ${MODELINC} ${MODELLIB})
endforeach(SUBDIR ${SUBDIRS})

include_directories(. ../models ${PROJECT_BINARY_DIR})
add_library(${PROJECT_NAME} STATIC ${SRCS})
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
   "${CMAKE_CURRENT_SOURCE_DIR}"
   "${PROJECT_BINARY_DIR}")
target_link_libraries(${PROJECT_NAME} util)
swdev_lib_install(${PROJECT_NAME})
swdev_warning_level(${PROJECT_NAME})

## find pre-built shared libraries
#if(UNIX)
#   FILE(GLOB ORIG_DEBUG_SHARED_LIBS   ../models/*/*.so) # find debug so files
#   FILE(GLOB ORIG_RELEASE_SHARED_LIBS ../models/*/*.so) # find release so files
#else()
#   FILE(GLOB ORIG_DEBUG_SHARED_LIBS   ../models/*/*.dll) # find debug dll files
#   FILE(GLOB ORIG_RELEASE_SHARED_LIBS ../models/*/*.dll) # find release dll files
#endif()
#
#if(WIN32)
#   install(FILES ${ORIG_DEBUG_SHARED_LIBS}
#           CONFIGURATIONS Debug
#           DESTINATION ${INSTALL_DLL_PATH})
#
#   install(FILES ${ORIG_RELEASE_SHARED_LIBS}
#           CONFIGURATIONS Release
#           DESTINATION ${INSTALL_DLL_PATH})
#else(WIN32)
#   install(FILES ${ORIG_DEBUG_SHARED_LIBS}
#           CONFIGURATIONS Debug
#           DESTINATION ${INSTALL_DLL_PATH}/libs)
#
#   install(FILES ${ORIG_RELEASE_SHARED_LIBS}
#           CONFIGURATIONS Release
#           DESTINATION ${INSTALL_DLL_PATH}/libs)
#endif(WIN32)
