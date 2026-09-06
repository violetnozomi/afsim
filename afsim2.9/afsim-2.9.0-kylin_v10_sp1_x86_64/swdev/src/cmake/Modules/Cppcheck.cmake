# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include(ProcessorCount)
ProcessorCount(PROCS)
math(EXPR PROCS "${PROCS}")

if(MSVC)
   set (CPPCHECK_HINT "C:\\Program Files\\Cppcheck")
   set(PROJECT_FILE "${PROJECT_NAME}.sln")
   if(CMAKE_CL_64)
      set(CPPCHECK_PLATFORM win64)
   else()
      set(CPPCHECK_PLATFORM win32W)
   endif()
else()
   set(PROJECT_FILE "compile_commands.json")
   set(CPPCHECK_PLATFORM unix64)
endif()

find_program(CPPCHECK_EXECUTABLE cppcheck HINTS ${CPPCHECK_HINT})
mark_as_advanced(CPPCHECK_EXECUTABLE)

set(CPPCHECK_OUTPUT_FLAGS "--suppress=*:*3rd_party*" "--xml" "--xml-version=2" CACHE STRING
    "Cppcheck output options, e.g. --template=gcc or --template=vs for display in an IDE")
mark_as_advanced(CPPCHECK_OUTPUT_FLAGS)

if(CPPCHECK_EXECUTABLE)
   execute_process(COMMAND "${CPPCHECK_EXECUTABLE}" --version
                   OUTPUT_VARIABLE CPPCHECK_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
   string(REGEX REPLACE ".* ([0-9]\\.([0-9]\\.[0-9])?)" "\\1"
          CPPCHECK_VERSION "${CPPCHECK_VERSION}")
   message(STATUS "cppcheck version: ${CPPCHECK_VERSION}")

   add_custom_target(CPPCHECK COMMAND ${CPPCHECK_EXECUTABLE}
                     -j ${PROCS}
                     --project=${PROJECT_FILE}
                     --platform=${CPPCHECK_PLATFORM}
                     --std=c++11 --std=c11 --std=posix
                     --max-configs=1
                     --enable=warning,performance,portability,missingInclude
                     --inline-suppr
                     --inconclusive
                     ${CPPCHECK_OUTPUT_FLAGS}
                     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                     SOURCES ${CMAKE_CURRENT_LIST_FILE}
                    )
   set_property(TARGET CPPCHECK PROPERTY FOLDER CMakeTargets)
endif()
