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
# project-specific rules.

install::
	$(MISCDIR)/install.sh 664 $(RELLIB) $(LIBDIR) lib$(PROJECT)$(SYSSUFFIX).a
	$(MISCDIR)/install.sh 664 $(RELLIB) $(LIBDIR) lib$(PROJECT)_d$(SYSSUFFIX).a

