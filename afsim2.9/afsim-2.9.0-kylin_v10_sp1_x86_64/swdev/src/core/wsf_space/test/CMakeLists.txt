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

FILE(GLOB SRCS *.cpp *.hpp)

if(GTest_FOUND)
   add_executable(space_test ${SRCS})
   swdev_warning_level(space_test)
   target_include_directories(space_test PUBLIC ${WSF_SPACE_INCLUDES})
   target_link_libraries(space_test
      ${WSF_LIBS}
      wsf_space
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
      GTest::Main
      GTest::GTest
   )

   add_test(NAME "space" COMMAND space_test)
   set_property(TARGET space_test PROPERTY FOLDER UnitTests)
endif()
