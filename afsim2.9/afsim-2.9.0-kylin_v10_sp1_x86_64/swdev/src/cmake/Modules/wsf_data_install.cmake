# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

#
# WSF CMake Configuration
#
# This file is included by the top-level CMake file. It defines
# utility functions and directories to be installed as part of a data
# package for release.

include(wsf_data_dirs)

# CMPTYPE = [ Demo | Scenario | Training | Tool ]
macro(install_wsf_data WSF_DATA_DIRS SRC_DIR TARGET_DIR CMPTYPE)

   # Parse optional arguments to macro
   set(oneValueArgs EXCLUDE_PATTERN)
   cmake_parse_arguments(DATA "" "${oneValueArgs}" "" ${ARGN})

   foreach(DIR ${WSF_DATA_DIRS})
     if(EXISTS "${SRC_DIR}/${DIR}")
        install(DIRECTORY ${SRC_DIR}/${DIR} DESTINATION ${TARGET_DIR} COMPONENT ${CMPTYPE}
                PATTERN ".*"     EXCLUDE
                PATTERN ".git"   EXCLUDE
                PATTERN "*.lis"  EXCLUDE
                PATTERN "*.aer"  EXCLUDE
                PATTERN "*.evt"  EXCLUDE
                PATTERN "*.rep"  EXCLUDE
                PATTERN "*.log"  EXCLUDE
                PATTERN "*.db"   EXCLUDE
                PATTERN "*.tmp"  EXCLUDE
                PATTERN "*.plt"  EXCLUDE
                PATTERN "*.sed"  EXCLUDE
                PATTERN ".*.swo" EXCLUDE
                PATTERN ".*.swp" EXCLUDE
                PATTERN "__pycache__" EXCLUDE
                PATTERN "${DATA_EXCLUDE_PATTERN}" EXCLUDE
               )
        if(COMMAND add_wsf_doc_input AND EXISTS "${SRC_DIR}/${DIR}/doc")
           add_wsf_doc_input(${SRC_DIR}/${DIR})
        endif()
     else()
        message("Skipping missing data directory: ${SRC_DIR}/${DIR}")
     endif()
   endforeach()
endmacro()

#if(WSF_INSTALL_DEMOS)
#   # Demo directories ALWAYS included with WSF
#   set(WSF_DEMOS_DIRS
#       bin
#      )
#   install_wsf_data("${WSF_DEMOS_DIRS}" ${WSF_DEMOS_ROOT} .)
#endif()

if(WSF_INSTALL_TOOLS)
   install_wsf_data(tools ${WSF_TOOLS_ROOT}/.. . Tool)
endif()
