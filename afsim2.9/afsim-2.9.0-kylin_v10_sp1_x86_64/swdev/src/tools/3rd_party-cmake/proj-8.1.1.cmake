# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
get_filename_component(CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
include(${CURRENT_LIST_DIR}/shared.cmake)
set(PROJROOT ${proj_ROOT_DIR})
set(PROJ_ROOT ${proj_ROOT_DIR})
list(APPEND CMAKE_PREFIX_PATH ${PROJ_ROOT})
list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)
set(PROJINC ${PROJROOT}/include)
set(PROJLIBDIR ${PROJROOT}/${SWDEV_THIRD_PARTY_BUILD_TYPE})
set(PROJDEBBINDIR ${PROJROOT}/debug/bin)
set(PROJRELBINDIR ${PROJ_ROOT}/release/bin)
set(proj_LIBRARY ${PROJ_ROOT}/lib/libproj.so)

set(PROJLIB proj)

file(GLOB PROJ_INSTALL_LIBS ${PROJLIBDIR}/*.so*)

set(proj_INCLUDE_DIR ${PROJROOT}/include)
set_property(GLOBAL PROPERTY proj_BINDIR_DEBUG ${PROJROOT}/debug/bin)
set_property(GLOBAL PROPERTY proj_BINDIR_RELEASE ${PROJROOT}/release/bin)

