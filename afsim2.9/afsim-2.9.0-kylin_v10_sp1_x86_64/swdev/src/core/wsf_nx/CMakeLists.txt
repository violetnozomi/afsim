# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(wsf_nx)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

set(WSF_NX_SUB ew sensor processor)
foreach(SUB ${WSF_NX_SUB})
   FILE(GLOB SUB_HDRS source/${SUB}/*.hpp)
   FILE(GLOB SUB_SRCS source/${SUB}/*.cpp)
   list(APPEND HDRS ${SUB_HDRS})
   list(APPEND SRCS ${SUB_SRCS})
   source_group(${SUB} FILES ${SUB_HDRS} ${SUB_SRCS})
endforeach(SUB ${WSF_NX_SUB})

wsf_project_template(${PROJECT_NAME} LIBRARY
HDRS
   ${HDRS}
SRCS
   ${SRCS}
GRAMMARS
   grammar/wsf_nx.ag
NOTICES
   NO_EXPORT.md
   README.md
)

target_link_libraries(${PROJECT_NAME} wsf_mil)

foreach(SUB ${WSF_NX_SUB})
   target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source/${SUB})
   add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/source/${SUB})
endforeach(SUB ${WSF_NX_SUB})

install_tests(test_sensor_plot ${_project_install_dir})

if(WSF_INSTALL_DEMOS)
   # Demo directories included with the non-exportable build
   set(WSF_NX_DEMO_DIRS
       alv_routing
       base_types_nx
       chaff
       exchange_proc
       suppressor
      )
   install_wsf_data("${WSF_NX_DEMO_DIRS}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
