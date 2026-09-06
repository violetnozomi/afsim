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
set(GDALROOT ${gdal_ROOT_DIR})
set(GDALINC ${GDALROOT}/include)
set(GDALLIBDIR ${GDALROOT}/${SWDEV_THIRD_PARTY_BUILD_TYPE})
set(GDALLIB gdal)
set(GDALDEBBINDIR ${GDALROOT}/debug)
set(GDALRELBINDIR ${GDALROOT}/release)

file(GLOB GDAL_INSTALL_LIBS ${GDALLIBDIR}/*.so*)
macro(link_gdal TARGET)
   if(WIN32)
      target_link_libraries(${TARGET} debug ${GDALROOT}/debug/gdal_i_d.lib optimized ${GDALROOT}/release/gdal_i.lib)
   else(WIN32)
      target_link_libraries(${TARGET} ${GDALROOT}/lib/libgdal.so)
   endif(WIN32)
endmacro(link_gdal)

set(gdal_INCLUDE_DIR ${GDALROOT}/include)
set_property(GLOBAL PROPERTY gdal_BINDIR_DEBUG ${GDALROOT}/debug)
set_property(GLOBAL PROPERTY gdal_BINDIR_RELEASE ${GDALROOT}/release)
if(WIN32)
   set(gdal_LIBRARY debug ${GDALROOT}/debug/gdal_i_d.lib optimized ${GDALROOT}/release/gdal_i.lib)
else()
   set(gdal_LIBRARY ${GDALROOT}/lib/libgdal.so)
endif()
