# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# Bare minimum AFSIM including mission, wsf, grammar_check and parser.
# Useful for testing a single plugin without dependencies on other extensions.
set(ENABLE_EXTENSION_DEFAULT FALSE CACHE INTERNAL "")

set(BUILD_WITH_mission TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_grammar_check TRUE CACHE BOOL "")
set(BUILD_WITH_wsf_parser TRUE CACHE BOOL "")
