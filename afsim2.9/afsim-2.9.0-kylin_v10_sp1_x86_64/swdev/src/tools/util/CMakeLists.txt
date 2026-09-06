# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# ****************************************************************************
# Updated by DCS Corporation.
# ****************************************************************************

project(util)
include(swdev_project)

include_directories(source)
add_subdirectory(source)
#add_subdirectory(test)

install_sources_all_files(. tools/${PROJECT_NAME})
