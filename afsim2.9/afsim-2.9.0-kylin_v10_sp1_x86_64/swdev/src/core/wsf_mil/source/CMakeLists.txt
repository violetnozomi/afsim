# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
include (GenerateExportHeader)
include(swdev_project)

if (MSVC)
   add_compile_options(-wd4251 -wd4286 -wd4275)
endif()

FILE(GLOB SRCS *.?pp)
set(WSF_MIL_COMP_SUB comm ew mover processor observer script sensor weapon)
foreach(SUB ${WSF_MIL_COMP_SUB})
   FILE(GLOB SUB_SRCS  ${SUB}/*.?pp)
   set(SRCS ${SRCS} ${SUB_SRCS})
   source_group(${SUB} FILES ${SUB_SRCS})
   include_directories("${CMAKE_CURRENT_SOURCE_DIR}/${SUB}")
   set(WSF_MIL_COMP_INCLUDES "${WSF_MIL_COMP_INCLUDES}" "${CMAKE_CURRENT_SOURCE_DIR}/${SUB}")
endforeach(SUB ${WSF_MIL_COMP_SUB})
set(WSF_MIL_INCLUDES ${WSF_MIL_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${WSF_MIL_COMP_INCLUDES} ${PROJECT_BINARY_DIR}/source)

#if(WSF_USE_DIS)
   set(WSF_MIL_SUB ${WSF_MIL_SUB} dis)
#endif()
#if(WSF_USE_XIO)
   set(WSF_MIL_SUB ${WSF_MIL_SUB} xio)
#endif()

foreach(SUB ${WSF_MIL_SUB})
   FILE(GLOB SUB_SRCS ${SUB}/*.?pp)
   set(SRCS ${SRCS} ${SUB_SRCS})
   source_group(${SUB} FILES ${SUB_SRCS})
endforeach(SUB ${WSF_MIL_SUB})
FILE(GLOB SUB_SRCS *.?pp)
set(SRCS ${SRCS} ${SUB_SRCS})

include_directories(${PROJECT_BINARY_DIR}/source)

# Add in event pipe
include(${WSF_ROOT}/wsf/event_pipe.cmake)
set(EVENT_PIPE_INCLUDES)
set(EVENT_PIPE_SOURCES)
add_event_pipe_schema(${CMAKE_CURRENT_SOURCE_DIR}/../wsf_mil_events.utpack
                      "${wsf_SOURCE_DIR}/../event_pipe/wsf.utpack"
                      ""
                      "WsfEventPipeClasses.hpp"
                      "WsfEventPipe"
                      "WsfMilEventPipe"
                      EVENT_PIPE_INCLUDES
                      EVENT_PIPE_SOURCES)
set(SRCS ${SRCS} ${EVENT_PIPE_INCLUDES})

wsf_grammar_file(SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../grammar/wsf_mil.ag")

add_library(${PROJECT_NAME} ${SRCS})
swdev_warning_level(${PROJECT_NAME})
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${WSF_MIL_INCLUDES})
target_link_libraries(${PROJECT_NAME} wsf)

# Set the subfolder that the target will appear under in an IDE's solution explorer.
# This isn't actually related to any file system folders.
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "core")

swdev_lib_install(${PROJECT_NAME})
