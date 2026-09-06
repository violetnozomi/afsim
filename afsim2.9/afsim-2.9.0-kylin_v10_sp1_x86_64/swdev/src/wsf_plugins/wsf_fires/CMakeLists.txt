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

project(wsf_fires)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} PLUGIN
HDRS
   ${HDRS}
SRCS
   ${SRCS}
GRAMMARS
   grammar/wsf_fires.ag
NOTICES
   NO_EXPORT.md
   README.md
)

target_link_libraries(${PROJECT_NAME} wsf_mil)

if(WSF_INSTALL_DEMOS)
   # Data directories ALWAYS included with mil build
   set(WSF_FIRES_DEMO_DIRS
       fires
      )
   install_wsf_data("${WSF_FIRES_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
