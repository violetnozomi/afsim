# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# ****************************************************************************
# Updated by Infoscitex, a DCS Company.
# ****************************************************************************
project(wsf_util)

# Add source directories to doxygen input
#add_wsf_doxygen_input(${SRCS}) TODO Use this instead? Adds all source files
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SRCS *.?pp)

add_library(${PROJECT_NAME} ${SRCS})
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR})

swdev_warning_level(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} util ${SWDEV_THREAD_LIB} ${SWDEV_DL_LIB})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Set the subfolder that the target will appear under in an IDE's solution explorer.
# This isn't actually related to any file system folders.
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "core")

swdev_lib_install(${PROJECT_NAME})
install_sources_all_files(../ core/wsf_util)
