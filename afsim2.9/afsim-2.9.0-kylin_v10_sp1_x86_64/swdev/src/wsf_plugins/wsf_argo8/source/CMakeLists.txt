# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
include (GenerateExportHeader)
include(swdev_project)

FILE(GLOB SRCS *.?pp)

wsf_grammar_file(SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../grammar/wsf_argo8.ag")

include_directories(${PROJECT_BINARY_DIR}/source)

add_library(${PROJECT_NAME} ${SRCS})
generate_export_header(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${WSF_LIBS} wsf_mil argo8)

swdev_warning_level(${PROJECT_NAME})

if(WSF_PLUGIN_BUILD)
   swdev_plugin_install(${PROJECT_NAME} wsf_plugins)
else()
   swdev_lib_install(${PROJECT_NAME})
endif()
