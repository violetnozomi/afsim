# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2003-2013 The Boeing Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# BRAWLER Aircraft Conversion to AFSIM P6DOF input format

# Quick Start:   perl  ConvertBrawlerToP6Dof.pl  <BRAWLER ACFT FILE>

# Files Provided:

   # The perl script that converts a BRAWLER aircraft definition file (*.FXW) into a format usable by the AFSIM P6DOF
   ConvertBrawlerToP6Dof.pl
   
   # An example [unclassified] BRAWLER aircraft definition file
   ACFT_BD.FXW
   
   # The output of running "ConvertBrawlerToP6Dof.pl" on "ACFT_BD.FXW"
   BRAWLER_ACFT_BD.txt
   
   # All other files are "place holders", they represent data that BRAWLER definitions do NOT provide
   # They will likely need updated if the aero properties of your brawler aircraft greatly differ from ACFT_BD.FXW
   BRAWLER_aero_components.txt
   BRAWLER_controls_providers.txt
   BRAWLER_flight_controls.txt
   BRAWLER_secondary_aero_data.txt
   BRAWLER_SyntheticAutopilotConfig.txt


# Directions:


1. Run the perl script on a BRAWLER aircraft definition file
   Example:    perl ConvertBrawlerToP6Dof.pl ACFT_BD.FXW
   
   Note: the output will named according to the aircraft specified in the BRAWLER file
   Example:    BRAWLER_ACFT_BD.txt     (aircraft name in the ACFT_BD.FXW file: "ACFT_BD")


2. Copy the output file and all five "place holder" files a directory known by your AFSIM scenario
   Put all the files in the "platforms" folder, otherwise you'll have to update line 44 of BRAWLER_controls_providers.txt with a relevant path


3. Use the P6DOF definition in your scenario by using a WSF_P6DOF_MOVER.
   See   ~/mediawiki/index.php/WSF_P6DOF_MOVER
   Example:
   platform ...
      mover  WSF_P6DOF_MOVER
         p6dof_object_type BRAWLER_ACFT_BD   //name of "p6dof_object_type" defined in BRAWLER_ACFT_BD.txt
      end_mover
      ...
   end_platform


4. Test and debug any issues with your mover
   You might have to adjust the size and response of various control surfaces or control values

