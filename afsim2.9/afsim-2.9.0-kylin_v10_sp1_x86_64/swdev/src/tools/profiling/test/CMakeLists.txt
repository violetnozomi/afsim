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

add_subdirectory(benchmark)

if(GTest_FOUND)
   add_subdirectory(dummy_library)
   add_subdirectory(version_greater_library)
   add_subdirectory(version_lesser_library)

   file(GLOB SRCS test_*.cpp test_*.hpp)
   add_executable(profiling_test
      ${SRCS}
      "${CMAKE_CURRENT_LIST_DIR}/../impl/ProfilingCsvUtil.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/../impl/ProfilingTimeUtil.cpp"
      # impl headers included in order for headers to be present in IDE folders.
      "${CMAKE_CURRENT_LIST_DIR}/../impl/ProfilingCsvUtil.hpp"
      "${CMAKE_CURRENT_LIST_DIR}/../impl/ProfilingTimeUtil.hpp"
   )
   target_link_libraries(profiling_test PUBLIC
      profiling
      ${SWDEV_THREAD_LIB}
      ${SWDEV_DL_LIB}
      GTest::Main
      GTest::GTest
   )
   target_include_directories(profiling_test PRIVATE
      "${CMAKE_CURRENT_LIST_DIR}/../common"
      "${CMAKE_CURRENT_LIST_DIR}/../impl"
   )
   add_dependencies(profiling_test
      profiling_test_dummy_library
      profiling_test_version_greater_library
      profiling_test_version_lesser_library
   )
   add_test(NAME "profiling" COMMAND profiling_test)
   set_property(TARGET profiling_test PROPERTY FOLDER UnitTests/profiling)
endif()
