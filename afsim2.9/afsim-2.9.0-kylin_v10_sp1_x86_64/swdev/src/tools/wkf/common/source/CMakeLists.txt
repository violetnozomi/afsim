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
# CMAKE file: wkf_common
# *****************************************************************************

include(GenerateExportHeader)
project(wkf_common)

# *****************************************************************************
set(COMMON_SUBDIRECTORIES annotation comment data_ring event_marker filter_widget interaction logging orbit misc particle_system platform_browser platform_history route satellite_tether selectors sensor_volume statistics visibility visual_effect wsf_draw zone zone_browser)
foreach(SUB ${COMMON_SUBDIRECTORIES})
   file(GLOB SUB_SRCS ${SUB}/*.?pp)
   set(WKF_COMMON_SRCS ${WKF_COMMON_SRCS} ${SUB_SRCS})
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
add_library(${PROJECT_NAME} ${WKF_COMMON_SRCS} ${UI_HEADERS})
generate_export_header(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools/wkf")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ${PROJECT_BINARY_DIR})
swdev_warning_level(${PROJECT_NAME})

# *****************************************************************************
# Libraries

swdev_package_link_libraries(${PROJECT_NAME} osg)
target_link_libraries(${PROJECT_NAME} wkf)
swdev_lib_install(${PROJECT_NAME})
if (NOT WIN32)
   find_package(PROJ REQUIRED)
   target_link_libraries(${PROJECT_NAME} PROJ::proj)
endif()


# *****************************************************************************
# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/..)

# *****************************************************************************
# Installation
install_sources_all_files(../ tools/wkf/common)
