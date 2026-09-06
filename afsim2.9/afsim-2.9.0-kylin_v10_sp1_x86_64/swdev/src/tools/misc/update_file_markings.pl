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

$gFirstDate = "";
$gLastDate  = "";

my $file;
foreach $file (@ARGV)
{
   ProcessFile($file);
}

# ==============================================================================
sub IsMarkingLine
{
   my $line = shift;

   my $isMarkingLine = 0;
   if ($line =~ /\s*\/\//)
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
   GetFirstAndLastDates($file);

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
      # Start and end of our standard marking is '// ***** ....'

      if ($line =~ /^\s*\/\/\s*\*+\s*$/)
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
         if ($line !~ /^\s*\/\//)
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

   print OFILE "// ****************************************************************************\n";
   if ($gFirstDate == $gLastDate)
   {
      print OFILE "//                        Copyright (c) $gLastDate Boeing.\n";
   }
   else
   {
      print OFILE "//                      Copyright (c) $gFirstDate-$gLastDate Boeing.\n"
   }
   print OFILE "// ****************************************************************************\n";

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
   rename "$file", "$file.old" or die "Unable to rename $file to $file.old\n";
   rename "$file.new", "$file" or die "***** ERROR Unable to rename $file.new to $file\n";
}

# ==============================================================================
sub GetFirstAndLastDates()
{
   my $file = shift;
   $gFirstDate = "";
   $gLastDate  = "";
   my $command = "cvs log -N $file";
   close CVSLOG;
   open CVSLOG, "$command |";
   my $line;
   while ($line = <CVSLOG>)
   {
      next if ($line !~ /^date: [12][0-9][0-9][0-9]\//);
      my $dateStr = substr($line, 6, 4);
      if ($gLastDate eq "")
      {
         $gLastDate = $dateStr;
      }
      $gFirstDate = $dateStr;
   }
   close CVSLOG;
}
