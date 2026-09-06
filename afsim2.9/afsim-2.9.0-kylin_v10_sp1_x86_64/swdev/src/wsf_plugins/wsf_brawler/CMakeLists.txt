# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(wsf_brawler)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} PLUGIN
SUB_PROJECTS
   brawler
HDRS
   ${HDRS}
SRCS
   ${SRCS}
GRAMMARS
   grammar/wsf_brawler.ag
NOTICES
   NO_EXPORT.md
   README.md
)

target_link_libraries(${PROJECT_NAME} wsf_mil)

# INSTALL

install_sources_all_files(conversion ${_project_install_dir})

if(WSF_INSTALL_DEMOS)
   # Demo directories included with the brawler build
   set(BRAWLER_DEMO_DIRS
       brawler
      )
   install_wsf_data("${BRAWLER_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
