# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2003-2015 The Boeing Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

import os

def get_type():
    uname = os.uname()
    if uname[0].startswith('Linux'):
        return 'ln4'
    if uname[0].startswith('Darwin'):
        return 'mac'
    if uname[0].startswith('CYG'):
        return 'cyg'
