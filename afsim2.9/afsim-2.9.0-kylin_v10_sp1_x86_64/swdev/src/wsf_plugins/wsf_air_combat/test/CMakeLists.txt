# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

FILE(GLOB SRCS *.cpp *.hpp)

if(GTest_FOUND)
   add_executable(air_combat_test ${SRCS})
   target_link_libraries(air_combat_test
      ${WSF_LIBS}
      wsf_air_combat
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
      GTest::Main
      GTest::GTest
   )

   add_test(NAME "air_combat" COMMAND air_combat_test)
   set_property(TARGET air_combat_test PROPERTY FOLDER UnitTests)
endif()
