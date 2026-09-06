# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright (C) 2022 Stellar Science; U.S. Government has Unlimited Rights.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

add_executable(artificer-cli main.cpp)

set_property(TARGET artificer-cli PROPERTY FOLDER "tools/artificer")
swdev_warning_level(artificer-cli)
install(TARGETS artificer-cli
   RUNTIME DESTINATION "${INSTALL_EXE_PATH}" COMPONENT Runtime
)
set_target_properties(artificer-cli PROPERTIES INSTALL_RPATH "$ORIGIN/lib")
target_link_libraries(artificer-cli util artificer)
