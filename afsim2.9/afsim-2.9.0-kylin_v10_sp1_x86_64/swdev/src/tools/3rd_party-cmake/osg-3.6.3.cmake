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
set(OSGROOT ${osg_ROOT_DIR})
set(OSGINC "${OSGROOT}/include")
set(OSGLIBDIR ${OSGROOT}/${SWDEV_THIRD_PARTY_BUILD_TYPE})
set(OSGLIB osg)
set(OSGDEBBINDIR ${OSGROOT}/debug/bin)
set(OSGRELBINDIR ${OSGROOT}/release/bin)
set(OSGBINSUBDIRLIST osgPlugins-3.6.3)

file(GLOB OSG_INSTALL_LIBS ${OSGLIBDIR}/*.so*)

macro(copy_osg_dlls_to_bin_p BINFILES TARGET LIBROOT outDir debugOrRelease)
   # Linux uses rpath to find these shared objects...
   if(WIN32)
      file(GLOB DLLS ${LIBROOT}/${debugOrRelease}/bin/*.dll)
      foreach(DLL ${DLLS})
         copy_file_build_rule(${BINFILES} ${DLL} ${outDir})
      endforeach(DLL ${DLLS})
      set(SHARED_OBJ_EXT "*.dll")
   else()
      file(GLOB DLLS ${LIBROOT}/${debugOrRelease}/lib64/*.so)
      foreach(DLL ${DLLS})
         copy_file_build_rule(${BINFILES} ${DLL} ${outDir})
      endforeach(DLL ${DLLS})
      set(SHARED_OBJ_EXT "*.so")
   endif()
   file(GLOB PLUGINLIST ${LIBROOT}/${debugOrRelease}/bin/osgPlugins*)
   foreach(PLUGINDIR ${PLUGINLIST})
      if(IS_DIRECTORY ${PLUGINDIR})
         get_filename_component(DIRNAME ${PLUGINDIR} NAME)
         file(GLOB DLLS ${PLUGINDIR}/${SHARED_OBJ_EXT})
         foreach(DLL ${DLLS})
            copy_file_build_rule(${BINFILES} ${DLL} "${outDir}/${DIRNAME}")
         endforeach(DLL ${DLLS})
      endif()
   endforeach(PLUGINDIR)
endmacro()

macro(copy_osg_dlls_to_bin BINFILES TARGET LIBROOT)
   copy_osg_dlls_to_bin_p(${BINFILES} ${TARGET} ${LIBROOT} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug" "debug")
   copy_osg_dlls_to_bin_p(${BINFILES} ${TARGET} ${LIBROOT} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release" "release")
endmacro()

macro(link_osg TARGET)
   if(WIN32)
      target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgd.lib optimized ${OSGROOT}/release/lib/osg.lib)
      target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/OpenThreadsd.lib optimized ${OSGROOT}/release/lib/OpenThreads.lib)
      target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgViewerd.lib optimized ${OSGROOT}/release/lib/osgViewer.lib)
      target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgTextd.lib optimized ${OSGROOT}/release/lib/osgText.lib)
      target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgdbd.lib optimized ${OSGROOT}/release/lib/osgdb.lib)
      target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgUtild.lib optimized ${OSGROOT}/release/lib/osgUtil.lib)
      target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgGAd.lib optimized ${OSGROOT}/release/lib/osgGA.lib)
      target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgSimd.lib optimized ${OSGROOT}/release/lib/osgSim.lib)
      target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgAnimationd.lib optimized ${OSGROOT}/release/lib/osgAnimation.lib)
      target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgFXd.lib optimized ${OSGROOT}/release/lib/osgFX.lib)
     #target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgManipulatord.lib optimized ${OSGROOT}/release/lib/osgManipulator.lib)
     target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgParticled.lib optimized ${OSGROOT}/release/lib/osgParticle.lib)
     #target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgPresentationd.lib optimized ${OSGROOT}/release/lib/osgPresentation.lib)
     #target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgShadowd.lib optimized ${OSGROOT}/release/lib/osgShadow.lib)
     #target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgTerraind.lib optimized ${OSGROOT}/release/lib/osgTerrain.lib)
     #target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgUId.lib optimized ${OSGROOT}/release/lib/osgUI.lib)
     #target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgVolumed.lib optimized ${OSGROOT}/release/lib/osgVolume.lib)
     #target_link_libraries(${TARGET} debug ${OSGROOT}/debug/lib/osgWidgetd.lib optimized ${OSGROOT}/release/lib/osgWidget.lib)

   else(WIN32)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosg.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libOpenThreads.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgViewer.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgText.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgDB.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgUtil.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgGA.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgSim.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgAnimation.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgTerrain.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgFX.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgShadow.so)
      target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgVolume.so)
     #target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgManipulator.so)
     target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgParticle.so)
     #target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgPresentation.so)
     #target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgUI.so)
     #target_link_libraries(${TARGET} ${OSGROOT}/lib64/libosgWidget.so)
   endif(WIN32)
endmacro(link_osg)

macro(install_osg LIBROOT)
   if(WIN32)
      file(GLOB REL_DLLS ${LIBROOT}/release/bin/*.dll)
      file(GLOB DBG_DLLS ${LIBROOT}/debug/bin/*.dll)
      install(FILES ${REL_DLLS} DESTINATION ${INSTALL_DLL_PATH} CONFIGURATIONS Release RelWithDebInfo PERMISSIONS
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
      install(FILES ${REL_PLUGIN_DLLS} CONFIGURATIONS Release RelWithDebInfo DESTINATION ${INSTALL_DLL_PATH}/${PLUGINDIRNAME} PERMISSIONS
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
      file(GLOB PLUGINLIST ${LIBROOT}/lib64/osgPlugins*)
      install(FILES ${REL_PLUGIN_DLLS} DESTINATION ${INSTALL_DLL_PATH}/${PLUGINDIRNAME} PERMISSIONS
              OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
   endif(WIN32)
   file(GLOB LICENSES ${LIBROOT}/license*/*.txt)
   install(FILES ${LICENSES} DESTINATION ${INSTALL_EXE_PATH}/../resources/data/3rd_party_licenses PERMISSIONS
           OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Resource)
