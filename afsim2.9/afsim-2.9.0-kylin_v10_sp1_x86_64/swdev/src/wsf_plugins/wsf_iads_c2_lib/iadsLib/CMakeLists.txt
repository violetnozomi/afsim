# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(iadsLib)
include (GenerateExportHeader)
include(swdev_project)

if (MSVC)
   add_compile_options(-wd4251 -wd4286 -wd4275)
endif()

FILE(GLOB SRCS common/*.?pp source/*.cpp include/*/*.hpp)

include_directories(include common)

IF(CMAKE_COMPILER_IS_GNUCXX)
   # This argument causes the linker to exit if the library has unresolved
   # symbols.  Do not disable this option.
   SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
ENDIF()

add_library(${PROJECT_NAME} STATIC ${SRCS})
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
   "${CMAKE_CURRENT_SOURCE_DIR}/common"
   "${CMAKE_CURRENT_SOURCE_DIR}/include"
   "${PROJECT_BINARY_DIR}")
swdev_warning_level(${PROJECT_NAME})
