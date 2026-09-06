# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

FILE(GLOB SRCS *.cpp *.hpp)

if(GTest_FOUND)
   add_executable(coverage_test ${SRCS})
   swdev_warning_level(coverage_test)
   target_include_directories(coverage_test PUBLIC
      ${WSF_COVERAGE_INCLUDES}
      ${CMAKE_CURRENT_BINARY_DIR}
   )
   target_link_libraries(coverage_test
      ${WSF_LIBS}
      wsf_coverage
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
      ${GTEST_BOTH_LIBRARIES}
   )

   add_test(NAME "coverage" COMMAND coverage_test)
   set_property(TARGET coverage_test PROPERTY FOLDER UnitTests)
endif()
