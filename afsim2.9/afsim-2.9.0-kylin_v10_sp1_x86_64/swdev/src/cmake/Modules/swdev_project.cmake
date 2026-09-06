# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# This file defines the commands and macros that are used
# to support a software build. It is intended to be included
# by the top-level CMakeLists.txt file.

# GCC Compiler version:
#---Obtain the major and minor version of the GNU compiler---
MACRO(get_gcc_version MAJOR_VAR MINOR_VAR PATCH_VAR)
   # cache the results so that we don't have execute gcc for each lookup
  get_property(ALREADY_SET GLOBAL PROPERTY SWDEV_GCC_MAJOR SET)
  IF (NOT ALREADY_SET)
     # As of GCC 7, the -dumpversion option only outputs the main (major) version, so dump the full version.
     # For compatibility use both -dumpfullversion followed by -dumpversion. This results in the first
     # supported dump option being used, and accommodates back to GCC 4.8.5 as well as Clang compilers.
     exec_program(${CMAKE_C_COMPILER} ARGS "-dumpfullversion" "-dumpversion" OUTPUT_VARIABLE _gcc_version_info)
     string(REGEX REPLACE "^([0-9]+).*$"                   "\\1" GCC_MAJOR ${_gcc_version_info})
     string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*$"          "\\1" GCC_MINOR ${_gcc_version_info})
     string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" GCC_PATCH ${_gcc_version_info})

     if(GCC_PATCH MATCHES "\\.+")
       set(GCC_PATCH "")
     endif()
     if(GCC_MINOR MATCHES "\\.+")
       set(GCC_MINOR "")
     endif()
     if(GCC_MAJOR MATCHES "\\.+")
       set(GCC_MAJOR "")
     endif()
     set_property(GLOBAL PROPERTY SWDEV_GCC_MAJOR "${GCC_MAJOR}")
     set_property(GLOBAL PROPERTY SWDEV_GCC_MINOR "${GCC_MINOR}")
     set_property(GLOBAL PROPERTY SWDEV_GCC_PATCH "${GCC_PATCH}")
     message(STATUS "Found GCC. Major version ${GCC_MAJOR}, minor version ${GCC_MINOR}")
  ENDIF()
  get_property(${MAJOR_VAR} GLOBAL PROPERTY "SWDEV_GCC_MAJOR")
  get_property(${MINOR_VAR} GLOBAL PROPERTY "SWDEV_GCC_MINOR")
  get_property(${PATCH_VAR} GLOBAL PROPERTY "SWDEV_GCC_PATCH")
ENDMACRO()

# Define compiler definitions and shared library install locations that we want for every project
if(WIN32)
   if(MSVC)
      add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS")
      add_compile_options(/MP)

      if(PROMOTE_HARDWARE_EXCEPTIONS)
         add_compile_options(/EHa)
      else()
         add_compile_options(/EHsc)
      endif()

      if (MSVC_VERSION GREATER 1900)
         # Acknowledge MSVC 2017 std::tr1 deprecation and silence output warning.
         # Only required for googletest usage, tracked at https://github.com/google/googletest/issues/1111
         add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
      endif()
   endif()
   if(BUILD_SHARED_LIBS)
      add_definitions("-DSWDEV_ALL_USE_DLL")
   endif(BUILD_SHARED_LIBS)
   # On windows, the dll's must reside in the same path as the .exe
   if("${INSTALL_DLL_PATH}" STREQUAL "")
      set(INSTALL_DLL_PATH .)
   endif("${INSTALL_DLL_PATH}" STREQUAL "")
   # Don't use the WinDef.h macros for min and max, conflicts with std::min/max
   add_definitions(-DNOMINMAX)
else(WIN32)
   # On linux, we can use rpath to locate so's in a subdirectory
   if("${INSTALL_DLL_PATH}" STREQUAL "")
      set(INSTALL_DLL_PATH ./lib)
   endif("${INSTALL_DLL_PATH}" STREQUAL "")
   # disable strict aliasing optimization (for UtFunction)
   add_definitions("-fno-strict-aliasing")
   
   if(PROMOTE_HARDWARE_EXCEPTIONS)
      add_compile_options(-fnon-call-exceptions)
   endif()
   
