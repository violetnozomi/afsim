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

project(profiling)
include(swdev_project)

add_subdirectory(afperf)
add_subdirectory(source)
#add_subdirectory(test)

# Add documentation content for Sphinx
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/doc)

# Add source directories to doxygen input
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/common)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source)

install_sources_all_files(. tools/${PROJECT_NAME})
