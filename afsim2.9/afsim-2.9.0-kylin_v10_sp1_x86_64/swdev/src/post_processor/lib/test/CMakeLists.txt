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
   add_executable(post_processor_test ${SRCS})
   target_link_libraries(post_processor_test
      GTest::Main
      GTest::GTest
      post_processor_lib
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
   )
   add_test(NAME "postprocessor" COMMAND post_processor_test "${CMAKE_CURRENT_SOURCE_DIR}/resources")
   set_property(TARGET post_processor_test PROPERTY FOLDER UnitTests)
endif()
