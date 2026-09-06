# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# Configure the unit testing framework with gtest, this will enable unit tests.

include(swdev_project)

# *****************************************************************************
# Configure the 3rd_party libraries
include(${TOOLS_DIRECTORY}/3rd_party-cmake/config.cmake)
set(SWDEV_GTEST_PACKAGE "gtest-1.8.0" CACHE STRING "" FORCE)
mark_as_advanced(FORCE SWDEV_GTEST_PACKAGE)
swdev_acquire_packages(${CMAKE_CURRENT_SOURCE_DIR}
                       ${SWDEV_GTEST_PACKAGE})

find_package(GTest)
if(GTEST_FOUND)
   enable_testing()
   include_directories(${GTEST_INCLUDE_DIR})
   install_source_files(${TOOLS_DIRECTORY}/3rd_party-cmake/config.cmake
                        ${TOOLS_DIRECTORY}/3rd_party-cmake/shared.cmake
                        ${TOOLS_DIRECTORY}/3rd_party-cmake/${SWDEV_GTEST_PACKAGE}.cmake
                        tools/3rd_party-cmake)
endif()
