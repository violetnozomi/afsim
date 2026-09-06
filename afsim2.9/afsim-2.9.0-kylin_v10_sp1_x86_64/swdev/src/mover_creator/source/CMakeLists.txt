# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

IF (NOT BUILD_WITH_wsf_mil OR NOT BUILD_WITH_wsf_p6dof OR NOT BUILD_WITH_wsf_six_dof OR NOT BUILD_WITH_wsf_brawler)
   message(SEND_ERROR "Cannot build mover_creator: requires wsf_mil, wsf_p6dof, wsf_six_dof, and wsf_brawler.")
ENDIF()

project(mover_creator)
include(swdev_project)

if (MSVC)
   add_compile_options(-WX)
endif()

# *****************************************************************************
# Get the source files
file(GLOB SRCS *.hpp *.cpp)
file(GLOB UIS ${CMAKE_CURRENT_SOURCE_DIR}/../ui/*.ui)
file(GLOB QRC ${CMAKE_CURRENT_SOURCE_DIR}/../ui/resources/*.qrc)

# *****************************************************************************
# Configure the 3rd_party libraries

set(SWDEV_QT_PACKAGE "qt-5.12.11" CACHE STRING "" FORCE)
mark_as_advanced(FORCE SWDEV_QT_PACKAGE)
swdev_acquire_packages(${CMAKE_CURRENT_SOURCE_DIR}
                       ${SWDEV_QT_PACKAGE})

try_add_subdirectory(${TOOLS_DIRECTORY}/utilqt/source utilqt)

# *****************************************************************************
# Include macros and configuration to support builds of Qt projects
include(qt_project)
configure_qt_project(UI UI_HEADERS ${UIS})

# *****************************************************************************
# The executable we are trying to build
add_executable(${PROJECT_NAME} ${SRCS} ${UI_HEADERS} ${QRC})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "applications")

# *****************************************************************************
# Libraries
target_link_libraries(${PROJECT_NAME} util utilqt p6dof wsf_six_dof wsf_brawler)

link_wsf_optional_projects(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/lib")

configure_qt_conf(${PROJECT_NAME})
configure_bash_launcher(${PROJECT_NAME})

swdev_warning_level(${PROJECT_NAME})

if (MSVC)
   include(user_file_config)
   create_vs_debug_env(${CMAKE_CURRENT_SOURCE_DIR}
                       "${TEMPLATE_DIR}/Template.VisualStudio.Settings.user"
                      )
endif()

if(COMMAND add_to_start_menu)
   add_to_start_menu(${PROJECT_NAME} "Mover Creator")
endif()

# *****************************************************************************
# Configure version header and resource files
include(GenerateVersionInfo)
generate_version_header(${PROJECT_NAME})
if(MSVC)
   generate_resource(${PROJECT_NAME}  "../ui/resources/icons/mc_icon.ico")
endif()

# *****************************************************************************
# Installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_EXE_PATH} COMPONENT Runtime)

# Install required qt core libs
install_qt_libs_all()
install_qt_plugins(${INSTALL_EXE_PATH}/qt_plugins)
install_qt_licenses()

# install 3rd_party-cmake files that are required to build mover creator not covered elsewhere
install_source_files(${TOOLS_DIRECTORY}/3rd_party-cmake/config.cmake
                     ${TOOLS_DIRECTORY}/3rd_party-cmake/shared.cmake
                     ${TOOLS_DIRECTORY}/3rd_party-cmake/${SWDEV_QT_PACKAGE}.cmake
                     tools/3rd_party-cmake
                    )

install_sources_all_files(../ ${PROJECT_NAME})
