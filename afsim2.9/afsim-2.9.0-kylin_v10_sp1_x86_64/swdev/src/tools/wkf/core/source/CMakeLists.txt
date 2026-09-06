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

# *****************************************************************************
# CMAKE file: wkf
# *****************************************************************************

include(GenerateExportHeader)
project(wkf)

if (MSVC)
   add_compile_options(-wd4251 -wd4286 -wd4275)
endif()

# *****************************************************************************
# Get the source files
file(GLOB SRCS *.hpp *.cpp)
file(GLOB QRC ${CMAKE_CURRENT_SOURCE_DIR}/../ui/resources/*.qrc)
file(GLOB UIS ${CMAKE_CURRENT_SOURCE_DIR}/../ui/*.ui)

# *****************************************************************************
# Acquire and configure Qt package
swdev_acquire_packages("" ${SWDEV_QT_PACKAGE})

include(qt_project)
configure_qt_project(UI UI_HEADERS ${UIS})


# *****************************************************************************
# Plugins
set(PLUGIN_ROOT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins"
                     "${CMAKE_SOURCE_DIR}/../afsim_*shared"
                     "${WSF_ADD_EXTENSION_PATH}")
set(PLUGIN_INSTALL_DIR "${INSTALL_PLUGIN_PATH}/wkf_plugins")
include(vis_plugins)
use_vis_plugins(WKF)

# *****************************************************************************
# enable code timers when configured in CMAKE
if (UTIL_DEV_CODE_TIMERS)
   file(GLOB CT_SRCS ../codetimers/*.cpp ../codetimers/*.hpp)
   set(SRCS ${SRCS} ${CT_SRCS})
   set(WKF_CODETIMER_PATH ../codetimers)
   add_definitions("-DWKFCODETIMERS_AVAILABLE") 
endif()

# *****************************************************************************
# The library we are trying to build
add_library(${PROJECT_NAME} ${SRCS} ${UI_HEADERS} ${QRC})
generate_export_header(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools/wkf")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ${WKF_CODETIMER_PATH} ${PROJECT_BINARY_DIR})
swdev_warning_level(${PROJECT_NAME})

# *****************************************************************************
# Libraries
target_link_libraries(${PROJECT_NAME} vespatk vespatk_qt Qt5::Widgets)
swdev_lib_install(${PROJECT_NAME})

# *****************************************************************************
# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/..)
#Add change log documentation path for all of WKF
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/../..)

# *****************************************************************************
# Installation
install_sources_all_files(../ tools/wkf/core)
