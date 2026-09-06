# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(utilqt)
include(swdev_project)
include(qt_project)
include(GenerateExportHeader)

file(GLOB SRCS *.cpp *.hpp)
file(GLOB QRC ${CMAKE_CURRENT_SOURCE_DIR}/../ui/resources/*.qrc)
file(GLOB UIS ../ui/qt_designer/*.ui)

if (UTIL_DEV_CODE_TIMERS)
   file(GLOB CT_SRCS ../codetimers/*.cpp ../codetimers/*.hpp)
   set(SRCS ${SRCS} ${CT_SRCS})
   set(UTILQT_CODETIMER_PATH ../codetimers)
endif()

if (${CMAKE_BUILD_SHARED_LIBS})
   add_definitions(-DSWDEV_ALL_USE_DLL)
endif (${CMAKE_BUILD_SHARED_LIBS})

set(SWDEV_QT_PACKAGE        "qt-5.12.11"       CACHE STRING "" FORCE)
mark_as_advanced(FORCE SWDEV_QT_PACKAGE)

swdev_acquire_packages("" ${SWDEV_QT_PACKAGE})

configure_qt_project(MODULES OpenGL Network UI UI_HEADERS ${UIS})

add_library(${PROJECT_NAME} ${SRCS} ${QRC} ${UI_HEADERS})

generate_export_header(${PROJECT_NAME})
if (MSVC)
   target_compile_options(${PROJECT_NAME} PUBLIC -wd4251 -wd4286 -wd4275)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "tools")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${UTILQT_CODETIMER_PATH} ${PROJECT_BINARY_DIR})

find_package(OpenGL)

target_link_libraries(${PROJECT_NAME} util ${OPENGL_LIBRARIES} Qt5::Widgets Qt5::OpenGL Qt5::Network)

swdev_lib_install(${PROJECT_NAME})
swdev_warning_level(${PROJECT_NAME})

install_sources_all_files(../ tools/${PROJECT_NAME})
