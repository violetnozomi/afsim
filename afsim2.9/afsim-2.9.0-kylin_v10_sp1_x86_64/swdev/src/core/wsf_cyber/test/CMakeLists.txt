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
if(GTest_FOUND)
   add_executable(cyber_test ${SRCS})
   target_include_directories(cyber_test PUBLIC ${WSF_CYBER_INCLUDES})
   target_link_libraries(cyber_test
      ${WSF_LIBS}
      wsf_cyber
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
      GTest::Main
      GTest::GTest
   )
   add_test(NAME "cyber" COMMAND cyber_test)
   set_property(TARGET cyber_test PROPERTY FOLDER UnitTests)
endif()
