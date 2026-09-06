# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include(swdev_project)

add_subdirectory(source)

# *****************************************************************************
# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR})
add_wsf_doc_sidebar_ext(${CMAKE_CURRENT_SOURCE_DIR}/wsftheme/wsf_sidebar_mover_creator.html)

install(DIRECTORY data/ DESTINATION resources/data/mover_creator COMPONENT Resource)
