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

function(_gmock_append_debugs _endvar _library)
   if(${_library} AND ${_library}_DEBUG)
      set(_output optimized ${${_library}} debug ${${_library}}_DEBUG)
   else()
      set(_output ${${_library}})
   endif()
   set(${_endvar} ${_output} PARENT_SCOPE)
endfunction()

function(_gmock_find_library _name)
   find_library(${_name}
      NAMES ${ARGN}
      HINTS
         ENV GTEST_ROOT
         ${GTEST_ROOT}
      PATH_SUFFIXES ${_gmock_libpath_suffixes})
   mark_as_advanced(${_name})
endfunction()

macro(_gmock_determine_windows_library_type _var)
   if(EXISTS "${${_var}}")
      file(TO_NATIVE_PATH "${${_var}}" _lib_path)
      get_filename_component(_name "${${_var}}" NAME_WE)
      file(STRINGS "${${_var}}" _match REGEX "${_name}\\.dll" LIMIT_COUNT 1)
      if(NOT _match STREQUAL "")
         set(${_var}_TYPE SHARED PARENT_SCOPE)
      else()
         set(${_var}_TYPE UNKNOWN PARENT_SCOPE)
      endif()
      return()
   endif()
endmacro()

function(_gmock_determine_library_type _var)
   if(WIN32)
      # For now, at least, only Windows really needs to know the library type
      _gmock_determine_windows_library_type(${_var})
      _gmock_determine_windows_library_type(${_var}_RELEASE)
      _gmock_determine_windows_library_type(${_var}_DEBUG)
   endif()
   # If we get here, no determination was made from the above checks
   set(${_var}_TYPE UNKNOWN PARENT_SCOPE)
endfunction()

function(_gmock_import_library _target _var _config)
   if(_config)
      set(_config_suffix "_${_config}")
   else()
      set(_config_suffix "")
   endif()

   set(_lib "${${_var}${_config_suffix}}")
   if(EXISTS "${_lib}")
      if(_config)
         set_property(TARGET ${_target} APPEND PROPERTY
            IMPORTED_CONFIGURATIONS ${_config})
      endif()
      set_target_properties(${_target} PROPERTIES
         IMPORTED_LINK_INTERFACE_LANGUAGES${_config_suffix} "CXX"
         IMPORTED_LOCATION${_config_suffix} "${_lib}")
   endif()
endfunction()

#

if(NOT DEFINED GMOCK_MSVC_SEARCH)
    set(GMOCK_MSVC_SEARCH MD)
endif()

set(_gmock_libpath_suffixes lib)
if(MSVC)
    if(GMOCK_MSVC_SEARCH STREQUAL "MD")
        list(APPEND _gmock_libpath_suffixes
            msvc/gtest-md/Debug
            msvc/gtest-md/Release
            msvc/x64/Debug
            msvc/x64/Release
            )
    elseif(GTEST_MSVC_SEARCH STREQUAL "MT")
        list(APPEND _gmock_libpath_suffixes
            msvc/gtest/Debug
            msvc/gtest/Release
            msvc/x64/Debug
            msvc/x64/Release
            )
    endif()
endif()

find_path(GMOCK_INCLUDE_DIR gmock/gmock.h
   HINTS
      ENV GTEST_ROOT
      ${GTEST_ROOT}
   PATH_SUFFIXES include
)
mark_as_advanced(GMOCK_INCLUDE_DIR)

if(MSVC AND GTEST_MSVC_SEARCH STREQUAL "MD")
   # The provided /MD project files for Google Mock add -md suffixes to the
   # library names
   _gmock_find_library(GMOCK_LIBRARY gmock-md gmock)
   _gmock_find_library(GMOCK_LIBRARY_DEBUG gmock-mdd gmockd)
   _gmock_find_library(GMOCK_MAIN_LIBRARY gmock_main-md gmock_main)
   _gmock_find_library(GMOCK_MAIN_LIBRARY_DEBUG gmock_main-mdd gmock_maind)
else()
   _gmock_find_library(GMOCK_LIBRARY gmock)
   _gmock_find_library(GMOCK_LIBRARY_DEBUG gmockd)
   _gmock_find_library(GMOCK_MAIN_LIBRARY gmock_main)
   _gmock_find_library(GMOCK_MAIN_LIBRARY_DEBUG gmock_maind)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GMock DEFAULT_MSG GMOCK_LIBRARY GMOCK_INCLUDE_DIR GMOCK_MAIN_LIBRARY)

if(GMOCK_FOUND)
   set(GMOCK_INCLUDE_DIRS ${GMOCK_INCLUDE_DIR})
   _gmock_append_debugs(GMOCK_LIBRARIES GMOCK_LIBRARY)
   _gmock_append_debugs(GMOCK_MAIN_LIBRARIES GMOCK_MAIN_LIBRARY)
   set(GMOCK_BOTH_LIBRARIES ${GMOCK_LIBRARIES} ${GMOCK_MAIN_LIBRARIES})
   
   find_package(GTest REQUIRED)
   
   if(NOT TARGET GMock::GMock)
      add_library(GMock::GMock UNKNOWN IMPORTED)
      set_target_properties(GMock::GMock PROPERTIES
         INTERFACE_LINK_LIBRARIES "GTest::GTest")
      if(GMOCK_INCLUDE_DIRS)
         set_target_properties(GMock::GMock PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GMOCK_INCLUDE_DIRS}")
      endif()
      _gmock_import_library(GMock::GMock GMOCK_LIBRARY "")
      _gmock_import_library(GMock::GMock GMOCK_LIBRARY "RELEASE")
      _gmock_import_library(GMock::GMock GMOCK_LIBRARY "DEBUG")
   endif()
   
   if(NOT TARGET GMock::Main)
      add_library(GMock::Main UNKNOWN IMPORTED)
      set_target_properties(GMock::Main PROPERTIES
         INTERFACE_LINK_LIBRARIES "GMock::GMock")
      _gmock_import_library(GMock::Main GMOCK_MAIN_LIBRARY "")
      _gmock_import_library(GMock::Main GMOCK_MAIN_LIBRARY "RELEASE")
      _gmock_import_library(GMock::Main GMOCK_MAIN_LIBRARY "DEBUG")
   endif()
endif()
