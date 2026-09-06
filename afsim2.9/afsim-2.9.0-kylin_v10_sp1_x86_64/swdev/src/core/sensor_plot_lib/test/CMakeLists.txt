# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

FILE(GLOB_RECURSE SRCS *.cpp *.hpp)

if(GTest_FOUND)
   add_executable(sensor_plot_lib_test ${SRCS})
   target_link_libraries(sensor_plot_lib_test
                         ${WSF_LIBS}
                         wsf_mil
                         sensor_plot_lib
                         ${SWDEV_THREAD_LIB}
                         ${SWDEV_DL_LIB}
                         GTest::Main
                         GTest::GTest
                        )
   add_test(NAME "sensor_plot_lib" COMMAND sensor_plot_lib_test "${CMAKE_CURRENT_SOURCE_DIR}/resources")
   link_wsf_optional_projects(sensor_plot_lib_test)
   set_property(TARGET sensor_plot_lib_test PROPERTY FOLDER UnitTests)
   # the VS_DEBUGGER_COMMAND_ARGUMENTS property makes the tests executable from VS and requires cmake 3.13, this will have no effect otherwise
   set_property(TARGET sensor_plot_lib_test PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
endif()
