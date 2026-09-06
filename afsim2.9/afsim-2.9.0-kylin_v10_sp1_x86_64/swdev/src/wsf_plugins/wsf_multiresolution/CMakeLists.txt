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

project(wsf_multiresolution)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} PLUGIN
HDRS
   ${HDRS}
SRCS
   ${SRCS}
GRAMMARS
   grammar/wsf_multiresolution.ag
   grammar/wsf_multirun_table.ag
NOTICES
   README.md
)

# wsf_mil needed for signatures
target_link_libraries(${PROJECT_NAME} wsf_mil)

# Tests
#add_subdirectory(test)
install_sources(test ${_project_install_dir})

# Documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/doc)
add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source)
if(WSF_TOOLS_ROOT AND IS_DIRECTORY "${WSF_TOOLS_ROOT}/pyrunplotter")
   add_wsf_python_module_docs(${WSF_TOOLS_ROOT}/pyrunplotter)
endif()

if(WSF_INSTALL_DEMOS)
   # Demo directories included with the multiresolution build
   install_wsf_data("multiresolution_demos" ${WSF_DEMOS_ROOT} demos Demo)
endif()
