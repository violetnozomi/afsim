# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
#!/bin/sh

# Used by makefiles to get the system type.

case `uname -s` in
  IRIX*)
    MAKEFLAGS=
    ;;
  Linux*)
    MAKEFLAGS=--no-print-directory
    ;;
  SunOS)
    MAKEFLAGS=
    ;;
  OSF1)
    MAKEFLAGS=
    ;;
  *)
    MAKEFLAGS=
    ;;
esac
echo ${MAKEFLAGS}
exit 0