endif(WIN32)

if (PROMOTE_HARDWARE_EXCEPTIONS)
   add_definitions(-DPROMOTE_HARDWARE_EXCEPTIONS)
endif()

if("${INSTALL_EXE_PATH}" STREQUAL "")
   set(INSTALL_EXE_PATH .)
endif("${INSTALL_EXE_PATH}" STREQUAL "")

if("${INSTALL_PLUGIN_PATH}" STREQUAL "")
   set(INSTALL_PLUGIN_PATH .)
endif("${INSTALL_PLUGIN_PATH}" STREQUAL "")

# Setup install of plugin project libraries. Used by extension and sub-project
# CMakeLists.txt files.
macro(swdev_plugin_install TARGET PLUGIN_DIR)
   swdev_lib_install(${TARGET} ${INSTALL_PLUGIN_PATH}/${PLUGIN_DIR})

   # Set RUNPATH for dynamic library loading on Linux
   set_target_properties(${TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN:$ORIGIN/../lib")

   # Move plug-in into subdirectory
   # Required in order to run in debug from Visual Studio
   add_custom_command(TARGET ${TARGET} POST_BUILD
                      COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TARGET}>/${PLUGIN_DIR}" VERBATIM
                      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET}> $<TARGET_FILE_DIR:${TARGET}>/${PLUGIN_DIR} VERBATIM)
endmacro()

# Setup install of project libraries. Used by extension and sub-project
# CMakeLists.txt files.
# swdev_lib_install(TARGET [DESTINATION])
# Optional argument specifies destination directory for RUNTIME and LIBRARY target files (default: ${INSTALL_DLL_PATH})
macro(swdev_lib_install TARGET)
   get_target_property(library_type ${TARGET} TYPE)

   if("${library_type}" STREQUAL "SHARED_LIBRARY")
      set(DEST_DIR ${INSTALL_DLL_PATH})
      if(${ARGC} GREATER 1)
         set(DEST_DIR ${ARGV1})
      endif()
      if(DEFINED INSTALL_LIB_PATH AND NOT "${INSTALL_LIB_PATH}" STREQUAL "")
         install(TARGETS ${TARGET}
                 RUNTIME DESTINATION ${DEST_DIR} COMPONENT Runtime
                 PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                             GROUP_EXECUTE GROUP_READ
                             WORLD_EXECUTE WORLD_READ
                 ARCHIVE DESTINATION ${INSTALL_LIB_PATH} COMPONENT Archive
                 PERMISSIONS OWNER_WRITE OWNER_READ
                             GROUP_READ
                             WORLD_READ
                 LIBRARY DESTINATION ${DEST_DIR} COMPONENT Library
                 PERMISSIONS OWNER_WRITE OWNER_READ
                             GROUP_READ
                             WORLD_READ)
      else()
         install(TARGETS ${TARGET}
                 RUNTIME DESTINATION ${DEST_DIR} COMPONENT Runtime
                 PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                             GROUP_EXECUTE GROUP_READ
                             WORLD_EXECUTE WORLD_READ
                 LIBRARY DESTINATION ${DEST_DIR} COMPONENT Library
                 PERMISSIONS OWNER_WRITE OWNER_READ
                             GROUP_READ
                             WORLD_READ)
      endif()
      set_property(TARGET ${TARGET} PROPERTY INSTALL_RPATH "$ORIGIN")

      # Install PDB file for Debug configuration when using MSVC
      if(MSVC)
         install(FILES $<TARGET_PDB_FILE:${TARGET}> DESTINATION ${DEST_DIR}
                 CONFIGURATIONS Debug RelWithDebInfo COMPONENT Runtime)
      endif()
   endif()
endmacro()

# Add libraries for Windows sockets
macro(link_sockets TARGET)
   if(WIN32)
      target_link_libraries(${TARGET} ws2_32)
   endif(WIN32)
endmacro(link_sockets TARGET)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)
set(SWDEV_THREAD_LIB Threads::Threads)

if(WIN32)
   set(SWDEV_DL_LIB)
