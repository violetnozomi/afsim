# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# DO NOT DELETE THIS FILE - It is included by the system-specific makefiles.
#
# This file can be modified as necessary to implement system-independent
# project-specific definitions

PROJECT=dis

# Include directories searched only for 'make depend'.  You probably want to
# omit things like system or package libraries that rarely change.
DINCLUDES=-I../../genio/source \
          -I../../util/source

# Include directories (-Idir1 -Idir2 ...)
INCLUDES=$(DINCLUDES) $(STLINC)

