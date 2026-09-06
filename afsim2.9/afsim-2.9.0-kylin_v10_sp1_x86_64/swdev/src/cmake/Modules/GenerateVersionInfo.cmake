# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

set(VERSION_TEMPLATE_DIR "${CMAKE_CURRENT_LIST_DIR}/version")

# Generate a version header file for a target
# generate_version_header(<target>)

# Header will contain preprocessor definitions with version information that may be included
# by an application's resource file
macro(generate_version_header TARGET)

   if(NOT DEFINED AFSIM_VERSION_MAJOR)
      message(FATAL_ERROR "AFSIM_VERSION_MAJOR, AFSIM_VERSION_MINOR, AFSIM_VERSION_PATCH, AFSIM_VERSION and AFSIM_RELEASE_VERSION must be defined")
   endif()

   # Use upper case of target name as prefix for all preprocessor definitions
   string(TOUPPER ${TARGET} VERSION_PREFIX)

   # Default all definitions to the corresponding AFSIM variable
   set(${VERSION_PREFIX}_VERSION_MAJOR ${AFSIM_VERSION_MAJOR})
   set(${VERSION_PREFIX}_VERSION_MINOR ${AFSIM_VERSION_MINOR})
   set(${VERSION_PREFIX}_VERSION_PATCH ${AFSIM_VERSION_PATCH})
   set(${VERSION_PREFIX}_VERSION ${AFSIM_VERSION})
   set(${VERSION_PREFIX}_RELEASE_VERSION ${AFSIM_RELEASE_VERSION})

   set(${VERSION_PREFIX}_FILEVERSION "${VERSION_PREFIX}_VERSION_MAJOR,${VERSION_PREFIX}_VERSION_MINOR,${VERSION_PREFIX}_VERSION_PATCH")
   set(${VERSION_PREFIX}_FILEVERSION_STR "${VERSION_PREFIX}_VERSION")
   set(${VERSION_PREFIX}_PRODUCTVERSION "${VERSION_PREFIX}_FILEVERSION")
   set(${VERSION_PREFIX}_PRODUCTVERSION_STR "${VERSION_PREFIX}_RELEASE_VERSION")

   set(${VERSION_PREFIX}_COMPANYNAME_STR "\"${AFSIM_COMPANYNAME}\"")
   set(${VERSION_PREFIX}_LEGALCOPYRIGHT_STR "\"${AFSIM_LEGALCOPYRIGHT}\"")
   set(${VERSION_PREFIX}_LEGALTRADEMARKS1_STR "\"${AFSIM_LEGALTRADEMARKS}\"")
   set(${VERSION_PREFIX}_LEGALTRADEMARKS2_STR "${VERSION_PREFIX}_LEGALTRADEMARKS1_STR")
   set(${VERSION_PREFIX}_COMPANYDOMAIN_STR "\"${AFSIM_COMPANYDOMAIN}\"")

   # If the target has its own version_info.cmake, include it and possibly override any
   # variables defined above
   set(VERSION_INFO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/version_info.cmake")
   if (EXISTS ${VERSION_INFO_FILE})
      include(${VERSION_INFO_FILE})
   else()
      set(VERSION_INFO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/source/version_info.cmake")
      if (EXISTS ${VERSION_INFO_FILE})
         include(${VERSION_INFO_FILE})
      endif()
   endif()

   # Configure the version header
   set(VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/${TARGET}_version_defines.hpp")
   configure_file("${VERSION_TEMPLATE_DIR}/version_defines.hpp.in" ${VERSION_HEADER})

   # Add generated version header to the sources and include directories of the target
   target_sources(${TARGET} PRIVATE ${VERSION_HEADER})
   target_include_directories(${TARGET} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include")

endmacro()

# Generate a MSVC resource (.rc) file for a target
# generate_resource(<target> [ICON_PATH])
#
# Can only be used if generate_version_header is called first to generate the version header,
# which will be included by the resource file.
# Optional argument ICON_PATH specifies the path to the target's ICON resource
macro(generate_resource TARGET)

   # Set variables for substitution in input file that is configured
   if (${ARGC} GREATER 1)
      set(ICON_RESOURCE ${ARGV1}) # Optional argument for icon
   endif()
   set(VERSION_HEADER_PREFIX ${TARGET})
   string(TOUPPER ${TARGET} VERSION_PREFIX)

   # Configure the resource file and add it to the sources of target
   set(RESOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.rc")
   configure_file("${VERSION_TEMPLATE_DIR}/version_info.rc.in" ${RESOURCE_FILE})
   target_sources(${TARGET} PRIVATE ${RESOURCE_FILE})

endmacro()
