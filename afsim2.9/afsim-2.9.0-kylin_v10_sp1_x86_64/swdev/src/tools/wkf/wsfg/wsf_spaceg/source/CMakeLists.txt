# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# *****************************************************************************
# CMAKE file: wsf_spaceg
# *****************************************************************************

include(GenerateExportHeader)
project(wsf_spaceg)

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
# The library we are trying to build
add_library(${PROJECT_NAME} ${SRCS} ${UI_HEADERS} ${QRC})
generate_export_header(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools/wkf/wsfg")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ${PROJECT_BINARY_DIR})
swdev_warning_level(${PROJECT_NAME})

# *****************************************************************************
# Libraries
target_link_libraries(${PROJECT_NAME} wkf wkf_common wsf_space)
swdev_lib_install(${PROJECT_NAME})

# *****************************************************************************
# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/..)

# *****************************************************************************
# Installation
install_sources_all_files(../ tools/wkf/wsfg/wsf_spaceg)
