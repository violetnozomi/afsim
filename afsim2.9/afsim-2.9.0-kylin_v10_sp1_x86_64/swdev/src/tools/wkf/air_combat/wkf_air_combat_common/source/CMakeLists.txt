# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# *****************************************************************************
# CMAKE file: wkf_air_combat_common
# *****************************************************************************

include(GenerateExportHeader)
project(wkf_air_combat_common)

# *****************************************************************************
set(COMMON_SUBDIRECTORIES aces air_combat hdd hud sa_display)
foreach(SUB ${COMMON_SUBDIRECTORIES})
   file(GLOB SUB_SRCS ${SUB}/*.?pp)
   set(WKF_AIR_COMBAT_COMMON_SRCS ${WKF_AIR_COMBAT_COMMON_SRCS} ${SUB_SRCS})
   source_group(${SUB} FILES ${SUB_SRCS})
endforeach(SUB ${COMMON_SUBDIRECTORIES})

file(GLOB UIS ${CMAKE_CURRENT_SOURCE_DIR}/../ui/*.ui)

# *****************************************************************************
# Acquire and configure Qt package
swdev_acquire_packages("" ${SWDEV_QT_PACKAGE})

include(qt_project)
configure_qt_project(UI UI_HEADERS ${UIS})

# *****************************************************************************
# The library we are trying to build
add_library(${PROJECT_NAME} ${WKF_AIR_COMBAT_COMMON_SRCS} ${UI_HEADERS})
generate_export_header(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools/wkf/air_combat")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ${PROJECT_BINARY_DIR})
swdev_warning_level(${PROJECT_NAME})

# *****************************************************************************
# Libraries

swdev_package_link_libraries(${PROJECT_NAME} osg)
target_link_libraries(${PROJECT_NAME} wkf wkf_common)
swdev_lib_install(${PROJECT_NAME})

# *****************************************************************************
# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/..)

# *****************************************************************************
# Installation
install_sources_all_files(../ tools/wkf/air_combat/wkf_air_combat_common)
