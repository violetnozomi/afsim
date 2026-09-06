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
# CMAKE file: evt_reader
# *****************************************************************************
project(evt_reader)
if (MSVC)
   add_compile_options(-wd4251 -wd4286 -wd4275)
endif()

# *****************************************************************************
# Get the source files
file(GLOB SRCS *.hpp *.cpp *.rc)

# *****************************************************************************
# Configure the 3rd_party libraries

set(SWDEV_QT_PACKAGE        "qt-5.12.11"       CACHE STRING "" FORCE)

mark_as_advanced(FORCE SWDEV_QT_PACKAGE)

swdev_acquire_packages(${CMAKE_CURRENT_SOURCE_DIR}
                       ${SWDEV_QT_PACKAGE})

# *****************************************************************************
# The executable we are trying to build

try_add_subdirectory(${TOOLS_DIRECTORY}/utilqt/source utilqt)
add_executable(${PROJECT_NAME} WIN32 ${SRCS})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "applications")
configure_qt_conf(${PROJECT_NAME})
configure_bash_launcher(${PROJECT_NAME})

###############################################################################
# Configure version header and resource files
include(GenerateVersionInfo)
generate_version_header(evt_reader)
if(MSVC)
   generate_resource(evt_reader)
endif()

# *****************************************************************************
# Libraries
target_link_libraries(${PROJECT_NAME} utilqt)

set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/lib")

# Required in order to run in debug from Visual Studio
if (MSVC)
   include(user_file_config)
   create_vs_debug_env(
     ${CMAKE_CURRENT_SOURCE_DIR}
     "${TEMPLATE_DIR}/Template.VisualStudio.Settings.user"
    )
endif()

# *****************************************************************************
# Add project to Sphinx for documentation
add_wsf_doc_input(${CMAKE_CURRENT_SOURCE_DIR}/../doc)
add_wsf_doc_sidebar_ext(${CMAKE_CURRENT_SOURCE_DIR}/../wsftheme/wsf_sidebar_evt_reader.html)

# *****************************************************************************
# Installation


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_EXE_PATH} COMPONENT Runtime)

# Install required qt core libs
install_qt_libs_all()
install_qt_plugins(${INSTALL_EXE_PATH}/qt_plugins)
install_qt_licenses()

install_sources_all_files(../ ${PROJECT_NAME})

# install 3rd_party-cmake files that are required to build results_vis not covered elsewhere
install_source_files(${TOOLS_DIRECTORY}/3rd_party-cmake/config.cmake
                     ${TOOLS_DIRECTORY}/3rd_party-cmake/shared.cmake
                     tools/3rd_party-cmake)
if(WIN32)
   install_source_files(${TOOLS_DIRECTORY}/3rd_party-cmake/${SWDEV_QT_PACKAGE}.cmake
                        tools/3rd_party-cmake)
endif()
