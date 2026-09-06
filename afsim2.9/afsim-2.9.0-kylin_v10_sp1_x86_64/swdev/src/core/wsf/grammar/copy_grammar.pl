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

# Blindly copies grammar files from all projects in the root directory
use FindBin;
my $thisDir = $FindBin::Bin;

use File::Copy;

die "Target Directory must be specified as the only argument" if (@ARGV == 0);

my $targetDir = $ARGV[0];
@projDirs = glob "$thisDir/../../*";

foreach $dir (@projDirs)
{
   if (-d $dir)
   {
      foreach $gf (glob "$dir/grammar/*.ag")
      {
         if ($gf =~ '\.ag$')
         {
            print "Copy $gf -> $targetDir\n";
            copy($gf, $targetDir);
         }
      }
   }
}

