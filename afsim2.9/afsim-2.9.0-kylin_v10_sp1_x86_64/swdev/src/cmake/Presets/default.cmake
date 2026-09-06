# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# Default configuration; all distributed modules enabled
set(ENABLE_EXTENSION_DEFAULT FALSE CACHE INTERNAL  "")

include("${CMAKE_CURRENT_LIST_DIR}/base.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/exportable.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/exportable_gui.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/restricted.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/restricted_gui.cmake")
