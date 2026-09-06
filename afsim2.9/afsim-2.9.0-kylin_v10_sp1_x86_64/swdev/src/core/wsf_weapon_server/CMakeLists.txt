# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# ****************************************************************************
# Updated by Infoscitex, a DCS Company.
# ****************************************************************************
project(wsf_weapon_server)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} LIBRARY
HDRS
   source/WsfWeaponServer.hpp
SRCS
   source/WsfWeaponServer.cpp
GRAMMARS
   grammar/wsf_weapon_server.ag
NOTICES
   NO_EXPORT.md
   README.md
)

target_link_libraries(${PROJECT_NAME} wsf_mil)

# The weapon server does additional things in L16 is present
if (BUILD_WITH_wsf_l16)
   ADD_DEFINITIONS(-DWSF_WITH_L16)
   target_link_libraries(${PROJECT_NAME} wsf_l16)
ENDIF()
