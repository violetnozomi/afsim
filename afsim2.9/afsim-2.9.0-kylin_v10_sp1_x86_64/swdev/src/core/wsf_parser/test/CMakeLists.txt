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

FILE(GLOB SRCS *.cpp *.hpp)

# Attempt to find the FindGMock module and include it
find_package(GMock REQUIRED)

# Only create the parser_test project if the GTest and GMock libraries exist
if(GTest_FOUND AND GMock_FOUND)
   add_executable(parser_test ${SRCS})
   target_include_directories(parser_test PUBLIC ${WSF_PARSER_INCLUDES})
   target_link_libraries(parser_test
      ${WSF_LIBS}
      wsf_parser
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
      GTest::Main
      GTest::GTest
      GMock::Main
      GMock::GMock
   )

   add_test(NAME "parser" COMMAND parser_test "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
   set_property(TARGET parser_test PROPERTY FOLDER UnitTests)
   # the VS_DEBUGGER_COMMAND_ARGUMENTS property makes the tests executable from VS and requires cmake 3.13, this will have no effect otherwise
   set_property(TARGET parser_test PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
endif()
