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
	./install_setup.sh $(RELROOT) $(RELVER)
	$(MISCDIR)/install_setup.sh $(RELROOT) $(RELVER)
	$(MISCDIR)/install.sh 664 $(RELROOT) . AAA_PROPRIETARY_NOTICE.TXT
	$(MISCDIR)/install.sh 664 $(RELROOT) . AAA_RELEASE_NOTES.TXT
	$(MISCDIR)/install.sh 664 $(RELROOT) . AAA_README_UNIX.TXT
	$(MISCDIR)/install.sh 775 $(RELROOT) . cme_configure 
	$(MISCDIR)/install.sh 775 $(RELROOT) . cme_default
	$(MISCDIR)/install.sh 775 $(RELROOT) . cme_install
	$(MISCDIR)/install.sh 664 $(RELETC)  . beam_parameters.txt
	$(MISCDIR)/install.sh 664 $(RELETC)  . dis_enums.txt
	$(MISCDIR)/install.sh 775 $(RELBIN)  $(MISCDIR) get_machine.sh
	rm -rf $(RELETC)/fonts
	mkdir $(RELETC)/fonts
	chmod 775 $(RELETC)/fonts
	cp fonts/*.ttf $(RELETC)/fonts
	cp fonts/*.txt $(RELETC)/fonts
	chmod 664 $(RELETC)/fonts/*.*
