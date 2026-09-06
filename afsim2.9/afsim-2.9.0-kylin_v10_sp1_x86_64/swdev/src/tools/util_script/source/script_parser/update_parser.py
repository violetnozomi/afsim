# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
# ****************************************************************************
# Updated by Infoscitex, a DCS Company.
# ****************************************************************************

# Front-end to invoke either update_parser_py2.py or update_parser_py3.py.

import sys
import os

cur_version = sys.version_info

if cur_version >= (3,0):
   full_path = os.path.realpath(__file__)
   path, file = os.path.split(full_path)
   full_path = os.path.join(path, "update_parser_py3.py")
   exec(open(full_path).read())
else:
   full_path = os.path.realpath(__file__)
   path, file = os.path.split(full_path)
   full_path = os.path.join(path, "update_parser_py2.py")
   execfile(full_path)
