# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(vespatk_qt)
include(swdev_project)
include(qt_project)
include(GenerateExportHeader)

FILE(GLOB SRCS *.cpp *.hpp)

configure_qt_project()

set(SWDEV_QT_PACKAGE "qt-5.12.11" CACHE STRING "" FORCE)
mark_as_advanced(FORCE SWDEV_QT_PACKAGE)

swdev_acquire_packages("" ${SWDEV_QT_PACKAGE})

add_library(${PROJECT_NAME} ${SRCS})

swdev_package_include_directories(${PROJECT_NAME} osg)

generate_export_header(${PROJECT_NAME})
if (MSVC)
   target_compile_options(${PROJECT_NAME} PUBLIC -wd4251 -wd4286 -wd4275)
endif()
swdev_warning_level(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
swdev_package_link_libraries(${PROJECT_NAME} osg)
target_link_libraries(${PROJECT_NAME} vespatk utilqt)
swdev_lib_install(${PROJECT_NAME})
install_sources_all_files(../ tools/vespatk/${PROJECT_NAME})
