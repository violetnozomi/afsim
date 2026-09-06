# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include(swdev_project)

# If the CMAKE_INSTALL_PREFIX is the default value
# and has not been specified by a higher level CMake file,
# set a default.
IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT  AND NOT (CMAKE_INSTALL_PREFIX MATCHES "wsf_install?"))
   SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/post_processor_install CACHE PATH "Installation path" FORCE)
ENDIF()

file(GLOB SRCS *.?pp)

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} post_processor_lib util ${SWDEV_THREAD_LIB} ${SWDEV_DL_LIB})
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/lib")

# *****************************************************************************
# Configure version header and resource files
include(GenerateVersionInfo)
generate_version_header(${PROJECT_NAME})
if(MSVC)
   generate_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/post_processor.ico")
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_EXE_PATH} COMPONENT Runtime)
