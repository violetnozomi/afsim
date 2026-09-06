# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(wsf_l16)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} LIBRARY
HDRS
   ${HDRS}
SRCS
   ${SRCS}
GRAMMARS
   grammar/wsf_l16.ag
NOTICES
   NO_EXPORT.md
   README.md
)

target_link_libraries(${PROJECT_NAME} wsf_mil wsf_nx)

if(WSF_INSTALL_DEMOS)
   # Data directories ALWAYS included with l16 build
   set(WSF_L16_DEMO_DIRS
       l16_j11
      )
   install_wsf_data("${WSF_L16_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
