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
get_filename_component(CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
include(${CURRENT_LIST_DIR}/shared.cmake)
set(tinyxml2_ROOT ${tinyxml2_ROOT_DIR})
list(APPEND CMAKE_PREFIX_PATH ${tinyxml2_ROOT})
list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)
set_property(GLOBAL PROPERTY tinyxml2_BINDIR_DEBUG ${tinyxml2_ROOT}/bin)
set_property(GLOBAL PROPERTY tinyxml2_BINDIR_RELEASE ${tinyxml2_ROOT}/bin)

function(install_tinyxml2)
   if(WIN32)
      install(FILES ${tinyxml2_ROOT}/bin/tinyxml2.dll DESTINATION "${INSTALL_DLL_PATH}" CONFIGURATIONS Release RelWithDebInfo COMPONENT Runtime)
      install(FILES ${tinyxml2_ROOT}/bin/tinyxml2d.dll DESTINATION "${INSTALL_DLL_PATH}" CONFIGURATIONS Debug COMPONENT Runtime)
   else()
      if(EXISTS ${tinyxml2_ROOT}/lib64)
         install(FILES
            ${tinyxml2_ROOT}/lib64/libtinyxml2.so.7.1.0
            ${tinyxml2_ROOT}/lib64/libtinyxml2.so.7
            ${tinyxml2_ROOT}/lib64/libtinyxml2.so
            DESTINATION "${INSTALL_DLL_PATH}" COMPONENT Runtime)
      else()
         install(FILES
            ${tinyxml2_ROOT}/lib/libtinyxml2.so.7.1.0
            ${tinyxml2_ROOT}/lib/libtinyxml2.so.7
            #${tinyxml2_ROOT}/lib/libtinyxml2
            DESTINATION "${INSTALL_DLL_PATH}" COMPONENT Runtime)
      endif()
   endif()
endfunction()
