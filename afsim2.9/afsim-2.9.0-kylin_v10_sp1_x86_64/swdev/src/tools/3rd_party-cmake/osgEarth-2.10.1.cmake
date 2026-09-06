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

set(OSGEARTHROOT ${osgEarth_ROOT_DIR})
set(OSGEARTHINC ${OSGEARTHROOT}/include)
set(OSGEARTHLIBDIR ${OSGEARTHROOT}/${SWDEV_THIRD_PARTY_BUILD_TYPE}/lib)
set(OSGEARTHLIB osgEarth)
set(OSGEARTHDEBBINDIR ${OSGEARTHROOT}/debug/bin)
set(OSGEARTHRELBINDIR ${OSGEARTHROOT}/release/bin)
set(OSGEARTHBINSUBDIRLIST osgPlugins-3.6.3)

file(GLOB OSGEARTH_INSTALL_LIBS ${OSGEARTHLIBDIR}/bin/*.so*)

macro(link_osgearth TARGET)
   if(WIN32)
      target_link_libraries(${TARGET} debug ${OSGEARTHROOT}/debug/lib/osgEarthd.lib optimized ${OSGEARTHROOT}/release/lib/osgEarth.lib)
      target_link_libraries(${TARGET} debug ${OSGEARTHROOT}/debug/lib/osgEarthUtild.lib optimized ${OSGEARTHROOT}/release/lib/osgEarthUtil.lib)
      target_link_libraries(${TARGET} debug ${OSGEARTHROOT}/debug/lib/osgEarthFeaturesd.lib optimized ${OSGEARTHROOT}/release/lib/osgEarthFeatures.lib)
      target_link_libraries(${TARGET} debug ${OSGEARTHROOT}/debug/lib/osgEarthSymbologyd.lib optimized ${OSGEARTHROOT}/release/lib/osgEarthSymbology.lib)
   else(WIN32)
      target_link_libraries(${TARGET} ${OSGEARTHROOT}/lib64/libosgEarth.so)
      target_link_libraries(${TARGET} ${OSGEARTHROOT}/lib64/libosgEarthUtil.so)
      target_link_libraries(${TARGET} ${OSGEARTHROOT}/lib64/libosgEarthFeatures.so)
      target_link_libraries(${TARGET} ${OSGEARTHROOT}/lib64/libosgEarthSymbology.so)
   endif(WIN32)
endmacro(link_osgearth)

macro(install_osgearth LIBROOT)
   if(WIN32)
      file(GLOB REL_DLLS ${LIBROOT}/release/bin/*.dll)
      file(GLOB DBG_DLLS ${LIBROOT}/debug/bin/*.dll)
      install(FILES ${REL_DLLS} DESTINATION ${INSTALL_DLL_PATH} CONFIGURATIONS Release PERMISSIONS
              OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
      install(FILES ${DBG_DLLS} DESTINATION ${INSTALL_DLL_PATH} CONFIGURATIONS Debug PERMISSIONS
              OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
      file(GLOB PLUGINLIST ${LIBROOT}/release/bin/osgPlugins*)
      foreach(FILE ${PLUGINLIST})
         if(IS_DIRECTORY ${FILE})
            get_filename_component(PLUGINDIRNAME ${FILE} NAME)
            file(GLOB REL_PLUGIN_DLLS ${FILE}/*.dll)
         endif(IS_DIRECTORY ${FILE})
      endforeach(FILE)
      file(GLOB PLUGINLIST ${LIBROOT}/debug/bin/osgPlugins*)
      foreach(FILE ${PLUGINLIST})
         if(IS_DIRECTORY ${FILE})
            get_filename_component(PLUGINDIRNAME ${FILE} NAME)
            file(GLOB DBG_PLUGIN_DLLS ${FILE}/*.dll)
         endif(IS_DIRECTORY ${FILE})
      endforeach(FILE)
      install(FILES ${REL_PLUGIN_DLLS} CONFIGURATIONS Release DESTINATION ${INSTALL_DLL_PATH}/${PLUGINDIRNAME} PERMISSIONS
              OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
      install(FILES ${DBG_PLUGIN_DLLS} CONFIGURATIONS Debug DESTINATION ${INSTALL_DLL_PATH}/${PLUGINDIRNAME} PERMISSIONS
              OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
   else(WIN32)
      file(GLOB REL_SOS ${LIBROOT}/lib64/*.so*)
      install(FILES ${REL_SOS} DESTINATION ${INSTALL_DLL_PATH} PERMISSIONS
              OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
      file(GLOB PLUGINLIST ${LIBROOT}/lib64/osgPlugins*)
      foreach(FILE ${PLUGINLIST})
         if(IS_DIRECTORY ${FILE})
            get_filename_component(PLUGINDIRNAME ${FILE} NAME)
            file(GLOB REL_PLUGIN_DLLS ${FILE}/*.so*)
         endif(IS_DIRECTORY ${FILE})
      endforeach(FILE)
      install(FILES ${REL_PLUGIN_DLLS} DESTINATION ${INSTALL_DLL_PATH}/${PLUGINDIRNAME} PERMISSIONS
              OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
   endif(WIN32)
   file(GLOB LICENSES ${LIBROOT}/license*/*.txt)
   install(FILES ${LICENSES} DESTINATION ${INSTALL_EXE_PATH}/../resources/data/3rd_party_licenses PERMISSIONS
           OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Resource)
endmacro(install_osgearth LIBROOT)

set(osgEarth_INCLUDE_DIR ${OSGEARTHROOT}/include)
set_property(GLOBAL PROPERTY osgEarth_BINDIR_DEBUG ${OSGEARTHROOT}/debug/bin)
set_property(GLOBAL PROPERTY osgEarth_BINDIR_RELEASE ${OSGEARTHROOT}/release/bin)
if(WIN32)
   set(osgEarth_LIBRARY debug ${OSGEARTHROOT}/debug/lib/osgEarthd.lib
                        debug ${OSGEARTHROOT}/debug/lib/osgEarthUtild.lib
                        debug ${OSGEARTHROOT}/debug/lib/osgEarthFeaturesd.lib
                        debug ${OSGEARTHROOT}/debug/lib/osgEarthSymbologyd.lib
                        optimized ${OSGEARTHROOT}/release/lib/osgEarth.lib
                        optimized ${OSGEARTHROOT}/release/lib/osgEarthUtil.lib
                        optimized ${OSGEARTHROOT}/release/lib/osgEarthFeatures.lib
                        optimized ${OSGEARTHROOT}/release/lib/osgEarthSymbology.lib)
else(WIN32)
   set(osgEarth_LIBRARY ${OSGEARTHROOT}/lib64/libosgEarth.so
                        ${OSGEARTHROOT}/lib64/libosgEarthUtil.so
                        ${OSGEARTHROOT}/lib64/libosgEarthFeatures.so
                        ${OSGEARTHROOT}/lib64/libosgEarthSymbology.so)
endif(WIN32)
