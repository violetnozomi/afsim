# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

These are very simple test cases for exercising DIS emission PDU's.

To run the tests:

*) Open two command windows.

*) If desired, update realtime.txt to select a different network configuration.

*) In one window type:

   <path_to_mission> aircraft_rt.txt

*) In the other window type:

   <path_to_mission> radar_rt.txt

*) Hit enter in each window quickly in succession to start the scenarios

The same procedure can be done with big_red_rt.txt and big_blue_rt.txt

The executables automatically start (they will not wait for a Start PDU) and
will stop after 60 seconds. You should see some output messages indicating
what is happening.

