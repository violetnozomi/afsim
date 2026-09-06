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
set(GEOSROOT ${geos_ROOT_DIR})
set(GEOSINC ${GEOSROOT}/include)
set(GEOSLIBDIR ${GEOSROOT}/${SWDEV_THIRD_PARTY_BUILD_TYPE})
set(GEOSLIB geos)
set(GEOSDEBBINDIR ${GEOSROOT}/debug/bin)
set(GEOSRELBINDIR ${GEOSROOT}/release/bin)

file(GLOB GEOS_INSTALL_LIBS ${GEOSLIBDIR}/*.so*)
macro(link_geos TARGET)
   if(WIN32)
      target_link_libraries(${TARGET} debug ${GEOSROOT}/debug/lib/geos.lib
                                      optimized ${GEOSROOT}/release/lib/geos.lib)
   else(WIN32)
      target_link_libraries(${TARGET} ${GEOSROOT}/lib/libgeos.so)
   endif(WIN32)
endmacro(link_geos)

set(geos_INCLUDE_DIR ${GEOSROOT}/include)
set_property(GLOBAL PROPERTY geos_BINDIR_DEBUG ${GEOSROOT}/debug/bin)
set_property(GLOBAL PROPERTY geos_BINDIR_RELEASE ${GEOSROOT}/release/bin)
if(WIN32)
   set(geos_LIBRARY debug ${GEOSROOT}/debug/lib/geos.lib
                    optimized ${GEOSROOT}/release/lib/geos.lib)
else(WIN32)
   set(geos_LIBRARY ${GEOSROOT}/lib/libgeos.so)
endif(WIN32)
