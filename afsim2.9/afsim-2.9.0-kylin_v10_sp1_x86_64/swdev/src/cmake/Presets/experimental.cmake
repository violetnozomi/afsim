# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

set(ENABLE_EXTENSION_DEFAULT FALSE CACHE INTERNAL "")

# Experimental modules that are managed but not distributed. 
# These may contain exportable and non-exportable plugins; refer
# to each module's README for any export restrictions.
set(BUILD_WITH_wsf_ibsm_sensor TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_jse TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_protobuf TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_script_io TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_zmq TRUE CACHE BOOL "")
