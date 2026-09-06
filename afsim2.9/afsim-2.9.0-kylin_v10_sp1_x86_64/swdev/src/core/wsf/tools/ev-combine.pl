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

# Read a WSF event log and combine multi-line events into a 
# single (potentially very long) line.
#
# The input is read from standard input and the output is written to standard output.

while (<>)
{
    chomp;
    if (/\\$/)
    {
        chop;
        print;
    }
    else
    {
        print "$_\n";
    }
}
