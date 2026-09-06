# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
#!/usr/bin/perl -W
# Update the file markings for the specified files.
 
use File::DosGlob qw(bsd_glob); # to get sane whitespace semantics

my $pattern;
foreach $pattern (@ARGV)
{
   my @files = glob($pattern);
   my $file;
   foreach $file (@files)
   {
      ProcessFile($file);
   }
}

# ==============================================================================
sub IsMarkingLine
{
   my $line = shift;

   my $isMarkingLine = 0;
   if (($line =~ /\s*\/\//) ||
       ($line =~ /\s*\#/))
   {
      if (($line =~ /boeing/i) ||
          ($line =~ /copyright/i))
      {
         $isMarkingLine = 1;
      }
   }
   return $isMarkingLine;
}

# ==============================================================================
sub ProcessFile
{
   my $file = shift;

   open IFILE, "<$file" or die "Unable to open $file\n";

   # Search the first 20 lines for a non-standard file header. If one doesn't exist then it is created.

   my $prefix = "";
   my $suffix = "";
   my $stars  = "";
   my $line   = "";
   my $lineCount = 0;
   my $foundStars = 0;
   my $skippingHeader = 0;
   while ($line = <IFILE>)
   {
      # Start and end of our standard marking is '// ***** ....' or '# ***** ....'

      if (($line =~ /^\s*\/\/\s*\*+\s*$/) ||
          ($line =~ /^\s*\#\s*\*+\s*$/))
      {
         last if $skippingHeader;
         $foundStars = 1; 
         $stars = $line;
      }
      elsif ($foundStars)
      {
         # Previous line was the potential start of a standard marking block. If this is a marking line
         # then it starts the header, otherwise it is just another line.

         $foundStars = 0;
         if (IsMarkingLine($line))
         {
            $skippingHeader = 1;
         }
         else
         {
            $prefix = $prefix . $stars . $line;
            $lineCount += 2;
            last if $lineCount >= 20;
         }
      }
      elsif ($skippingHeader)
      {
         # The header terminates on the first non-comment line.
         if (($line !~ /^\s*\/\//) &&
		     ($line !~ /^\s*\#/) &&
			 ($line !~ /\#include/))
         {
            $prefix = $prefix . $line;
            last;
         }
      }
      else
      {
         $prefix = $prefix . $line;
         last if ($lineCount >= 20);
      }
   }

   # Open the temporary output file and write the new header.

   unlink("$file.new");
   open OFILE, ">$file.new" or die "Unable to open $file.new\n";

   if (($file =~ /\.txt$/i) ||
       ($file =~ /\.pl$/i) ||
       ($file =~ /\.py$/i)){
      print OFILE "# ****************************************************************************\n";
      print OFILE "# Copyright 2003-2015 The Boeing Company. All rights reserved.\n";
      print OFILE "#\n";
      print OFILE "# This software is provided 'As Is', with no warranties, either express or\n";
      print OFILE "# implied, and Boeing will not be liable for any damages arising in any way\n";
      print OFILE "# from the use of this software.\n";
      print OFILE "#\n";
      print OFILE "# No license is required for the dissemination of the commercial information\n";
      print OFILE "# contained herein to foreign persons other than those from or in the\n";
      print OFILE "# terrorist supporting countries identified in the United States Export\n";
      print OFILE "# Administration Regulations (EAR) (15 CFR 730-774). It is the responsibility\n";
      print OFILE "# of the individual in control of this data to abide by U.S. export laws.\n";
      print OFILE "#\n";
      print OFILE "# ECCN: EAR-99\n";
      print OFILE "# ****************************************************************************\n";
   }
   else {
      print OFILE "// ****************************************************************************\n";
      print OFILE "// Copyright 2003-2015 The Boeing Company. All rights reserved.\n";
      print OFILE "//\n";
      print OFILE "// This software is provided 'As Is', with no warranties, either express or\n";
      print OFILE "// implied, and Boeing will not be liable for any damages arising in any way\n";
      print OFILE "// from the use of this software.\n";
      print OFILE "//\n";
      print OFILE "// No license is required for the dissemination of the commercial information\n";
      print OFILE "// contained herein to foreign persons other than those from or in the\n";
      print OFILE "// terrorist supporting countries identified in the United States Export\n";
      print OFILE "// Administration Regulations (EAR) (15 CFR 730-774). It is the responsibility\n";
      print OFILE "// of the individual in control of this data to abide by U.S. export laws.\n";
      print OFILE "//\n";
      print OFILE "// ECCN: EAR-99\n";
      print OFILE "// ****************************************************************************\n";
   }

   # Copy the remainder of the file.

   print OFILE $prefix;
   while ($line = <IFILE>)
   {
      print OFILE $line;
   }
   close IFILE;
   close OFILE;

   # Rename the <file> to <file>.old and rename <file>.new file to <file>.

   unlink "$file.old";
   #rename "$file", "$file.old" or die "Unable to rename $file to $file.old\n";
   rename "$file.new", "$file" or die "***** ERROR Unable to rename $file.new to $file\n";
}
