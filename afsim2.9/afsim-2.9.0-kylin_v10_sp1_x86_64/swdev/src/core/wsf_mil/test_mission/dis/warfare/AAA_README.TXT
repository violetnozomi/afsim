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
This is a very simply test case for exercising DIS warfare (fire and detonate)
PDU's.

To run the test:

*) Open two windows.

*) If desired, update realtime.txt to select a different network configuration.

*) In one window type:

   run shooter-rt.txt

*) As fast as possible, in the other window type:

   run target-rt.txt

The executables automatically start (they will not wait for a Start PDU) and
will stop after 60 seconds. You should see some output messages indicating
what is happening (fire, detonate, etc.)

In order to simulate the Fire PDU that may be sent by other applications, you
can also modify WsfDisWeaponEvents::WeaponTerminated near the bottom (there
are some lines that are commented out). Simply uncomment the lines for the
pieces you want to be 'undefined', rebuild and rerun. (JUST MAKE SURE TO CHANGE
IT BACK BEFORE YOU CHECK IT IN!!!)

