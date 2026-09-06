# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
if(NOT DEFINED CMAKE_THIRD_PARTY_ROOT)
   get_filename_component(CMAKE_THIRD_PARTY_ROOT ${CMAKE_CURRENT_LIST_DIR}/../ PATH)
endif()

if(MSVC)
   # https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html
   if(MSVC_VERSION GREATER 1900)
      set(SWDEV_SYS_SUFFIX vs2017)
   elseif(MSVC_VERSION EQUAL 1900)
      set(SWDEV_SYS_SUFFIX vs2015)
   elseif(MSVC_VERSION EQUAL 1800)
      set(SWDEV_SYS_SUFFIX vs2013)
   elseif(MSVC_VERSION EQUAL 1700)
      set(SWDEV_SYS_SUFFIX vs2012)
   elseif(MSVC_VERSION EQUAL 1600)
      set(SWDEV_SYS_SUFFIX vs2010)
   else()
      message(WARNING "${MSVC_VERSION} not recognized for 3rd party packages.")
   endif()
else(MSVC)
   set(SWDEV_SYS_SUFFIX ln4)
endif(MSVC)

# 64 bit compiler detection...
set(ARCH_64 FALSE)
set(ARCH_SUFFIX "x86-")
if(CMAKE_CL_64)
   set(ARCH_64 TRUE)
   set(ARCH_SUFFIX "x64-")
endif()

# 64-bit Linux: check if compiling as 32-bit.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
   if(NOT DEFINED WIN32)
      string(FIND "${CMAKE_CXX_FLAGS}${CMAKE_EXE_LINKER_FLAGS}${CMAKE_C_FLAGS}" "-m32" IS_32_BIT)
      if ("${IS_32_BIT}" STREQUAL "-1")
         set(ARCH_64 TRUE)
         set(ARCH_SUFFIX "x64-")
      endif()
   endif()
endif()

set(SWDEV_LIB_SUFFIX "${ARCH_SUFFIX}${SWDEV_SYS_SUFFIX}")

string(TOLOWER "${CMAKE_BUILD_TYPE}" SWDEV_THIRD_PARTY_BUILD_TYPE)
if("${SWDEV_THIRD_PARTY_BUILD_TYPE}" STREQUAL "")
set(SWDEV_THIRD_PARTY_BUILD_TYPE debug)
endif("${SWDEV_THIRD_PARTY_BUILD_TYPE}" STREQUAL "")

# Create a custom command to copy a file
macro(copy_file_build_rule BINFILES SRC DESTDIR)
   get_filename_component(FILE_NAME "${SRC}" NAME)
   set(DEST_FILE "${DESTDIR}/${FILE_NAME}")
   add_custom_command(OUTPUT "${DESTDIR}/${FILE_NAME}"
      COMMAND ${CMAKE_COMMAND} -E make_directory ${DESTDIR}
      COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DESTDIR}
      DEPENDS ${SRC}
      )
   list(APPEND ${BINFILES} ${DEST_FILE})
endmacro()

# Generate rules to copy all dll files from a 3rd party
# library to the output directory.  Allows running from visual studio.
macro(copy_third_party_dlls_to_bin BINFILES LIBROOT)
   if(WIN32)
      # Note, cmake has no way to enable a file on a per-configuration basis
      # so both debug and release builds will copy all 3rd party binaries
      file(GLOB DLLS ${LIBROOT}/debug/*.dll ${LIBROOT}/debug/bin/*.dll ${LIBROOT}/debug/*.pdb)
      set(outDir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
      foreach(DLL ${DLLS})
         copy_file_build_rule(${BINFILES} ${DLL} ${outDir})
      endforeach(DLL ${DLLS})
      file(GLOB DLLS ${LIBROOT}/release/*.dll ${LIBROOT}/release/bin/*.dll)
      set(outDir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
      foreach(DLL ${DLLS})
         copy_file_build_rule(${BINFILES} ${DLL} ${outDir})
      endforeach(DLL ${DLLS})
   else(WIN32)
   endif(WIN32)
endmacro(copy_third_party_dlls_to_bin BINFILES LIBROOT)

macro(copy_third_party_dlls_to_bin_ex BINFILES DLL_VAR)
   if(WIN32)
      set(outDir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
      foreach(DLL ${${DLL_VAR}_Debug})
         copy_file_build_rule(${BINFILES} ${DLL} ${outDir})
      endforeach()
      set(outDir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
      foreach(DLL ${${DLL_VAR}_Release})
         copy_file_build_rule(${BINFILES} ${DLL} ${outDir})
      endforeach()
   else(WIN32)
      # Not necessary
   endif(WIN32)
endmacro()

macro(install_third_party LIBROOT)
   if(WIN32)
      file(GLOB REL_DLLS ${LIBROOT}/release/*.dll ${LIBROOT}/release/bin/*.dll)
      file(GLOB DBG_DLLS ${LIBROOT}/debug/*.dll ${LIBROOT}/debug/bin/*.dll)
   else(WIN32)
      file(GLOB REL_DLLS ${LIBROOT}/lib*/*.so* ${LIBROOT}/release/*.so* ${LIBROOT}/release/bin/*.so* ${LIBROOT}/lib/*.so* ${LIBROOT}/release/lib/*.so* ${LIBROOT}/release/lib64/*.so*)
      file(GLOB DBG_DLLS ${LIBROOT}/lib*/*.so* ${LIBROOT}/debug/*.so* ${LIBROOT}/debug/bin/*.so* ${LIBROOT}/lib/*.so* ${LIBROOT}/debug/lib/*.so* ${LIBROOT}/debug/lib64/*.so*)
   endif(WIN32)
   file(GLOB LICENSES ${LIBROOT}/license*/*.txt)
   install(FILES ${REL_DLLS} DESTINATION ${INSTALL_DLL_PATH} CONFIGURATIONS Release RelWithDebInfo PERMISSIONS
          OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
   install(FILES ${DBG_DLLS} DESTINATION ${INSTALL_DLL_PATH} CONFIGURATIONS Debug PERMISSIONS
          OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
   install(FILES ${LICENSES} DESTINATION ${INSTALL_EXE_PATH}/../resources/data/3rd_party_licenses PERMISSIONS
          OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Resource)
   FOREACH (SD ${ARGN})
      if(WIN32)
         file(GLOB REL_SD_DLLS ${LIBROOT}/release/${SD}/*.dll ${LIBROOT}/release/bin/${SD}/*.dll)
         file(GLOB DBG_SD_DLLS ${LIBROOT}/debug/${SD}/*.dll ${LIBROOT}/debug/bin/${SD}/*.dll)
      else(WIN32)
         file(GLOB REL_SD_DLLS ${LIBROOT}/lib*/${SD}/*.so* ${LIBROOT}/release/${SD}/*.so* ${LIBROOT}/release/bin/${SD}/*.so*)
         file(GLOB DBG_SD_DLLS ${LIBROOT}/lib*/${SD}/*.so* ${LIBROOT}/debug/${SD}/*.so* ${LIBROOT}/debug/bin/${SD}/*.so*)
      endif(WIN32)
      install(FILES ${REL_SD_DLLS} DESTINATION ${INSTALL_DLL_PATH}/${SD} CONFIGURATIONS Release RelWithDebInfo PERMISSIONS
              OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
      install(FILES ${DBG_SD_DLLS} DESTINATION ${INSTALL_DLL_PATH}/${SD} CONFIGURATIONS Debug PERMISSIONS
              OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT Runtime)
   ENDFOREACH(SD)
endmacro(install_third_party LIBROOT)
