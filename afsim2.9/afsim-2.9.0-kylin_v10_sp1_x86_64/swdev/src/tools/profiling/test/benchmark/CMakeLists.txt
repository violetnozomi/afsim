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

file(GLOB SRCS test_*.cpp test_*.hpp)
# This is a benchmark, not a test, so we don't use add_test.
add_executable(profiling_benchmarks profiling_benchmarks.cpp )
target_link_libraries(profiling_benchmarks
   profiling
   ${SWDEV_THREAD_LIB}
   ${SWDEV_DL_LIB}
)
set_property(TARGET profiling_benchmarks PROPERTY FOLDER UnitTests/profiling)