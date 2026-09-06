# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# Determine and set MACHINE

X11_BIN_DIR="/usr/bin/X11"
if [ ! "$MACHINE" ]
then
  case "`uname -s`" in
    IRIX*)
      MACHINE="_sgi"
      ;;
    Linux*)
      MACHINE="_lnx"
      ;;
    SunOS*)
      X11_BIN_DIR="/usr/openwin/bin"
      MACHINE="_sun"
      ;;
    *)
      MACHINE="_unk"
      ;;
  esac
fi

echo ${MACHINE}
