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
file(GLOB SRCS *.cpp *.hpp)
if(GTest_FOUND)
   add_executable(util_test ${SRCS})
   target_link_libraries(util_test PUBLIC
      util
      GTest::GTest
      GTest::Main
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
   )
   add_test(NAME "util" COMMAND util_test "${CMAKE_CURRENT_SOURCE_DIR}/data")
   set_property(TARGET util_test PROPERTY FOLDER UnitTests)
   # the VS_DEBUGGER_COMMAND_ARGUMENTS property makes the tests executable from VS and requires cmake 3.13, this will have no effect otherwise
   set_property(TARGET util_test PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/data")
endif()
