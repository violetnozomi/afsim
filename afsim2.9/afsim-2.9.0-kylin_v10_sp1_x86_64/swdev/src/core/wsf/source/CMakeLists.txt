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

# Add all component sub directories to the SRCS variable and other variables
# that will be referred to by other projects
set(WSF_COMP_SUB comm event_pipe mover observer processor sensor)
foreach(SUB ${WSF_COMP_SUB})
   file(GLOB SUB_SRCS ${SUB}/*.?pp)
   set(SRCS ${SRCS} ${SUB_SRCS})
   source_group(${SUB} FILES ${SUB_SRCS})
   set(WSF_COMP_INCLUDES "${WSF_COMP_INCLUDES}" ${CMAKE_CURRENT_SOURCE_DIR}/${SUB})
   add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/${SUB})
endforeach()

# Add all other sub directories to the SRCS variable and other variables
# that will be referred to by other projects
set(WSF_SUB dis ext script traffic xio xio_sim)
foreach(SUB ${WSF_SUB})
   file(GLOB SUB_SRCS ${SUB}/*.?pp)
   set(SRCS ${SRCS} ${SUB_SRCS})
   source_group(${SUB} FILES ${SUB_SRCS})
   add_wsf_doxygen_input(${CMAKE_CURRENT_SOURCE_DIR}/${SUB})
endforeach(SUB ${WSF_SUB})

# Add source files in wsf/source
file(GLOB SUB_SRCS *.?pp)
set(SRCS ${SRCS} ${SUB_SRCS})

# Configuration for WsfEventPipe. Adding dependency: pack_to_cpp,
# and rules to generate source from schema
include(${WSF_ROOT}/wsf/event_pipe.cmake)
add_event_pipe_schema(${CMAKE_CURRENT_SOURCE_DIR}/../event_pipe/wsf.utpack
                      ""
                      ""
                      ""
                      "WsfEventPipe"
                      "WsfEventPipe"
                      EVENT_PIPE_INCLUDES
                      EVENT_PIPE_SOURCES)

# Some .obj files exceed 65,279 symbols in MSVC DEBUG builds, requiring /bigobj,
# especially in older MSVC compilers (2015, 2017.)
# /bigobj increases file sizes by 2-3% so we only add it where needed.
large_source_files(WsfEventOutputBase.cpp
                   event_pipe/WsfEventPipeInterface.cpp
                   WsfScenario.cpp
                   dis/WsfDisInterface.cpp
                   processor/WsfTaskManager.cpp
                   script/WsfScriptManager.cpp
                   script/WsfScriptObserver.cpp
                   script/WsfScriptPlatformClass.cpp # This one still needs /bigobj even in VS 2022 (Debug)
                   script/WsfScriptTrackClass.cpp
                   traffic/XWsfOSM_Traffic.cpp
                   xio_sim/WsfXIO_AutoDisMapping.cpp
                   xio_sim/WsfXIO_MessageService.cpp
                   xio_sim/WsfXIO_ObjectInfoLoad.cpp
                   xio_sim/WsfXIO_PlatformInfoService.cpp
                   xio_sim/WsfXIO_Simulation.cpp
)

# Add the project's grammar file to the list that is used to install
# all grammar files to the correct location
wsf_grammar_file(SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../grammar/wsf.ag")

# Add a 'wsf' library target that will be built from the listed sources
add_library(${PROJECT_NAME} ${SRCS} ${CONFIGURATION_FILES}
            ${CMAKE_CURRENT_SOURCE_DIR}/../event_pipe/wsf.utpack
            ${EVENT_PIPE_INCLUDES}
           )
swdev_warning_level(${PROJECT_NAME})
generate_export_header(${PROJECT_NAME})

# Set the subfolder that the target will appear under in an IDE's solution explorer.
# This isn't actually related to any file system folders.
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "core")

# Wsf is just too big as one giant unity build file. Set maximum batch size of 128.
if (NOT CMAKE_UNITY_BUILD_BATCH_SIZE OR CMAKE_UNITY_BUILD_BATCH_SIZE EQUAL 0 OR CMAKE_UNITY_BUILD_BATCH_SIZE GREATER 128)
   set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD_BATCH_SIZE 128)
endif()

# For MSVC set linker flags for debug to allow incremental linking without
# fatal error LNK1210: exceeded internal ILK size limit...
if(MSVC)
   set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/MAXILKSIZE:0x7ff00000")
endif()

# Configure version header file
include(GenerateVersionInfo)
generate_version_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${WSF_COMP_INCLUDES}
                           ${PROJECT_BINARY_DIR}/source
                           ${afsim_BINARY_DIR} ${WSF_EXTENSION_HEADER_DIR}) # For generated files ( WsfEventPipeClasses* )

# Specify the libraries required by the wsf target project
target_link_libraries(${PROJECT_NAME} ${TOOLS_LIBS} wsf_util)

# Install the wsf lib file.
swdev_lib_install(${PROJECT_NAME})
