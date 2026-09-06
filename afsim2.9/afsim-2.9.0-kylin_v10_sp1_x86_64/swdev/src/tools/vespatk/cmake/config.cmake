# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# VTK related configuration
# Configures CME location and optionally configures visual studio .user file

# Configure CME location
# Default to c:/cme or /local/cme
# unless CME_REL_BASE is set
set(CME_REL_BASE_DEFAULT_VALUE "C:/cme")
if(NOT WIN32)
   set(CME_REL_BASE_DEFAULT_VALUE "/local/cme")
endif()
if(NOT "$ENV{CME_REL_BASE}" STREQUAL "")
   set(CME_REL_BASE_DEFAULT_VALUE "$ENV{CME_REL_BASE}")
endif()
set(CME_REL_BASE "${CME_REL_BASE_DEFAULT_VALUE}" CACHE PATH "Location of a CME install")

# Configure CME version
set(CME_REL_SUFFIX_DEFAULT_VALUE ".v6.4.0")
if(NOT "$ENV{CME_REL_SUFFIX}" STREQUAL "")
   # ENV is set explicitly
   set(CME_REL_SUFFIX_DEFAULT_VALUE "$ENV{CME_REL_SUFFIX}")
elseif(EXISTS "${CME_REL_BASE_DEFAULT_VALUE}")
   # Attempt to find CME version by looking at CME's installation
   FILE(GLOB CME_BIN_DIRS "${CME_REL_BASE_DEFAULT_VALUE}/bin.v*")
   if(CME_BIN_DIRS)
      # Sort and grab greatest version
      list(SORT CME_BIN_DIRS)
      list(GET CME_BIN_DIRS "-1" CME_BIN_DIR)
      string(REGEX MATCH ".*(\\.v[0-9.]+)" CME_REL_SUFFIX_MATCH "${CME_BIN_DIR}")
      if(CME_REL_SUFFIX_MATCH)
         set(CME_REL_SUFFIX_DEFAULT_VALUE "${CMAKE_MATCH_1}")
      endif()
   endif()
endif()

set(CME_REL_SUFFIX "${CME_REL_SUFFIX_DEFAULT_VALUE}" CACHE STRING "Version suffix of a CME install")

get_filename_component(CME_SWDEV_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." REALPATH)

macro(vtk_write_vcproj_user
      ADDITIONAL_PATH_DEBUG
      ADDITIONAL_PATH_RELEASE
      ADDITIONAL_ENV_DEBUG
      ADDITIONAL_ENV_RELEASE
      )
if (WIN32)
   swdev_write_vcproj_user(
      "${CME_SWDEV_ROOT_DIR}/3rd_party-cmake/Template.VisualStudio.Settings.user"
      "${ADDITIONAL_PATH_DEBUG}"
      "${ADDITIONAL_PATH_RELEASE}"
      "CME_DATA_PATH=${CME_SWDEV_ROOT_DIR}/vespatk\nCME_REL_BASE_DEVELOPER=${CME_REL_BASE}\nCME_REL_SUFFIX_DEVELOPER=${CME_REL_SUFFIX}\n${ADDITIONAL_ENV_DEBUG}"
      "CME_DATA_PATH=${CME_SWDEV_ROOT_DIR}/vespatk\nCME_REL_BASE_DEVELOPER=${CME_REL_BASE}\nCME_REL_SUFFIX_DEVELOPER=${CME_REL_SUFFIX}\n${ADDITIONAL_ENV_RELEASE}"
    )
endif (WIN32)
endmacro()
