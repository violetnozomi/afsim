# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2003-2015 The Boeing Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
add_subdirectory(../../source ./wsf_parser)
add_subdirectory(../../../util/source ./util)
include_directories(../../source ../../../util/source)
add_executable(proxy_test proxy_test.cpp)

target_link_libraries(proxy_test wsf_parser util)

