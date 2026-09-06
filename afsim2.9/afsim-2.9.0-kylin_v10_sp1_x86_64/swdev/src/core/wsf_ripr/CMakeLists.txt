# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(wsf_ripr)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} LIBRARY
HDRS
   ${HDRS}
SRCS
   ${SRCS}
GRAMMARS
   grammar/wsf_ripr.ag
NOTICES
   README.md
)

target_link_libraries(${PROJECT_NAME} wsf_mil)

#if(WSF_INSTALL_DEMOS)
#   # Demo directories ALWAYS included with ripr build
#   set(WSF_DEMO_RIPR_DIRS
#      )
#   install_wsf_data("${WSF_DATA_RIPR_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
#endif()
