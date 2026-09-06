# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

file(GLOB SRCS *.cpp *.hpp)
if(GTest_FOUND)
   add_executable(six_dof_test ${SRCS})
   target_link_libraries(six_dof_test PUBLIC
      wsf_six_dof
      GTest::Main
      GTest::GTest
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
   )
   add_test(NAME "six_dof" COMMAND six_dof_test)
   set_property(TARGET six_dof_test PROPERTY FOLDER UnitTests)
endif()