else(WIN32)
   set(SWDEV_DL_LIB dl)
endif(WIN32)

# Indicate the stable branch for the purposes of managing configurations
# across releases, e.g. for 3rd party and resource dependencies
set(SWDEV_BASE_BRANCH "release_2p9")

# The add_subdirectory() command can only be used once for a directory
# this wraps add_subdirectory() so that it only executes the first time
# We assume TARGET_NAME will be the same as the bin subdirectory name
macro(try_add_subdirectory DIR TARGET_NAME)
   if(NOT TARGET ${TARGET_NAME})
      add_subdirectory(${DIR} ${TARGET_NAME})
   endif(NOT TARGET ${TARGET_NAME})
endmacro(try_add_subdirectory DIR TARGET_NAME)

# Recurse directory (SRC_DIR) installing that directory with the source files
# matching the pattern under INSTALL_SOURCE_ROOT/TARGET_DIR
macro(install_sources SRC_DIR TARGET_DIR)
   if(NOT "${INSTALL_SOURCE_ROOT}" STREQUAL "")
      install(DIRECTORY ${SRC_DIR} DESTINATION ${INSTALL_SOURCE_ROOT}/${TARGET_DIR} COMPONENT Source
              FILES_MATCHING PATTERN "*.cpp"
                             PATTERN "*.hpp"
                             PATTERN "*.hxx"
                             PATTERN "*.cxx"
                             PATTERN "*.c"
                             PATTERN "*.h"
                             PATTERN "*.txt"
                             PATTERN "*.py"
                             PATTERN "Doxyfile"
                             PATTERN "CMakeLists.txt"
                             PATTERN "Makefile"
                             PATTERN "README"
                             PATTERN "*_module"
                             PATTERN "*_plugin"
                             PATTERN "*.cmake"
                             PATTERN "*.in"
                             PATTERN "*.rc"
                             PATTERN "*.ico"
                             PATTERN "*.png"
                             PATTERN "*.timestamp" EXCLUDE
                             PATTERN ".git"        EXCLUDE
                             PATTERN "__pycache__" EXCLUDE
                             ${ARGN}
             )
   endif()
endmacro(install_sources SRC_DIR TARGET_DIR)

# Recurse directory (SRC_DIR) installing that directory with all files,
# except those excluded, under INSTALL_SOURCE_ROOT/TARGET_DIR
macro(install_sources_all_files SRC_DIR TARGET_DIR)
   if(NOT "${INSTALL_SOURCE_ROOT}" STREQUAL "")
      install(DIRECTORY ${SRC_DIR} DESTINATION ${INSTALL_SOURCE_ROOT}/${TARGET_DIR} COMPONENT Source
              FILES_MATCHING PATTERN "*.*"
                             PATTERN "Makefile"
                             PATTERN "README"
                             PATTERN "*_module"
                             PATTERN "*_plugin"
                             PATTERN ".*.swp"         EXCLUDE
                             PATTERN ".*.swo"         EXCLUDE
                             PATTERN "*.timestamp"    EXCLUDE
                             PATTERN ".git"           EXCLUDE
                             # doesn't work:  PATTERN ".*" EXCLUDE
                             PATTERN "AAA_PACKAGE"    EXCLUDE
                             PATTERN "__pycache__"    EXCLUDE
                             ${ARGN}
             )
   endif()
endmacro()

# Install all files (leading arguments) (SRC_FILES) into the TARGET_DIR (last argument),
# install_source_files(SRC_FILE_1 SRC_FILE_2, ... SRC_FILE_N, TARGET_DIR)
macro(install_source_files)
   if (ARGC LESS 2)
      message(FATAL_ERROR "Incorrect number of arguments for install_source_files in swdev_project.cmake.")
   endif()

   if(NOT "${INSTALL_SOURCE_ROOT}" STREQUAL "")
      set(SRC_FILES "${ARGV}")
      MATH(EXPR TGT_INDEX "${ARGC}-1")
      list(GET SRC_FILES "${TGT_INDEX}" TARGET_DIR)
      list(REMOVE_AT SRC_FILES "${TGT_INDEX}")
      install(FILES ${SRC_FILES} DESTINATION ${INSTALL_SOURCE_ROOT}/${TARGET_DIR} COMPONENT Source)
   endif()
