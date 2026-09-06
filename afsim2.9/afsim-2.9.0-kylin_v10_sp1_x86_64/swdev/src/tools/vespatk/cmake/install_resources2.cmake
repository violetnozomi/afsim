# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# Acquires resources of general interest to VespaTK applications

if (NOT SWDEV_RESOURCE_SOURCES)
   if(WIN32)
      if(IS_DIRECTORY "//sllabl1/data/resources")
         set(SWDEV_RESOURCE_SOURCES "//sllabl1/data/resources" CACHE PATH "")
      elseif(IS_DIRECTORY "//homer/l1r/data/resources")
         set(SWDEV_RESOURCE_SOURCES "//homer/l1r/data/resources" CACHE PATH "")
      endif()
   else()
      set(SWDEV_RESOURCE_SOURCES "/data/resources" CACHE PATH "")
   endif()
   mark_as_advanced(FORCE SWDEV_RESOURCE_SOURCES)
endif()

set(RESOURCE_FILE ${SWDEV_RESOURCE_SOURCES}/vtk_resources.tar.gz)
if(EXISTS "${RESOURCE_FILE}") # if we can find a "server-side" resource file
   file(TIMESTAMP ${RESOURCE_FILE} SERVER_TIMESTAMP)
   set(TIMESTAMP_FILE ${CMAKE_INSTALL_PREFIX}/resources/vtk_resources.tar.gz.timestamp)
   if (EXISTS ${TIMESTAMP_FILE}) # if we have a timestamp file
      file(READ ${TIMESTAMP_FILE} LOCAL_TIME_STAMP)
      if (NOT ${SERVER_TIMESTAMP} STRGREATER ${LOCAL_TIME_STAMP})
         message(STATUS "Resources are up to date ${CMAKE_INSTALL_PREFIX}")
      else ()
         set(SITE_MOVE 0)
         set(SITE_FOLDER "${CMAKE_INSTALL_PREFIX}/resources/site")
         if(EXISTS "${SITE_FOLDER}")
            execute_process(COMMAND ${CMAKE_COMMAND} -E rename "${SITE_FOLDER}" "${CMAKE_INSTALL_PREFIX}/site")
            set(SITE_MOVE 1)
         endif()
         message(STATUS "Updated resources found")
         execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_INSTALL_PREFIX}/resources")
         execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${RESOURCE_FILE}" "${CMAKE_INSTALL_PREFIX}/vtk_resources.tar.gz")
         message(STATUS "Unpacking vtk_resources.tar.gz")
         execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_PREFIX}/resources")
         execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_INSTALL_PREFIX}/vtk_resources.tar.gz" WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/resources")
         execute_process(COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_INSTALL_PREFIX}/vtk_resources.tar.gz")
         file(WRITE ${TIMESTAMP_FILE} "${SERVER_TIMESTAMP}")
         if (SITE_MOVE)
            execute_process(COMMAND ${CMAKE_COMMAND} -E rename "${CMAKE_INSTALL_PREFIX}/site" "${SITE_FOLDER}")
         endif()
      endif ()
   else ()
      message(STATUS "Acquiring resource files")
      execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_INSTALL_PREFIX}/resources")
      execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${RESOURCE_FILE}" "${CMAKE_INSTALL_PREFIX}/vtk_resources.tar.gz")
      message(STATUS "Unpacking vtk_resources.tar.gz")
      execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_INSTALL_PREFIX}/resources")
      execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_INSTALL_PREFIX}/vtk_resources.tar.gz" WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/resources")
      execute_process(COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_INSTALL_PREFIX}/vtk_resources.tar.gz")
      file(WRITE ${TIMESTAMP_FILE} "${SERVER_TIMESTAMP}")
   endif ()
else ()
   message(ERROR " Could not find resource file: ${RESOURCE_FILE}!")
endif ()
