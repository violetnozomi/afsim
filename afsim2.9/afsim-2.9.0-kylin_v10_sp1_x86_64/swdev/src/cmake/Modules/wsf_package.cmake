# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# CPack

include(swdev_project)

if (MSVC AND WSF_INSTALL_DEPENDENCIES)
   swdev_install_c_runtime()
   set(CPACK_COMPONENT_MSVC_LIBS_DISPLAY_NAME "MSVC Runtimes")
endif()

# Environment
if(WIN32 AND NOT UNIX)
   if(NOT "$ENV{WIX}" STREQUAL "")
      set(CPACK_WIX_COMPONENT_INSTALL ON)
   endif()
   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(CPACK_SYSTEM_NAME win64)
   else()
      set(CPACK_SYSTEM_NAME win32)
   endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^ppc")
   set(CPACK_SYSTEM_NAME lnx${CMAKE_SYSTEM_PROCESSOR}-gcc${GCC_MAJOR})
else()
   if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(CPACK_SYSTEM_NAME lnx64-gcc${GCC_MAJOR})
   else()
      set(CPACK_SYSTEM_NAME lnx32-gcc${GCC_MAJOR})
   endif()
endif()

# Basic Information
set(CPACK_PACKAGE_NAME "AFSIM")
if(AFSIM_CUSTOM_BUILD_VARIANT)
   set(CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-${AFSIM_CUSTOM_BUILD_VARIANT}")
endif(AFSIM_CUSTOM_BUILD_VARIANT)
set(CPACK_PACKAGE_VENDOR "${AFSIM_COMPANYNAME}")
set(CPACK_PACKAGE_CONTACT "USAF <afsim-info@vdl.afrl.af.mil>")
if (WIN32 AND NOT UNIX)
    # There is a bug in NSIS that does not handle full UNIX paths properly.
    # Make sure there is at least one set of four backlashes.
    set(CPACK_PACKAGE_ICON "${afsim_SOURCE_DIR}/cmake/logos\\\\AFSIM_icon-128x128.png")
else()
    set(CPACK_PACKAGE_ICON "${afsim_SOURCE_DIR}/cmake/logos/AFSIM_icon-128x128.png")
endif()
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AFSIM Project including all applications, demos and scenarios")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/cpack_license.txt")
set(CPACK_PACKAGE_CHECKSUM "SHA512")
set(CPACK_PACKAGE_VERSION_MAJOR "${AFSIM_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${AFSIM_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${AFSIM_VERSION_PATCH}")

# By default, CPACK_PACKAGE_VERSION is built from CPACK_PACKAGE_VERSION_MAJOR, CPACK_PACKAGE_VERSION_MINOR, and CPACK_PACKAGE_VERSION_PATCH.
# Override default CPACK_PACKAGE_FILE_NAME to use AFSIM_VERSION (which may contain non-numeric characters, e.g. 'rc') instead of CPACK_PACKAGE_VERSION.
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${AFSIM_VERSION}-${CPACK_SYSTEM_NAME}")

if(WIN32)
   set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}-${AFSIM_VERSION}")
else()
   string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME_LOWER)
   set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME_LOWER}-${AFSIM_VERSION}")