endmacro()

# Recurse directory (SRC_DIR) installing that directory with the source files
# matching the pattern under INSTALL_SOURCE_ROOT/TARGET_DIR
macro(install_tests SRC_DIR TARGET_DIR)
   if(NOT "${INSTALL_SOURCE_ROOT}" STREQUAL "")
      install(DIRECTORY ${SRC_DIR} DESTINATION ${INSTALL_SOURCE_ROOT}/${TARGET_DIR} COMPONENT Source
              USE_SOURCE_PERMISSIONS
              PATTERN ".*"             EXCLUDE
              PATTERN "*.lis"          EXCLUDE
              PATTERN "*.vg.xml"       EXCLUDE # valgrind report files
              PATTERN "vgcore.*"       EXCLUDE # valgrind crash dumps
              PATTERN "*.aer"          EXCLUDE
              PATTERN "*.evt"          EXCLUDE
              PATTERN "*.rep"          EXCLUDE
              PATTERN "*.log"          EXCLUDE
              PATTERN "*.db"           EXCLUDE
              PATTERN "*.tmp"          EXCLUDE
              PATTERN "*.gnu"          EXCLUDE
              PATTERN "*.plt"          EXCLUDE
              PATTERN "*.sed"          EXCLUDE
              PATTERN "__pycache__"    EXCLUDE
              ${ARGN}
             )
   endif()
endmacro(install_tests SRC_DIR TARGET_DIR)

# Take a list of relative or absolute paths and assign to VARNAME the absolute paths
macro(swdev_absolute_paths VARNAME)
   set(${VARNAME})
   foreach(ARG ${ARGN})
      if (IS_ABSOLUTE "${ARG}")
         GET_FILENAME_COMPONENT(ARG_ABS_PATH "${ARG}" ABSOLUTE)
         set(${VARNAME} ${${VARNAME}} "${ARG_ABS_PATH}")
      elseif(EXISTS "${ARG}")
         GET_FILENAME_COMPONENT(ARG_ABS_PATH "${ARG}" ABSOLUTE)
         set(${VARNAME} ${${VARNAME}} "${ARG_ABS_PATH}")
      elseif (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ARG}")
         GET_FILENAME_COMPONENT(ARG_ABS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${ARG}" ABSOLUTE)
         set(${VARNAME} ${${VARNAME}} "${ARG_ABS_PATH}")
      endif()
   endforeach()
endmacro()

# Microsoft's compilers need to use the /bigobj flag for source files containing many symbols
# This macro marks a list of sources as large (> 65,279 symbols.)
# Without it, especially Debug builds in older MSVC versions (2015, 2017), report:
#   error: number of sections exceeded object file format limit: compile with /bigobj
# /bigobj increases .obj file sizes by 2-3% so we use it only where needed.
MACRO(large_source_files)
   # Setting compile flags per file disables unity builds for those files.
   # Fortunately when CMAKE_UNITY_BUILD is enabled, afsim/CMakeLists.txt already
   # adds "/bigobj" to everything, so we can skip adding it again.
   IF(MSVC AND NOT CMAKE_UNITY_BUILD)
      FOREACH(SRC ${ARGV})
         set_source_files_properties(${SRC} PROPERTIES COMPILE_FLAGS "/bigobj")
      ENDFOREACH()
   ENDIF()
ENDMACRO()

# Suffix libraries with a semi-unique ending
MACRO(use_swdev_lib_suffixes)
   IF(WIN32)
      SET(CMAKE_DEBUG_POSTFIX "_d")
   ELSE()
      get_gcc_version(GCC_MAJOR GCC_MINOR GCC_PATCH)
      # 64-bit detection
      SET(pARCH_SUFFIX)
      if(CMAKE_SIZEOF_VOID_P EQUAL 8)
         string(FIND "${CMAKE_EXE_LINKER_FLAGS}" "-m32" IS_32_BIT)
         if ("${IS_32_BIT}" STREQUAL "-1")
            set(pARCH_SUFFIX "m64")
         endif()
      endif()
      SET(CMAKE_DEBUG_POSTFIX "_d_ln${GCC_MAJOR}${pARCH_SUFFIX}")
      SET(CMAKE_RELEASE_POSTFIX "_ln${GCC_MAJOR}${pARCH_SUFFIX}")
   ENDIF()
ENDMACRO()

# Add Microsoft redistributable runtime libs for Windows
MACRO(swdev_install_c_runtime)
   if(MSVC)
      # May need this for mixed plugins using different versions of libraries.
      # https://blog.kitware.com/deploying-on-windows-with-dll-manifest-issues/
      # http://msdn.microsoft.com/en-us/library/aa374224(v=vs.85).aspx
      #if (MSVC)   # Do not generate manifests for the plugins - caused issues loading plugins
      #   set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
      #endif()
      if (MSVC_VERSION GREATER 1800)
         set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
      endif(MSVC_VERSION GREATER 1800)
      set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${INSTALL_DLL_PATH})
      set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS TRUE)
      set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT MSVC_LIBS)
      include(InstallRequiredSystemLibraries)
   endif(MSVC)
