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

project(wsf_sosm)

wsf_project_template(${PROJECT_NAME} PLUGIN
SUB_PROJECTS
   sosm
HDRS
   source/WsfSOSM_Interaction.hpp
   source/WsfSOSM_Interface.hpp
   source/WsfSOSM_Sensor.hpp
SRCS
   source/WsfSOSM_Interaction.cpp
   source/WsfSOSM_Interface.cpp
   source/WsfSOSM_Sensor.cpp
GRAMMARS
   grammar/wsf_sosm.ag
NOTICES
   NO_EXPORT.md
   README.md
)

if(WSF_INSTALL_DEMOS)
   # Demo directories included with the sosm build
   set(WSF_DEMOS_SOSM
       sosm
      )
   install_wsf_data("${WSF_DEMOS_SOSM}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
