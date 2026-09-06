# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

project(sensor_plot_lib)

FILE(GLOB HDRS source/*.hpp)
FILE(GLOB SRCS source/*.cpp)

wsf_project_template(${PROJECT_NAME} LIBRARY
HDRS
   ${HDRS}
SRCS
   ${SRCS}
GRAMMARS
   grammar/sensor_plot_lib.ag
NOTICES
   README.md
)

target_link_libraries(${PROJECT_NAME} wsf_mil)

#add_subdirectory(test)

install_sources_all_files(test ${_project_install_dir})
