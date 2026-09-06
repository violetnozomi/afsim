# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

include(GenerateExportHeader)
include(swdev_project)

if (MSVC)
   add_compile_options(-wd4251 -wd4286 -wd4275)
endif()

FILE(GLOB SRCS *.cpp *.hpp)

add_library(${PROJECT_NAME} ${SRCS})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/source)
target_link_libraries(${PROJECT_NAME} util genio)
generate_export_header(${PROJECT_NAME})
if (MSVC)
   target_compile_options(${PROJECT_NAME} PUBLIC -wd4251 -wd4286 -wd4275)
endif()
swdev_lib_install(${PROJECT_NAME})
swdev_warning_level(${PROJECT_NAME})
