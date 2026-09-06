# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(engage)

# If the CMAKE_INSTALL_PREFIX is the default value
# and has not been specified by a higher level CMake file,
# set a default.
IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT (CMAKE_INSTALL_PREFIX MATCHES "wsf_install?"))
  SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/engage_install CACHE PATH "Installation path" FORCE)
ENDIF()


# Set the shared library "paths" before calling "add_executable"
# The actual lib names can just be added, in the modules cmake file,
# using 'target_link_libraries'
if (COMMAND set_3rd_party_shared_paths)
   set_3rd_party_shared_paths()
endif()


FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} EXECUTABLE
HDRS
   ${HDRS}
SRCS
   ${SRCS}
GRAMMARS
   grammar/engage.ag
NOTICES
   NO_EXPORT.md
   README.md
)

# *****************************************************************************
# Configure version header and resource files
include(GenerateVersionInfo)
generate_version_header(${PROJECT_NAME})
if(MSVC)
   generate_resource(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/source/engage_icon.ico")
endif()

link_wsf_optional_projects(${PROJECT_NAME})
link_sockets(${PROJECT_NAME})

# Generate this applications AUTO_TEST target.
generate_wsf_auto_test(${PROJECT_NAME})

# Generate this applications REGRESSION_TEST target.
generate_wsf_regression_test(${PROJECT_NAME})

# Install all WSF grammar to get "main" grammar file and any files defining
# commands that may be used by engage but not included in engage.ag
get_property(WSF_GRAMMAR_FILES GLOBAL PROPERTY WSF_GRAMMAR_FILES)
install(FILES ${WSF_GRAMMAR_FILES} DESTINATION bin/${PROJECT_NAME}_grammar COMPONENT Runtime)
# Install engage specific grammar
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/grammar/engage.ag DESTINATION bin/${PROJECT_NAME}_grammar COMPONENT Runtime)

install_tests(tests ${PROJECT_NAME})

if(WSF_INSTALL_DEMOS)
   # Data directories included with the engage build
   set(WSF_DEMO_ENGAGE
       engage
      )
   install_wsf_data("${WSF_DEMO_ENGAGE}" ${WSF_DEMOS_ROOT} demos Demo)
endif()