endif()
if(NOT WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
   set(CPACK_STRIP_FILES TRUE)
endif(NOT WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
#set(CPACK_SOURCE_STRIP_FILES "")

# Set the start menu items
get_property(CPACK_PACKAGE_EXECUTABLES GLOBAL PROPERTY START_MENU_EXECUTABLES)
get_property(CPACK_CREATE_DESKTOP_LINKS GLOBAL PROPERTY DESKTOP_LINKS)

# Set the options file that needs to be included inside CPackProjectConfig.cmake
configure_file("${CMAKE_CURRENT_LIST_DIR}/CPackProjectConfig.cmake.in"
               "${CMAKE_BINARY_DIR}/CPackProjectConfig.cmake" @ONLY)
set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_BINARY_DIR}/CPackProjectConfig.cmake")

include(CPack)

# Component Install
cpack_add_install_type(Full      DISPLAY_NAME "Full")
cpack_add_install_type(USER      DISPLAY_NAME "End User")
cpack_add_install_type(DEVELOPER DISPLAY_NAME "Developer")
cpack_add_install_type(DATA      DISPLAY_NAME "Data Only")

#   Applications and Documentation
cpack_add_component_group(APPLICATIONS
                          DISPLAY_NAME "Application"
                          DESCRIPTION "Applications and documentation for end users"
                          EXPANDED)
cpack_add_component(Runtime
                    DISPLAY_NAME "Applications and Plugins"
                    DESCRIPTION "Applications, Plugins and supporting application data"
                    GROUP APPLICATIONS
                    DEPENDS Resource
                    INSTALL_TYPES FULL USER)

cpack_add_component(Resource
                    DISPLAY_NAME "Resources"
                    DESCRIPTION "Applications resources"
                    GROUP APPLICATIONS
                    INSTALL_TYPES FULL USER)

if (WSF_INSTALL_DOCUMENTATION)
   cpack_add_component(Documentation
                       DISPLAY_NAME "Documentation"
                       DESCRIPTION "End user documentation"
                       GROUP APPLICATIONS
                       INSTALL_TYPES FULL USER)
endif()

#   Data
cpack_add_component_group(DATA
                          DISPLAY_NAME "Data"
                          DESCRIPTION "All of the demos, scenarios, tools, etc."
                          EXPANDED)

if (WSF_INSTALL_DEMOS)
   cpack_add_component(Demo
                       DISPLAY_NAME "Demos"
                       DESCRIPTION "Demonstration of application capabilities"
                       GROUP DATA
                       INSTALL_TYPES FULL DATA USER)
endif()

if (WSF_INSTALL_SCENARIOS)
   cpack_add_component(Scenario
                       DISPLAY_NAME "Scenarios"
                       DESCRIPTION "Scenarios and Box Sets"
                       GROUP DATA
                       INSTALL_TYPES FULL DATA USER)
endif()

if (WSF_INSTALL_TOOLS)
   cpack_add_component(Tool
                       DISPLAY_NAME "Tools"
                       DESCRIPTION "User tools and scripts"
                       GROUP DATA
                       INSTALL_TYPES FULL DATA USER)
endif()

if (WSF_INSTALL_TRAINING)
   cpack_add_component(Training
                       DISPLAY_NAME "Training"
                       DESCRIPTION "User and Developer Training"
                       GROUP DATA
                       INSTALL_TYPES FULL DATA USER)
endif()

#   Development
cpack_add_component_group(DEVELOPMENT
                          DISPLAY_NAME "Development"
                          DESCRIPTION "All of the necessary source and libraries to develop software"
                          EXPANDED)

cpack_add_component(Library
                    DISPLAY_NAME "Libraries"
                    DESCRIPTION "Pre-built Libraries"
                    DISABLED
                    GROUP DEVELOPMENT
                    INSTALL_TYPES FULL DEVELOPER)

cpack_add_component(Archive
                    DISPLAY_NAME "Archives"
                    DESCRIPTION "Pre-built Libraries and Archives"
                    DISABLED
                    GROUP DEVELOPMENT
                    INSTALL_TYPES FULL DEVELOPER)

if (WSF_INSTALL_SOURCE)
   cpack_add_component(Source
                       DISPLAY_NAME "Source"
                       DESCRIPTION "Software source code"
                       GROUP DEVELOPMENT
                       INSTALL_TYPES FULL DEVELOPER)
endif()

if (WSF_INSTALL_DOXYGEN)
   cpack_add_component(Doxygen
                       DISPLAY_NAME "Doxygen"
                       DESCRIPTION "Software source code documentation"
                       GROUP DEVELOPMENT
                       INSTALL_TYPES FULL DEVELOPER)
endif()
