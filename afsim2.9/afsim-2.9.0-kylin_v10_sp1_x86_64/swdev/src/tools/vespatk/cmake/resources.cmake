# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# Acquires resources of general interest to VespaTK applications

if (NOT VTK_RESOURCES_SOURCEDIR)
   # An initial search path is not expected to normally be defined, but
   # provides a hook for a developer or build system to specify alternate
   # layouts without requiring source modification.
   if(NOT VTK_RESOURCES_SEARCH_PATH)
      list(APPEND VTK_RESOURCES_SEARCH_PATH "${CMAKE_SOURCE_DIR}/../vtk_resources")
      if(WIN32)
         list(APPEND VTK_RESOURCES_SEARCH_PATH "//sllabl1/data/resources/${SWDEV_BASE_BRANCH}")
         list(APPEND VTK_RESOURCES_SEARCH_PATH "//homer/l1r/data/resources/${SWDEV_BASE_BRANCH}")
      else()
         list(APPEND VTK_RESOURCES_SEARCH_PATH "/data/resources/${SWDEV_BASE_BRANCH}")
      endif()
   endif()

   foreach(PATHDIR IN LISTS VTK_RESOURCES_SEARCH_PATH)
      if(IS_DIRECTORY "${PATHDIR}")
         get_filename_component(VTK_RESOURCES_SOURCE_FOUND "${PATHDIR}" ABSOLUTE)
         break()
      endif()
   endforeach()

   if(VTK_RESOURCES_SOURCE_FOUND)
      set(VTK_RESOURCES_SOURCEDIR "${VTK_RESOURCES_SOURCE_FOUND}" CACHE PATH "Directory used to acquire VespaTK resources")
   else()
      # If no valid resource path found, set variable value for this execution only.
      # This fallback value is intentionally not cached to attempt re-detection on the next execution.
      get_filename_component(VTK_RESOURCES_SOURCEDIR "${CMAKE_SOURCE_DIR}/../dependencies/resources" ABSOLUTE)
   endif()
endif()

macro(swdev_extract_resources)
   message(STATUS "Unpacking ${RESOURCE_ARCHIVE} to ${VTK_DEV_RESOURCES_PATH}")
   execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${VTK_DEV_RESOURCES_PATH}")
   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${VTK_DEV_RESOURCES_PATH}")
   execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${RESOURCE_ARCHIVE}" WORKING_DIRECTORY "${VTK_DEV_RESOURCES_PATH}")
   file(WRITE ${TIMESTAMP_FILE} "${ARCHIVE_TIMESTAMP}")
endmacro()

macro(swdev_acquire_resources)
   # Check for existing unpacked resource contents using a rough heuristic of
   # expected subdirectories. If an existing resources directory is found,
   # use it as is, assume it's externally managed by git or user.
   if(IS_DIRECTORY "${VTK_RESOURCES_SOURCEDIR}/maps" AND IS_DIRECTORY "${VTK_RESOURCES_SOURCEDIR}/models")
      message(STATUS "Using resource files from directory ${VTK_RESOURCES_SOURCEDIR}")
      # Using VTK_RESOURCES_SOURCEDIR directly, set (but not cache) VTK_DEV_RESOURCES_PATH to use for install
      set(VTK_DEV_RESOURCES_PATH ${VTK_RESOURCES_SOURCEDIR})
   else()
      # Check if a prepackaged resource archive is present
      set(VTK_DEV_RESOURCES_PATH "${CMAKE_SOURCE_DIR}/../resources" CACHE PATH "Destination path to which VespaTk resources will be extracted")
      # Construct default filename for resources archive based on version ("master" for development stream)
      if(${SWDEV_BASE_BRANCH} STREQUAL "master")
         set(RESOURCES_VERSION "master")
      else()
         set(RESOURCES_VERSION "${AFSIM_VERSION_MAJOR}.${AFSIM_VERSION_MINOR}.${AFSIM_VERSION_PATCH}")
      endif()
      set(VTK_RESOURCES_ARCHIVE_FILENAME "vtk_resources-${RESOURCES_VERSION}-noarch.tar.gz" CACHE STRING "Filename of resource archive to use")
      mark_as_advanced(FORCE VTK_RESOURCES_ARCHIVE_FILENAME)
      set(RESOURCE_ARCHIVE ${VTK_RESOURCES_SOURCEDIR}/${VTK_RESOURCES_ARCHIVE_FILENAME})

      if(EXISTS "${RESOURCE_ARCHIVE}")
         file(TIMESTAMP ${RESOURCE_ARCHIVE} ARCHIVE_TIMESTAMP)
         set(TIMESTAMP_FILE ${VTK_DEV_RESOURCES_PATH}/${VTK_RESOURCES_ARCHIVE_FILENAME}.timestamp)
         if (EXISTS ${TIMESTAMP_FILE}) # if we have a timestamp file
            file(READ ${TIMESTAMP_FILE} LOCAL_TIME_STAMP)
            if (NOT ${ARCHIVE_TIMESTAMP} STRGREATER ${LOCAL_TIME_STAMP})
               message(STATUS "Resources are up to date with ${RESOURCE_ARCHIVE}")
            else ()
               message(STATUS "Updating resource files from ${VTK_DEV_RESOURCES_PATH}")
               set(SITE_FOLDER "${VTK_DEV_RESOURCES_PATH}/site")
               if(EXISTS "${SITE_FOLDER}")
                  execute_process(COMMAND ${CMAKE_COMMAND} -E rename "${SITE_FOLDER}" "${VTK_DEV_RESOURCES_PATH}/../vtk_resources-site")
                  swdev_extract_resources()
                  execute_process(COMMAND ${CMAKE_COMMAND} -E rename "${VTK_DEV_RESOURCES_PATH}/../vtk_resources-site" "${SITE_FOLDER}")
               else()
                  swdev_extract_resources()
               endif()
            endif ()
         else ()
            message(STATUS "Acquiring resource files from ${RESOURCE_ARCHIVE}")
            swdev_extract_resources()
         endif ()
      elseif(IS_DIRECTORY "${VTK_DEV_RESOURCES_PATH}/maps" AND IS_DIRECTORY "${VTK_DEV_RESOURCES_PATH}/models")
         message("Could not detect prepackaged resource archive. Using resource files from directory ${VTK_DEV_RESOURCES_PATH}.")
      else ()
         # Generate error by default if resources are requested but not found,
         # but provide a cmake option for developer use to force continuation.
         option(VTK_RESOURCES_CONTINUEIFMISSING
            "Whether CMake should only warn (instead of error) if required resources are not found"
            OFF)
         mark_as_advanced(FORCE VTK_RESOURCES_CONTINUEIFMISSING)
         set(MISSING_RESOURCE_MSG "Could not determine resource location! Expected archive at '${RESOURCE_ARCHIVE}' or contents at '${VTK_RESOURCES_SOURCEDIR}'")
         if (VTK_RESOURCES_CONTINUEIFMISSING)
            message(WARNING ${MISSING_RESOURCE_MSG})
         else()
            message(SEND_ERROR ${MISSING_RESOURCE_MSG})
         endif()
      endif()
   endif ()
endmacro()
