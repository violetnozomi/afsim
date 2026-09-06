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

include(wsf_data_dirs)

include(ProcessorCount)
ProcessorCount(PROCS)
math(EXPR PROCS "${PROCS}")

set(REGRESSION_LIST_FILE "${CMAKE_CURRENT_LIST_FILE}")

macro(generate_wsf_regression_test TARGET)
   # configured input list file, "-l" option
   foreach(ROOT ${WSF_DEMOS_ROOT} ${WSF_TRAINING_ROOT} ${WSF_SCENARIOS_ROOT})
      if(IS_DIRECTORY "${ROOT}/regression_tests")
         if (EXISTS "${ROOT}/regression_tests/${TARGET}_list.txt")
            set(REGRESSION_LIST_FILES -l ${ROOT}/regression_tests/${TARGET}_list.txt ${REGRESSION_LIST_FILES})
         endif()
      endif()
   endforeach()

   if (REGRESSION_LIST_FILES)
      # configured output dir, "-o" option
      set(WSF_REGRESSION_OUTPUT "${CMAKE_BINARY_DIR}/regression" CACHE PATH "WSF regression test output directory")

      # Use python where available to do regression tests

      # CMake doesn't support env variables inside custom target commands
      # so we use 'set X=A && cmd' on windows and 'X=A cmd' on linux

      if(WIN32)
         set(SETENVCMD set)
         set(SETENVAND &&)
      else()
         set(SETENVCMD)
         set(SETENVAND)
      endif()
      find_package(Python3)
      if (Python3_FOUND)
         set(TESTS_DIR ${${TARGET}_SOURCE_DIR}/tests)
         if(NOT EXISTS ${TESTS_DIR})
            set(TESTS_DIR ${${TARGET}_SOURCE_DIR}/../tests)
         endif()

         add_custom_target(${PROJECT_NAME}_REGRESSION_TEST
                           ${SETENVCMD} WSF_SWDEV_PATH=${WSF_ROOT}${SETENVAND} ${Python3_EXECUTABLE} regression_test.py
                           -j ${PROCS} -x $<TARGET_FILE:${TARGET}> -t ${TARGET} ${REGRESSION_LIST_FILES} -o ${WSF_REGRESSION_OUTPUT}
                           WORKING_DIRECTORY ${TESTS_DIR}
                           DEPENDS ${TARGET}
                           SOURCES ${REGRESSION_LIST_FILE}
                          )
         set_property(TARGET ${PROJECT_NAME}_REGRESSION_TEST PROPERTY FOLDER CMakeRegressionTargets)

         if(NOT WIN32)
            add_custom_target(${PROJECT_NAME}_REGRESSION_TEST_VALGRIND
                              AUTO_TEST_CPU_TIME_LIMIT=600 ${TARGET}_PATH="$<TARGET_FILE:${TARGET}>" ${Python3_EXECUTABLE} regression_test.py
                              -j ${PROCS} -v -x $<TARGET_FILE:${TARGET}> -t ${TARGET} ${REGRESSION_LIST_FILES} -o ${WSF_REGRESSION_OUTPUT} --run-valgrind
                              WORKING_DIRECTORY ${TESTS_DIR}
                              DEPENDS ${TARGET}
                              SOURCES ${REGRESSION_LIST_FILE}
                             )
            set_property(TARGET ${PROJECT_NAME}_REGRESSION_TEST_VALGRIND PROPERTY FOLDER CMakeRegressionTargets)
         endif()
      endif()
   endif()
endmacro()
