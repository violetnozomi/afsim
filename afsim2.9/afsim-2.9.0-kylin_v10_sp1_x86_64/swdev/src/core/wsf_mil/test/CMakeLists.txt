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
   add_executable(mil_test ${SRCS})
   target_include_directories(mil_test PUBLIC ${WSF_MIL_INCLUDES})
   target_link_libraries(mil_test
      ${WSF_LIBS}
      wsf_mil
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
      GTest::Main
      GTest::GTest
   )
   add_test(NAME "mil" COMMAND mil_test "${CMAKE_CURRENT_SOURCE_DIR}/resources")
   set_property(TARGET mil_test PROPERTY FOLDER UnitTests)
   # the VS_DEBUGGER_COMMAND_ARGUMENTS property makes the tests executable from VS and requires cmake 3.13, this will have no effect otherwise
   set_property(TARGET mil_test PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
endif()
