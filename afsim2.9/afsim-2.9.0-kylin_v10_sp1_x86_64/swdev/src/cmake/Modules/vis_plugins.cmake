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

# *****************************************************************************
# CMAKE file: vis_plugins
# Macros and configuration to support builds and installation of visualization
# software that hosts plugins: e.g., wkf, warlock, mystic, wizard
# *****************************************************************************
include(swdev_project)

# Recursively find all plugin configuration files
set(PLUGIN_CONFIG_FILES)
foreach(PLUGIN_ROOT_DIR ${PLUGIN_ROOT_DIRS})
   file(GLOB_RECURSE PLUGIN_CONFIGS ${PLUGIN_ROOT_DIR}/*_plugin.cmake)
   set(PLUGIN_CONFIG_FILES ${PLUGIN_CONFIG_FILES} ${PLUGIN_CONFIGS})
endforeach()

# Adds the plugin as a CMake option (BUILD_<HOSTNAME>_PLUGIN_xxxx) and
# sets directory properties that locate and identify the plugins
function(add_vis_plugin_project HOSTNAME PROJSOURCE PROJNAME PROJBUILD)
   set(BUILD_${HOSTNAME}_PLUGIN_${PROJNAME} ${PROJBUILD} CACHE BOOL "Build ${HOSTNAME} with Plugin ${PROJNAME}")
   set_property(DIRECTORY PROPERTY ${HOSTNAME}_PLUGIN_PROJECT_NAMES ${PROJNAME} APPEND)
   set_property(DIRECTORY PROPERTY ${HOSTNAME}_PLUGIN_PROJECT_SRCS "${PROJSOURCE}" APPEND)
endfunction()

# Add any plugins that have been found to the build
macro(configure_vis_plugin HOSTNAME PLUGIN_CONFIG_FILE)
   # Initialize variables to be set by plugin configuration
   set(${HOSTNAME}_PLUGIN_NAME) # The plugin name
   set(${HOSTNAME}_PLUGIN_SOURCE_PATH) # Relative path from plugin's cmake to source directory
   set(${HOSTNAME}_PLUGIN_BUILD ${ENABLE_EXTENSION_DEFAULT}) # Switch for whether a plugin is built by default

   include("${PLUGIN_CONFIG_FILE}")
   if (${HOSTNAME}_PLUGIN_NAME)
      message(STATUS "Configuring ${HOSTNAME} Plugin ${${HOSTNAME}_PLUGIN_NAME}")
      get_filename_component(PLUGIN_CONFIG_DIR "${PLUGIN_CONFIG_FILE}" PATH)
      set(PROJSOURCE "${PLUGIN_CONFIG_DIR}/${${HOSTNAME}_PLUGIN_SOURCE_PATH}")
      add_vis_plugin_project(${HOSTNAME} "${PROJSOURCE}" ${${HOSTNAME}_PLUGIN_NAME} ${${HOSTNAME}_PLUGIN_BUILD})
   endif()
endmacro()

swdev_absolute_paths(PLUGIN_CONFIG_FILES ${PLUGIN_CONFIG_FILES})

# Configure the build to include plugins
macro(use_vis_plugins HOSTNAME)
   foreach(PLUGIN_CONFIG_FILE ${PLUGIN_CONFIG_FILES})
      configure_vis_plugin(${HOSTNAME} ${PLUGIN_CONFIG_FILE})
   endforeach()

   get_property(${HOSTNAME}_PLUGIN_PROJECT_NAMES DIRECTORY PROPERTY ${HOSTNAME}_PLUGIN_PROJECT_NAMES)
   get_property(${HOSTNAME}_PLUGIN_PROJECT_SRCS DIRECTORY PROPERTY ${HOSTNAME}_PLUGIN_PROJECT_SRCS)
   foreach(PROJNAME ${${HOSTNAME}_PLUGIN_PROJECT_NAMES})
      if(BUILD_${HOSTNAME}_PLUGIN_${PROJNAME})
         list(FIND ${HOSTNAME}_PLUGIN_PROJECT_NAMES ${PROJNAME} IDX)
         list(GET ${HOSTNAME}_PLUGIN_PROJECT_SRCS ${IDX} PROJSOURCE)
         add_subdirectory("${PROJSOURCE}" "${PROJNAME}")
      endif()
   endforeach()
endmacro()

# Move plugin into subdirectory
# Required in order to run in debug from Visual Studio
macro(vis_plugin_post_build PLUGIN_NAME)
   if (WIN32)
      get_filename_component(DIR_NAME ${PLUGIN_INSTALL_DIR} NAME)
      add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
                         COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PLUGIN_NAME}>/${DIR_NAME}" VERBATIM
                         COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PLUGIN_NAME}> $<TARGET_FILE_DIR:${PLUGIN_NAME}>/${DIR_NAME} VERBATIM)
   endif()
endmacro()

# Optional arguments:
# NO_INSTALL_SOURCE
#     Specifies that the install commands should not be ran on the source
macro(vis_plugin_install PLUGIN_NAME TARGET_DIR)
   set(options NO_INSTALL_SOURCE)
   cmake_parse_arguments(_PI "${options}" "" "" ${ARGN})

   install(TARGETS ${PLUGIN_NAME} RUNTIME DESTINATION ${PLUGIN_INSTALL_DIR} COMPONENT Runtime
                                  LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR} COMPONENT Library)
   # Install PDB file for Debug configuration when using MSVC
   if (MSVC)
      install(FILES $<TARGET_PDB_FILE:${PLUGIN_NAME}> DESTINATION ${PLUGIN_INSTALL_DIR}
              CONFIGURATIONS Debug RelWithDebInfo COMPONENT Runtime)
   endif()
   if (WSF_INSTALL_SOURCE AND NOT _PI_NO_INSTALL_SOURCE)
      install_sources_all_files(../ ${TARGET_DIR})
   endif()
endmacro()

# plugin_cmakelists_template(PLUGIN_NAME
#                            HOST_TARGET
#                            [THIRD_PARTY_INCLUDE_DIRS <pkgs>...]
#                            [THIRD_PARTY_LINK_LIBS <pkgs>...]
#                            [QT_MODULES <modules>...]
#                            [LIBRARIES <libs>...]
#                            [ADDITIONAL_SOURCES <srcs>...]
#                            [SOURCE_INSTALL_DIR <directory>]
#                            [NO_INSTALL_SOURCE])
# Optional arguments:
# THIRD_PARTY_INCLUDE_DIRS
#     Specifies 3rd party packages to add to include directories
# THIRD_PARTY_LINK_LIBS
#     Specifies 3rd party package libraries to link target with
# QT_MODULES
#     Specifies additional Qt modules (beyond Core, Gui, Widgets) to import as targets
# REQUIRES
#     Specifies internal optional libraries to link to, checked at configure time
# LIBRARIES
#     Specifies internal library targets to link to
# ADDITIONAL_SOURCES
#     Specifies additional source files
# SOURCE_INSTALL_DIR
#     Specifies the non-standard relative source target install dir
# NO_INSTALL_SOURCE
#     Specifies that the install commands should not be ran
macro(plugin_cmakelists_template PLUGIN_NAME HOST_TARGET)
   # Parse optional arguments to macro
   set(options NO_INSTALL_SOURCE)
   set(oneValueArgs SOURCE_INSTALL_DIR)
   set(multiValueArgs THIRD_PARTY_INCLUDE_DIRS THIRD_PARTY_LINK_LIBS QT_MODULES REQUIRES LIBRARIES ADDITIONAL_SOURCES)
   cmake_parse_arguments(PLUGIN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

   # *****************************************************************************
   # Get the source files
   file(GLOB SRCS *.cpp *.hpp)
   file(GLOB QRC ${CMAKE_CURRENT_SOURCE_DIR}/../ui/resources/*.qrc)
   file(GLOB UIS ${CMAKE_CURRENT_SOURCE_DIR}/../ui/*.ui)

   include(qt_project)
   configure_qt_project(MODULES ${PLUGIN_QT_MODULES} UI UI_HEADERS ${UIS})

   # *****************************************************************************
   # The library we are trying to build
   include_directories(${afsim_BINARY_DIR}/include)

   add_library(${PLUGIN_NAME} ${SRCS} ${QRC} ${UI_HEADERS} ${PLUGIN_ADDITIONAL_SOURCES})
   swdev_warning_level(${PLUGIN_NAME})

   # Place under plugins folder in IDE
   set_property(TARGET ${PLUGIN_NAME} PROPERTY FOLDER "${HOST_TARGET}/plugins")

   # Configure the RUNPATH for linux installation
   set_target_properties(${PLUGIN_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib")

   vis_plugin_post_build(${PLUGIN_NAME})

   set(_SOURCE_INSTALL)
   if (PLUGIN_NO_INSTALL_SOURCE)
         set(_SOURCE_INSTALL "NO_INSTALL_SOURCE")
   endif()
   if (PLUGIN_SOURCE_INSTALL_DIR)
      set(REL_PATH ${PLUGIN_SOURCE_INSTALL_DIR})
   else()
      file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
   endif()
   vis_plugin_install(${PLUGIN_NAME} ${REL_PATH} ${_SOURCE_INSTALL})

   # *****************************************************************************
   # Add project to Sphinx for documentation
   add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/..)

   # Add source directories to doxygen input
   #add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR})

   if (PLUGIN_THIRD_PARTY_INCLUDE_DIRS)
      foreach(pkg ${PLUGIN_THIRD_PARTY_INCLUDE_DIRS})
         swdev_package_include_directories(${PLUGIN_NAME} ${pkg})
      endforeach()
   endif()

   if (PLUGIN_THIRD_PARTY_LINK_LIBS)
      foreach(pkg ${PLUGIN_THIRD_PARTY_LINK_LIBS})
         swdev_package_link_libraries(${PLUGIN_NAME} ${pkg})
      endforeach()
   endif()

   if (PLUGIN_REQUIRES)
      # get hostname
      if (${HOST_TARGET} STREQUAL wizard)
         set(hostname WIZARD)
      elseif (${HOST_TARGET} STREQUAL warlock)
         set(hostname WARLOCK)
      elseif (${HOST_TARGET} STREQUAL mystic)
         set(hostname MYSTIC)
      elseif (${HOST_TARGET} STREQUAL tools/wkf)
         set(hostname WKF)
      endif()

      # link libraries
      foreach (req ${PLUGIN_REQUIRES})
         if (BUILD_WITH_${req} OR BUILD_${hostname}_PLUGIN_${req})
            target_link_libraries(${PLUGIN_NAME} ${req})
         elseif (DEFINED BUILD_${hostname}_PLUGIN_${req})
            message(SEND_ERROR "Cannot build ${PLUGIN_NAME} for ${hostname}: BUILD_${hostname}_PLUGIN_${req} must be set to true")
         else()
            message(SEND_ERROR "Cannot build ${PLUGIN_NAME} for ${hostname}: BUILD_WITH_${req} must be set to true")
         endif()
      endforeach()
   endif()

   if (PLUGIN_LIBRARIES)
      foreach(lib ${PLUGIN_LIBRARIES})
         target_link_libraries(${PLUGIN_NAME} ${lib})
      endforeach()
   endif()

   if (PLUGIN_QT_MODULES)
      foreach(module ${PLUGIN_QT_MODULES})
         target_link_libraries(${PLUGIN_NAME} Qt5::${module})
         install_qt_lib(Qt5${module})
      endforeach()
   endif()
endmacro()
