# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include(swdev_project)

include(ProcessorCount)
ProcessorCount(PROCS)
math(EXPR PROCS "${PROCS}")

set(AUTO_TEST_LIST_FILE "${CMAKE_CURRENT_LIST_FILE}")
set(AUTO_TEST_PY "${CMAKE_CURRENT_LIST_DIR}/../../tools/misc/auto_test.py")

function(generate_wsf_auto_test TARGET)
   # Build a list of directories that include tests for the target
   get_optional_project_tests(${TARGET})

   # Add the current directory along WSF root directory if it also exists.
   if(EXISTS "${WSF_ROOT}/wsf/test_${TARGET}" AND IS_DIRECTORY "${WSF_ROOT}/wsf/test_${TARGET}")
      list(APPEND TESTDIRS "${WSF_ROOT}/wsf/test_${TARGET}")
      list(APPEND TESTNAMES "wsf")
   endif()

   # Define auto test targets
   find_package(Python3)
   if (Python3_FOUND)
      set(TESTS_DIR ${${TARGET}_SOURCE_DIR}/tests)
      if(NOT EXISTS ${TESTS_DIR})
         set(TESTS_DIR ${${TARGET}_SOURCE_DIR}/../tests)
      endif()
      
      set(PATTERN "test_*.txt")

      # Make sure to capture any non-plugin tests
      set(GLOBS "${TESTS_DIR}/${PATTERN}" "${TESTS_DIR}/auto_*/${PATTERN}")
      set(WSF_TESTDIRS "-d" "${TESTS_DIR}" "-d" "${TESTS_DIR}/auto_*")
      set(TARGET_DEPS "${TARGET}")
      foreach(testName ${TESTNAMES})
         list(FIND TESTNAMES "${testName}" index)
         list(GET TESTDIRS "${index}" testDir)

         # Grab all the tests so they can be put in the target sources
         list(APPEND GLOBS "${testDir}/${PATTERN}")
         file(GLOB_RECURSE TEST_FILES "${testDir}/${PATTERN}")

         #Make sure this dep gets built before testing.
         set(deps ${TARGET})
         if(TARGET "${testName}")
            list(APPEND deps "${testName}")
            list(APPEND TARGET_DEPS "${testName}")
         endif()

         add_custom_target(${testName}_${TARGET}_AUTO_TEST
            COMMAND ${CMAKE_COMMAND} -E env "WSF_SWDEV_PATH=${WSF_ROOT}" "${Python3_EXECUTABLE}" ${AUTO_TEST_PY} -d "${testDir}" -j "${PROCS}" -v -x $<TARGET_FILE:${TARGET}>
            DEPENDS ${deps}
            SOURCES "${AUTO_TEST_LIST_FILE}" ${TEST_FILES}
         )
         set_property(TARGET "${testName}_${TARGET}_AUTO_TEST" PROPERTY FOLDER CMakeAutoTestTargets/${TARGET})


         # Concat paths to form the testdir for the big test target.
         list(APPEND WSF_TESTDIRS "-d" "${testDir}")
      endforeach()

      file(GLOB_RECURSE TEST_FILES ${GLOBS})

      add_custom_target(${TARGET}_AUTO_TEST
         ${CMAKE_COMMAND} -E env "WSF_SWDEV_PATH=${WSF_ROOT}" ${Python3_EXECUTABLE} ${AUTO_TEST_PY} ${WSF_TESTDIRS} -j ${PROCS} -x $<TARGET_FILE:${TARGET}>
         DEPENDS "${TARGET_DEPS}"
         SOURCES "${AUTO_TEST_LIST_FILE}" ${TEST_FILES}
      )
      set_property(TARGET "${TARGET}_AUTO_TEST" PROPERTY FOLDER CMakeAutoTestTargets)

      if(NOT WIN32)
         add_custom_target(${TARGET}_AUTO_TEST_VALGRIND
            AUTO_TEST_CPU_TIME_LIMIT=600 ${TARGET}_PATH="$<TARGET_FILE:${TARGET}>" WSF_SWDEV_PATH=${WSF_ROOT} ${Python3_EXECUTABLE} ${AUTO_TEST_PY} ${WSF_TESTDIRS} -j ${PROCS} -v -x $<TARGET_FILE:${TARGET}> --run-valgrind
            DEPENDS "${TARGET}"
            SOURCES "${AUTO_TEST_LIST_FILE}" ${TEST_FILES}
         )
         set_property(TARGET "${TARGET}_AUTO_TEST_VALGRIND" PROPERTY FOLDER CMakeAutoTestTargets)
      endif()
   endif()
endfunction()
