# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# ****************************************************************************
# Updated by DCS Corporation.
# ****************************************************************************

include(GenerateExportHeader)
include(swdev_project)

# CMake command line/GUI option to include code-timers (a software developer tool)
option(UTIL_DEV_CODE_TIMERS            "Include Code Timers (a software developer's profiling tool)" FALSE)
mark_as_advanced(UTIL_DEV_CODE_TIMERS)

file(GLOB SRCS *.cpp *.hpp)

if (UTIL_DEV_CODE_TIMERS)
   file(GLOB CT_SRCS ../codetimers/*.cpp ../codetimers/*.hpp)
   set(SRCS ${SRCS} ${CT_SRCS})
   set(UTIL_CODETIMER_PATH ../codetimers)
endif()

include_directories(${PROJECT_BINARY_DIR}/source)

# Ut code not currently used.  We may be able to move these to util_legacy or some other project
file(GLOB PURGE_SRCS
      # Replaced by WsfAttributeContainer
      UtAttribute*
      # Unit classes, should be using classes in UtUnitTypes.hpp instead ( they match UtInput )
      UtLength.* UtLLAPos.* UtSpeed.* UtAccel.* UtAngleR.* UtAngleRate.* UtDataSize.*

      UcavUtils.*
      # Compatibility headers, no longer necessary
      default_defs.hpp)
#list(REMOVE_ITEM SRCS ${PURGE_SRCS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/UtBuildInfoMacros.hpp.in ${PROJECT_BINARY_DIR}/source/UtBuildInfoMacros.hpp)
set(SRCS ${SRCS} ${PROJECT_BINARY_DIR}/source/UtBuildInfoMacros.hpp)

IF(CMAKE_COMPILER_IS_GNUCXX)
   # This argument causes the linker to exit if the library has unresolved
   # symbols.  Do not disable this option.
   SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
ENDIF()

# Add native visualizer for Visual Studio debugger
if (MSVC_IDE)
   set(SRCS ${SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/../util.natvis)
endif()

large_source_files(${CMAKE_CURRENT_SOURCE_DIR}/UtTable_RegularTable.cpp)

ADD_LIBRARY(${PROJECT_NAME} ${SRCS})
generate_export_header(${PROJECT_NAME} BASE_NAME ut)
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${UTIL_CODETIMER_PATH} ${PROJECT_BINARY_DIR}/source)
swdev_warning_level(${PROJECT_NAME})
if (MSVC)
   target_compile_options(${PROJECT_NAME} PUBLIC -wd4251 -wd4286 -wd4275)
endif()
target_link_libraries(${PROJECT_NAME} ${SWDEV_THREAD_LIB} ${SWDEV_DL_LIB})
swdev_lib_install(${PROJECT_NAME})
