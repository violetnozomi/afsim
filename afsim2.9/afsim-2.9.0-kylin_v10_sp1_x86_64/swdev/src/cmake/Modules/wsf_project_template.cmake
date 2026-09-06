# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2020 Boeing. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include(swdev_project)
include(user_file_config)
include(GenerateExportHeader)

set(PLUGIN_wsf_ide_folder "wsf_plugins")
set(LIBRARY_wsf_ide_folder "core")
set(EXECUTABLE_wsf_ide_folder "applications")

if(NOT DEFINED WSF_INSTALL_DIR)
   set(WSF_INSTALL_DIR ".")
endif()

###############################################################################
#----Register a wsf project----
#
# expected project directory layout:
#  /docs/          # sphinx documentation for the project
#     /_conditional_section.txt  # one liner file containing '* <feature name the project passes to aApp.RegisterFeature()>'
#  /grammar/       # contains afsim grammar files
#     /*.ag
#  /source/        # contains header and source files
#  /test_mission/  # contains test scenarios to be ran by the mission application
#     /test_*.txt  # files that are picked up to be executed as part of the system tests
#  /CMakeLists.txt
#  /wsf_cmake_extension.cmake
#  /wsf_module
#
# wsf_project_template(
#   PROJECT_NAME <PLUGIN|LIBRARY|EXECUTABLE>
#   [HDRS ...]         # .hpp files
#   [SRCS ...]         # .cpp files
#   [GRAMMARS ...]     # grammar/*.ag files
#   [NOTICES ...]      # notice files. e.g. NO_EXPORT.md
#   [SUB_PROJECTS ...] # libraries to link that are in a directory below
#   [STANDALONE]       # specify to prevent linking the core WSF_LIBS to the project
#   )
#
# Note that the grammar files are first searched for in the current SOURCE directory, then the current BINARY directory
# to accommodate auto-generated grammar files.
macro(wsf_project_template PROJECT_NAME PROJECT_TYPE)
   set(options "STANDALONE")
   set(oneValueArgs "")
   set(multiValueArgs SRCS HDRS GRAMMARS SUB_PROJECTS NOTICES)
   cmake_parse_arguments(args "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

   set(_is_exe FALSE)
   set(_is_lib FALSE)
   set(_is_plugin FALSE)
   if(${PROJECT_TYPE} STREQUAL "PLUGIN")
      set(_is_plugin TRUE)
   elseif(${PROJECT_TYPE} STREQUAL "LIBRARY")
      set(_is_lib TRUE)
   elseif(${PROJECT_TYPE} STREQUAL "EXECUTABLE")
      set(_is_exe TRUE)
   else()
      message(SEND_ERROR "Illegal PROJECT_TYPE specified for ${PROJECT_NAME}: ${PROJECT_TYPE}")
   endif()

   set(_project_dir ${CMAKE_CURRENT_SOURCE_DIR})
   set(_project_install_dir ${WSF_INSTALL_DIR}/${PROJECT_NAME})
   if(_is_plugin)
      set(_project_install_dir ${WSF_INSTALL_DIR}/wsf_plugins/${PROJECT_NAME})
   elseif(_is_lib AND ${WSF_INSTALL_DIR} STREQUAL ".")
      set(_project_install_dir ${WSF_INSTALL_DIR}/core/${PROJECT_NAME})
   endif()
   set(_has_include_dir FALSE)
   if(EXISTS ${_project_dir}/include)
      set(_has_include_dir TRUE)
   endif()

   # *****************************************************************************
   # Add project to Sphinx for documentation
   add_wsf_doc_input(${_project_dir})

   # Add source directory to doxygen input
   add_wsf_doxygen_input(${_project_dir}/source)

   # Add include directory to doxygen input
   if(_has_include_dir)
      add_wsf_doxygen_input(${_project_dir}/include)
   endif()

   ###############
   #  grammars
   foreach(grammar ${args_GRAMMARS})
      if(EXISTS ${_project_dir}/${grammar})
         wsf_grammar_file(_grammars ${_project_dir}/${grammar})
      elseif(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${grammar})
         wsf_grammar_file(_grammars ${CMAKE_CURRENT_BINARY_DIR}/${grammar})
      else()
         message(FATAL_ERROR "Unable to locate grammar file ${grammar} for ${PROJECT_NAME}")
      endif()
   endforeach()

   #################
   # create target
   if(_is_exe)
      add_executable(${PROJECT_NAME} ${args_SRCS} ${args_HDRS} ${_grammars})
      set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/lib")
   else()
      add_library(${PROJECT_NAME} ${args_SRCS} ${args_HDRS} ${_grammars})
   endif()

   # configure compiler warning level for the target
   swdev_warning_level(${PROJECT_NAME})

   # Set the subfolder that the target will appear under in an IDE's solution explorer.
   # This isn't actually related to any file system folders.
   set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "${${PROJECT_TYPE}_wsf_ide_folder}")

   if(_has_include_dir)
      target_include_directories(${PROJECT_NAME} PUBLIC
                                 ${_project_dir}/include)
   else()
      target_include_directories(${PROJECT_NAME} PUBLIC
                                 ${_project_dir}/source)
   endif()

   # link core wsf libraries to the plugin
   if(NOT args_STANDALONE)
      target_link_libraries(${PROJECT_NAME} ${WSF_LIBS})
   endif()
   target_link_libraries(${PROJECT_NAME} ${SWDEV_THREAD_LIB} ${SWDEV_DL_LIB})

   if (MSVC)
      # AFSIM 2.2 changed how things are being exported over DLL boundaries
      # These warning suppression flags were added for that reason
      target_compile_options(${PROJECT_NAME} PRIVATE -wd4251 -wd4286 -wd4275)
   endif()

   ###################
   # sub projects
   if (args_SUB_PROJECTS)
      # include each sub project as a subdirectory
      set(_prev_wsf_install_dir ${WSF_INSTALL_DIR})
      set(WSF_INSTALL_DIR ${_project_install_dir})
      foreach(sublib ${args_SUB_PROJECTS})
         add_subdirectory(${sublib})
         #install_sources_all_files(${sublib} ${_project_install_dir})
         set_property(TARGET ${sublib} PROPERTY FOLDER "${${PROJECT_TYPE}_wsf_ide_folder}/${PROJECT_NAME}")
      endforeach()
      set(WSF_INSTALL_DIR ${_prev_wsf_install_dir})

      set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "${${PROJECT_TYPE}_wsf_ide_folder}/${PROJECT_NAME}")
      # link all sub project to the plugin
      target_link_libraries(${PROJECT_NAME} ${args_SUB_PROJECTS})
   endif()

   ################
   # export header
   if(NOT _is_exe)
      generate_export_header(${PROJECT_NAME})

      # add generated export header to list of sources
      string(TOLOWER ${PROJECT_NAME} _project_name_lower)
      target_sources(${PROJECT_NAME} PRIVATE
                     ${CMAKE_CURRENT_BINARY_DIR}/${_project_name_lower}_export.h)
      unset(_project_name_lower)
   endif()

   # include directory that the generated export header is placed in.
   target_include_directories(${PROJECT_NAME} PUBLIC
                              ${CMAKE_CURRENT_BINARY_DIR})

   #####################
   # install logic
   if(WSF_PLUGIN_BUILD AND _is_plugin)
      swdev_plugin_install(${PROJECT_NAME} wsf_plugins)
   elseif(_is_exe)
      install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_EXE_PATH} COMPONENT Runtime)
   else()
      swdev_lib_install(${PROJECT_NAME})
   endif()

   #if(args_GRAMMARS)
   if(EXISTS ${_project_dir}/grammar)
      install_sources_all_files(grammar ${_project_install_dir})
   endif()

   if(_has_include_dir)
      install_sources(include ${_project_install_dir})
   endif()
   install_sources(source ${_project_install_dir})

   # install /doc folder if it exists
   if(EXISTS ${_project_dir}/doc)
      install_sources_all_files(doc ${_project_install_dir})
   endif()

   # install test_mission folder if it exists
   if(EXISTS ${_project_dir}/test_mission)
      install_tests(test_mission ${_project_install_dir})
   endif()

   if(EXISTS ${_project_dir}/data)
      install_sources_all_files(data ${_project_install_dir})
   endif()

   # install any additional files
   set(_extra_files CMakeLists.txt)
   if(EXISTS ${_project_dir}/wsf_cmake_extension.cmake)
      # assuming all extension.cmake files have a matching wsf_module
      list(APPEND _extra_files wsf_module wsf_cmake_extension.cmake)
      if(NOT _is_exe)
         install_source_files(wsf_module ${_project_install_dir}/..)
      endif()
   endif()
   if(args_NOTICES)
      list(APPEND _extra_files ${args_NOTICES})
   endif()
   install_source_files(${_extra_files}
                        ${_project_install_dir})

   # Required in order to run in debug from Visual Studio
   if (MSVC_IDE AND _is_exe)
      create_vs_debug_env(
        "${_project_dir}"
        "${TEMPLATE_DIR}/Template.VisualStudio.Settings.user"
       )
   endif()

   if(COMMAND clangtidy_add_headers_to_compile_commands_json)
      clangtidy_add_headers_to_compile_commands_json(${args_HDRS})
   endif()

   unset(_has_include_dir)
   unset(_extra_files)
   unset(_grammars)
   unset(_is_exe)
   unset(_is_lib)
   unset(_is_plugin)
   # intentionally leak _project_install_dir
endmacro(wsf_project_template)
###############################################################################
