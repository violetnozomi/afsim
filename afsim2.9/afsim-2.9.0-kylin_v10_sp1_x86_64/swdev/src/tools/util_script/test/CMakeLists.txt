# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
file(GLOB SRCS *.cpp *.hpp)
if(GTest_FOUND)
   add_executable(util_script_test ${SRCS})
   target_link_libraries(util_script_test PUBLIC
      util_script
      GTest::GTest
      GTest::Main
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
   )
   add_test(NAME "util_script" COMMAND util_script_test)
   set_property(TARGET util_script_test PROPERTY FOLDER UnitTests)
endif()
