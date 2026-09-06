# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

file(GLOB QRC ${CMAKE_CURRENT_SOURCE_DIR}/../ui/resources/*.qrc)

plugin_cmakelists_template(MapDisplay tools/wkf
                           ADDITIONAL_SOURCES ${QRC}
                           THIRD_PARTY_INCLUDE_DIRS osg
                           LIBRARIES wkf wkf_common)
