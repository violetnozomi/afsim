# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# All non-gui modules that are excluded from export variants of AFSIM

include("${CMAKE_CURRENT_LIST_DIR}/base.cmake")

set(ENABLE_EXTENSION_DEFAULT FALSE CACHE INTERNAL "")

# Modules that are tracked in core afsim repo
set(BUILD_WITH_engage TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_air_combat TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_brawler TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_fires TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_l16 TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_mtt TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_nx TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_sosm TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_weapon_server TRUE CACHE BOOL "")

# Distributed modules that are tracked outside of afsim repo
set(BUILD_WITH_wsf_argo8 TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_iads_c2 TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_oms_uci TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_scenario_analyzer_iads_c2 TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_simdis TRUE CACHE BOOL "")
