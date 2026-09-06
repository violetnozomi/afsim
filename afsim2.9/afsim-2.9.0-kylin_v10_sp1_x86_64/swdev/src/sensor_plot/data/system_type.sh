# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
#!/bin/sh
# $Id: system_type.sh,v 1.3 2007/10/26 21:05:44 m191713 Exp $
# Used by makefiles to get the system type.

case `uname -s` in
  IRIX*)
    SYSTEM=sgi
    ;;
  Linux*)
    version=`gcc -v 2>&1 | tail -1 | awk '{print $3}'`
    case "$version" in
      2.*) SYSTEM=lnx
           ;;
      3.*) SYSTEM=ln3
           ;;
      4.*) SYSTEM=ln4
           ;;
      *)   SYSTEM=ln3
           ;;
    esac
    ;;
  SunOS)
    SYSTEM=sun
    ;;
  OSF1)
    SYSTEM=dec
    ;;
  CYG*)
    SYSTEM=cyg
    ;;
  *)
    SYSTEM=unk
    ;;
esac
echo ${SYSTEM}
exit 0
