# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(wsf_mil_parser)

wsf_project_template(${PROJECT_NAME} LIBRARY
STANDALONE
HDRS
   source/WsfPM_MilPlatform.hpp
   source/WsfPM_MilRoot.hpp
   source/WsfPM_RF_Jammer.hpp
   source/WsfPM_Weapon.hpp
SRCS
   source/WsfPM_MilPlatform.cpp
   source/WsfPM_MilRoot.cpp
   source/WsfPM_RF_Jammer.cpp
NOTICES
   README.md
)

target_link_libraries(${PROJECT_NAME} wsf_parser)
