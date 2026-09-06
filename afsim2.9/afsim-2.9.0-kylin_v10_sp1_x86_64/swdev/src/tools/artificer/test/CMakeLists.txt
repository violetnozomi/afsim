# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

file(GLOB SRCS *.cpp *.hpp)
if(GTest_FOUND)
   add_executable(artificer_test ${SRCS})
   target_link_libraries(artificer_test PUBLIC
      artificer
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
      GTest::Main
      GTest::GTest
   )
   add_test(NAME "artificer" COMMAND artificer_test)
   set_property(TARGET artificer_test PROPERTY FOLDER UnitTests)
endif()
