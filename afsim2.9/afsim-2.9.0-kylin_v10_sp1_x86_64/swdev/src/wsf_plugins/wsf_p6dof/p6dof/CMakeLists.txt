# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
project(p6dof)

###########################################
# Use L4 Warnings with MSVC
###########################################
if(MSVC)
  # Force to always compile with W4. Flag /wd4201 disables warnings
  # for a non-standard extension on a nameless struct in an included file.
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201")
  else(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4201")
  endif()
endif(MSVC)

IF(CMAKE_COMPILER_IS_GNUCXX)
   # This argument causes the linker to exit if the library has unresolved
   # symbols.  Do not disable this option.
   SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
ENDIF()

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} LIBRARY
STANDALONE
HDRS
   ${HDRS}
SRCS
   ${SRCS}
)

target_link_libraries(${PROJECT_NAME} util genio)

add_subdirectory(test)

install_sources_all_files(test ${_project_install_dir})