ENDMACRO()

# Add executable to CPack list for menu creation with installers
macro(add_to_start_menu TARGET NAME)
   get_target_property(WSF_EXE_NAME ${PROJECT_NAME} OUTPUT_NAME)
   if (${WSF_EXE_NAME} STREQUAL WSF_EXE_NAME-NOTFOUND)
      set(WSF_EXE_NAME ${TARGET})
   endif()

   get_property(EXECUTABLES GLOBAL PROPERTY START_MENU_EXECUTABLES)
   set(EXECUTABLES ${EXECUTABLES} ${WSF_EXE_NAME} ${NAME})
   set_property(GLOBAL PROPERTY START_MENU_EXECUTABLES "${EXECUTABLES}")

   get_property(LINKS GLOBAL PROPERTY DESKTOP_LINKS)
   set(LINKS ${LINKS} ${WSF_EXE_NAME})
   set_property(GLOBAL PROPERTY DESKTOP_LINKS "${LINKS}")
endmacro()

# Sets the compiler warning level for a project
macro(swdev_warning_level TARGET)
   if(WIN32)
      #  C4266 (level 4) 'function': no override available for virtual member function from base 'type'; function is hidden
      # ADD_DEFINITIONS(/W3 /w34706 /w34264 /w34266)
      target_compile_options(${TARGET} PRIVATE "/W3")
   endif()
   if(CMAKE_COMPILER_IS_GNUCXX)
      target_compile_options(${TARGET} PRIVATE "-Wall")
   endif()
endmacro()

# Are we compiling 64 or 32 bit
# usage:   swdev_is_64bit(is64)
#          if (is64) ...
MACRO(swdev_is_64bit OUTVAR)
   SET(${OUTVAR} FALSE)
   IF (CMAKE_CL_64)
      SET(${OUTVAR} TRUE)
   ELSE()
      IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
         STRING(FIND "${CMAKE_EXE_LINKER_FLAGS}" "-m32" IS_32_BIT)
         IF ("${IS_32_BIT}" STREQUAL "-1")
            SET(${OUTVAR} TRUE)
         ENDIF()
      ENDIF()
   ENDIF()
ENDMACRO()

# Generates a bash script to launch an application (TARGET_NAME) from outside of the install directory
function(configure_bash_launcher TARGET_NAME)
   if (NOT WIN32)
      configure_file(${CMAKE_SOURCE_DIR}/cmake/Modules/BashLauncher.sh.in ${CMAKE_BINARY_DIR}/${TARGET_NAME}.sh @ONLY)
      install(FILES ${CMAKE_BINARY_DIR}/${TARGET_NAME}.sh DESTINATION bin COMPONENT Runtime
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
   endif()
endfunction()

# This must be in the top-level CMake file in order for CMake's unit
# testing facilities to be used; e.g. add_test(...).
# See: https://gitlab.kitware.com/cmake/community/-/wikis/doc/ctest/Testing-With-CTest
enable_testing()
