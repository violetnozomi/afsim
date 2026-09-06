# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
include(../../misc/swdev_project.cmake)
try_add_subdirectory(../source wsf_parser)
try_add_subdirectory(../../util/source util)
add_subdirectory(proxy_test)

