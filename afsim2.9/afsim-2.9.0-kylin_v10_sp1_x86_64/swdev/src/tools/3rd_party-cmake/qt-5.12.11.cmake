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

set(QTROOT ${qt_ROOT_DIR})
string(REGEX REPLACE ".*qt-5.12.11-" "" SUFFIX ${QTROOT})
list(APPEND CMAKE_PREFIX_PATH ${QTROOT})
list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)

set(QTPLUGINPATH "${QTROOT}/plugins")

# Finds the subdirectories of a given directory.
macro(subdir_list result curdir)
   file(GLOB children RELATIVE ${curdir} ${curdir}/*)
   set(dirlist "")
   foreach(child ${children})
      if(IS_DIRECTORY ${curdir}/${child})
         set(dirlist ${dirlist} ${child})
      endif()
   endforeach()
   set(${result} ${dirlist})
endmacro(subdir_list result curdir)

# Installs each folder under Qt "plugins" to the bin folder.
macro(install_qt_platform_dlls LIBROOT)
   subdir_list(PLUGINS_DIRS ${LIBROOT}/plugins)
   foreach (dir ${PLUGINS_DIRS})
      install(DIRECTORY "${LIBROOT}/plugins/${dir}" DESTINATION ${INSTALL_DLL_PATH} COMPONENT Runtime)
   endforeach()
endmacro(install_qt_platform_dlls LIBROOT)

# install a single qt lib
macro(install_qt_lib LIBNAME)
   if(WIN32)
      install(FILES ${QTROOT}/bin/${LIBNAME}.dll DESTINATION "${INSTALL_DLL_PATH}" CONFIGURATIONS Release RelWithDebInfo COMPONENT Runtime)
      install(FILES ${QTROOT}/bin/${LIBNAME}d.dll DESTINATION "${INSTALL_DLL_PATH}" CONFIGURATIONS Debug COMPONENT Runtime)
   else()
      install(FILES
         ${QTROOT}/lib/lib${LIBNAME}.so.5.12.11
         ${QTROOT}/lib/lib${LIBNAME}.so.5.12
         ${QTROOT}/lib/lib${LIBNAME}.so.5
         ${QTROOT}/lib/lib${LIBNAME}.so
         DESTINATION "${INSTALL_DLL_PATH}" COMPONENT Runtime)
   endif()
endmacro()

macro(install_qt_core_libs)
   install_qt_lib(Qt5Core)
endmacro()

# Install a set of qt libs.  This command would install all libs:
#   install_qt_libs(Qt5Concurrent Qt5Core Qt5Gui Qt5Network Qt5OpenGL Qt5PrintSupport Qt5Script Qt5Widgets Qt5DBus)
macro(install_qt_libs)
   foreach (LIBLIST ${ARGN})
      foreach (LIB ${LIBLIST})
         install_qt_lib(${LIB})
      endforeach()
   endforeach()
   if(NOT WIN32)
      # needed for linux
      install_qt_lib(Qt5DBus)
      install_qt_lib(Qt5XcbQpa)
   endif()
endmacro()

macro(install_qt_libs_all)
   install_qt_core_libs()
   install_qt_libs(Qt5Concurrent Qt5Core Qt5Gui Qt5Network Qt5OpenGL Qt5PrintSupport Qt5Script Qt5Widgets)
endmacro()

macro(install_qt_plugins OUTDIR)
   if(WIN32)
      set(FILESUFFIX .dll)
      set(FILEPREFIX)
      set(DBGSUFFIX d)
   else()
      set(FILESUFFIX .so)
      set(FILEPREFIX lib)
      set(DBGSUFFIX)
   endif()

   # Copy plugins
   foreach(DLL qgif;qico;qjpeg;qtga;qtiff;qwbmp)
      if(WIN32)
         install(FILES ${QTROOT}/plugins/imageformats/${FILEPREFIX}${DLL}d${FILESUFFIX} DESTINATION "${OUTDIR}/imageformats" CONFIGURATIONS Debug COMPONENT Runtime)
         install(FILES ${QTROOT}/plugins/imageformats/${FILEPREFIX}${DLL}${FILESUFFIX} DESTINATION "${OUTDIR}/imageformats" CONFIGURATIONS Release RelWithDebInfo COMPONENT Runtime)
      else()
         install(FILES ${QTROOT}/plugins/imageformats/${FILEPREFIX}${DLL}${FILESUFFIX} DESTINATION "${OUTDIR}/imageformats" COMPONENT Runtime)
      endif()
   endforeach()
   foreach(DLL qminimal;qoffscreen;qwindows;qlinuxfb;qxcb)
      if(EXISTS ${QTROOT}/plugins/platforms/${FILEPREFIX}${DLL}${FILESUFFIX})
         if(WIN32)
            install(FILES ${QTROOT}/plugins/platforms/${FILEPREFIX}${DLL}d${FILESUFFIX} DESTINATION "${OUTDIR}/platforms" CONFIGURATIONS Debug COMPONENT Runtime)
            install(FILES ${QTROOT}/plugins/platforms/${FILEPREFIX}${DLL}${FILESUFFIX} DESTINATION "${OUTDIR}/platforms" CONFIGURATIONS Release RelWithDebInfo COMPONENT Runtime)
         else()
            install(FILES ${QTROOT}/plugins/platforms/${FILEPREFIX}${DLL}${FILESUFFIX} DESTINATION "${OUTDIR}/platforms" COMPONENT Runtime)
         endif()
      else()
      endif()
   endforeach()
   foreach(DLL qsqlite;qsqlodbc;qsqlpsql)
      if(EXISTS ${QTROOT}/plugins/sqldrivers/${FILEPREFIX}${DLL}${FILESUFFIX})
         if(WIN32)
            install(FILES ${QTROOT}/plugins/sqldrivers/${FILEPREFIX}${DLL}d${FILESUFFIX} DESTINATION "${OUTDIR}/sqldrivers" CONFIGURATIONS Debug COMPONENT Runtime)
            install(FILES ${QTROOT}/plugins/sqldrivers/${FILEPREFIX}${DLL}${FILESUFFIX} DESTINATION "${OUTDIR}/sqldrivers" CONFIGURATIONS Release RelWithDebInfo COMPONENT Runtime)
         else()
            install(FILES ${QTROOT}/plugins/sqldrivers/${FILEPREFIX}${DLL}${FILESUFFIX} DESTINATION "${OUTDIR}/sqldrivers" COMPONENT Runtime)
         endif()
      endif()
   endforeach()
   if(WIN32)
      foreach(DLL windowsprintersupport)
         if(EXISTS ${QTROOT}/plugins/printsupport/${FILEPREFIX}${DLL}${FILESUFFIX})
            install(FILES ${QTROOT}/plugins/printsupport/${FILEPREFIX}${DLL}d${FILESUFFIX} DESTINATION "${OUTDIR}/printsupport" CONFIGURATIONS Debug COMPONENT Runtime)
            install(FILES ${QTROOT}/plugins/printsupport/${FILEPREFIX}${DLL}${FILESUFFIX} DESTINATION "${OUTDIR}/printsupport" CONFIGURATIONS Release RelWithDebInfo COMPONENT Runtime)
         endif()
      endforeach()
   endif()
   # Needed for Linux
   if(NOT WIN32)
      install(DIRECTORY ${QTROOT}/plugins/ DESTINATION "${OUTDIR}" COMPONENT Runtime)
   endif()
endmacro()

macro(install_qt_licenses)
   file(GLOB LICENSES ${QTROOT}/license*/*.txt)
   install(FILES ${LICENSES} DESTINATION ${INSTALL_EXE_PATH}/../resources/data/3rd_party_licenses PERMISSIONS
           OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Resource)
endmacro()

# These are included backwards, so that /lib shows up first.  This way the .dll's found are next to the .pdb files
set_property(GLOBAL PROPERTY qt_BINDIR_DEBUG ${QTROOT}/bin ${QTROOT}/lib)
set_property(GLOBAL PROPERTY qt_BINDIR_RELEASE ${QTROOT}/bin ${QTROOT}/lib)