endmacro(install_osg LIBROOT)

set(osg_INCLUDE_DIR ${OSGROOT}/include)
if(WIN32)
   set_property(GLOBAL PROPERTY osg_BINDIR_RELEASE ${OSGROOT}/release/bin)
   set_property(GLOBAL PROPERTY osg_BINDIR_DEBUG ${OSGROOT}/debug/bin)
else()
   set_property(GLOBAL PROPERTY osg_BINDIR_RELEASE ${OSGROOT}/bin)
   set_property(GLOBAL PROPERTY osg_BINDIR_DEBUG ${OSGROOT}/bin)
endif()

if(WIN32)
   set(osg_LIBRARY
      debug ${OSGROOT}/debug/lib/osgd.lib optimized ${OSGROOT}/release/lib/osg.lib
      debug ${OSGROOT}/debug/lib/OpenThreadsd.lib optimized ${OSGROOT}/release/lib/OpenThreads.lib
      debug ${OSGROOT}/debug/lib/osgViewerd.lib optimized ${OSGROOT}/release/lib/osgViewer.lib
      debug ${OSGROOT}/debug/lib/osgTextd.lib optimized ${OSGROOT}/release/lib/osgText.lib
      debug ${OSGROOT}/debug/lib/osgdbd.lib optimized ${OSGROOT}/release/lib/osgdb.lib
      debug ${OSGROOT}/debug/lib/osgUtild.lib optimized ${OSGROOT}/release/lib/osgUtil.lib
      debug ${OSGROOT}/debug/lib/osgGAd.lib optimized ${OSGROOT}/release/lib/osgGA.lib
      debug ${OSGROOT}/debug/lib/osgSimd.lib optimized ${OSGROOT}/release/lib/osgSim.lib
      debug ${OSGROOT}/debug/lib/osgAnimationd.lib optimized ${OSGROOT}/release/lib/osgAnimation.lib
      debug ${OSGROOT}/debug/lib/osgFXd.lib optimized ${OSGROOT}/release/lib/osgFX.lib
     #debug ${OSGROOT}/debug/lib/osgManipulatord.lib optimized ${OSGROOT}/release/lib/osgManipulator.lib
     debug ${OSGROOT}/debug/lib/osgParticled.lib optimized ${OSGROOT}/release/lib/osgParticle.lib
     #debug ${OSGROOT}/debug/lib/osgPresentationd.lib optimized ${OSGROOT}/release/lib/osgPresentation.lib
     #debug ${OSGROOT}/debug/lib/osgShadowd.lib optimized ${OSGROOT}/release/lib/osgShadow.lib
     #debug ${OSGROOT}/debug/lib/osgTerraind.lib optimized ${OSGROOT}/release/lib/osgTerrain.lib
     #debug ${OSGROOT}/debug/lib/osgUId.lib optimized ${OSGROOT}/release/lib/osgUI.lib
     #debug ${OSGROOT}/debug/lib/osgVolumed.lib optimized ${OSGROOT}/release/lib/osgVolume.lib
     #debug ${OSGROOT}/debug/lib/osgWidgetd.lib optimized ${OSGROOT}/release/lib/osgWidget.lib
      )
else()
   set(osg_LIBRARY
      ${OSGROOT}/lib64/libosg.so
      ${OSGROOT}/lib64/libOpenThreads.so
      ${OSGROOT}/lib64/libosgViewer.so
      ${OSGROOT}/lib64/libosgText.so
      ${OSGROOT}/lib64/libosgDB.so
      ${OSGROOT}/lib64/libosgUtil.so
      ${OSGROOT}/lib64/libosgGA.so
      ${OSGROOT}/lib64/libosgSim.so
      ${OSGROOT}/lib64/libosgAnimation.so
      ${OSGROOT}/lib64/libosgTerrain.so
      ${OSGROOT}/lib64/libosgFX.so
      ${OSGROOT}/lib64/libosgShadow.so
      ${OSGROOT}/lib64/libosgVolume.so
      # ${OSGROOT}/lib64/libosgManipulator.so
      ${OSGROOT}/lib64/libosgParticle.so
      # ${OSGROOT}/lib64/libosgPresentation.so
      # ${OSGROOT}/lib64/libosgUI.so
      # ${OSGROOT}/lib64/libosgWidget.so
     )
endif()
