# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
file(GLOB_RECURSE SRCS *.cpp *.hpp)
if(GTest_FOUND)
   add_executable(core_test ${SRCS})
   swdev_warning_level(core_test)
   target_link_libraries(core_test PUBLIC
      wsf
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
      GTest::Main
      GTest::GTest
   )
   add_test(NAME "core" COMMAND core_test)
   set_property(TARGET core_test PROPERTY FOLDER UnitTests)
endif()
