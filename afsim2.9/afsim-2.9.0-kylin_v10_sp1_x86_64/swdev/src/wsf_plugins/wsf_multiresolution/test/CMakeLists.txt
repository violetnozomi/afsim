# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright (C) 2021 Stellar Science; U.S. Government has Unlimited Rights.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

file(GLOB SRCS *.cpp *.hpp)
if(GTest_FOUND)
   add_executable(multiresolution_test ${SRCS})
   target_link_libraries(multiresolution_test PUBLIC
      wsf_multiresolution
      GTest::Main
      GTest::GTest
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
   )
   add_test(NAME "multiresolution" COMMAND multiresolution_test)
   set_property(TARGET multiresolution_test PROPERTY FOLDER UnitTests)
endif()
