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

project(wsf_mtt)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} LIBRARY
HDRS
   ${HDRS}
SRCS
   ${SRCS}
GRAMMARS
   grammar/wsf_mtt.ag
NOTICES
   NO_EXPORT.md
   README.md
)

target_link_libraries(${PROJECT_NAME} wsf_mil)
