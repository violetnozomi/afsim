# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# All exportable non-gui modules
include("${CMAKE_CURRENT_LIST_DIR}/base.cmake")

set(ENABLE_EXTENSION_DEFAULT FALSE CACHE INTERNAL "")

# Modules that are tracked in core afsim repo
set(BUILD_WITH_sensor_plot TRUE CACHE BOOL "")
set(BUILD_WITH_sensor_plot_lib TRUE CACHE BOOL "")
set(BUILD_WITH_weapon_tools TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_alternate_locations TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_annotation TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_coverage TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_cyber TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_mil TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_mil_parser TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_multiresolution TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_p6dof TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_six_dof TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_ripr TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_space TRUE CACHE BOOL "")

# Distributed modules that are tracked outside of afsim repo
set(BUILD_WITH_post_processor TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_scenario_analyzer TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_tmap	TRUE CACHE BOOL "")
