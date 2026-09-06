# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# ****************************************************************************
# Updated by Infoscitex, a DCS Company.
# ****************************************************************************
include(GenerateExportHeader)

SET(UT_SCRIPT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

if(MSVC)
   add_compile_options(-wd4251 -wd4286 -wd4275)
endif()

file(GLOB SRCS *.cpp *.hpp)

add_library(${PROJECT_NAME} ${SRCS})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

if(BUILD_SHARED_LIBS)
  add_definitions(-DUT_SCRIPT_BUILD_DLL)
endif(BUILD_SHARED_LIBS)

swdev_warning_level(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/source)
target_link_libraries(${PROJECT_NAME} util ${SWDEV_THREAD_LIB} ${SWDEV_DL_LIB})
generate_export_header(${PROJECT_NAME})
if(MSVC)
   target_compile_options(${PROJECT_NAME} PUBLIC -wd4251 -wd4286 -wd4275)
endif()
swdev_lib_install(${PROJECT_NAME})
