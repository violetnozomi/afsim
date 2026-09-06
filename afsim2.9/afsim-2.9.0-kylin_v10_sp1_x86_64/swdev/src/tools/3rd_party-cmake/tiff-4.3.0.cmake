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
set(TIFFROOT ${tiff_ROOT_DIR})
set(TIFFINC ${TIFFROOT}/include)
set(TIFFLIBDIR ${TIFFROOT}/${SWDEV_THIRD_PARTY_BUILD_TYPE})
set(TIFFDEBBINDIR ${TIFFROOT}/debug/bin)
set(TIFFRELBINDIR ${TIFFROOT}/release/bin)
set(TIFFLIB tiff)

file(GLOB TIFF_INSTALL_LIBS ${TIFFLIBDIR}/*.so*)
macro(link_tiff TARGET)
   if(WIN32)
      target_link_libraries(${TARGET} debug ${TIFFROOT}/debug/lib/tiffd.lib optimized ${TIFFROOT}/release/lib/tiff.lib)
   else(WIN32)
      target_link_libraries(${TARGET} debug ${TIFFROOT}/bin/libtiff.so optimized ${TIFFROOT}/release/libtiff.so)
   endif(WIN32)
endmacro(link_tiff)

set(tiff_INCLUDE_DIR ${TIFFROOT}/include)
set_property(GLOBAL PROPERTY tiff_BINDIR_DEBUG ${TIFFROOT}/debug/bin)
set_property(GLOBAL PROPERTY tiff_BINDIR_RELEASE ${TIFFROOT}/release/bin)
if(WIN32)
   set(tiff_LIBRARY debug ${TIFFROOT}/debug/lib/tiffd.lib optimized ${TIFFROOT}/release/lib/tiff.lib)
else()
   set(tiff_LIBRARY debug ${TIFFROOT}/debug/libtiff.so optimized ${TIFFROOT}/release/libtiff.so)
endif()
