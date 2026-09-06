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

my $checkComment = "\\#";
foreach $pattern (@ARGV)
{  
   my @files = glob($pattern);
   my $file;
   foreach $file (@files)
   {
      if (($file =~ /\.rst$/i) ||
          ($file =~ /\.png$/i) ||
          ($file =~ /\.jpg$/i) ||
          ($file =~ /\.ico$/i) ||
          ($file =~ /\.csv$/i) ||
          ($file =~ /\.zip$/i) ||
          ($file =~ /\.ui$/i) ||
          ($file =~ /\.pdf$/i) ||
          ($file =~ /\.shx$/i) ||
          ($file =~ /\.qrc$/i) ||
          ($file =~ /\.dbf$/i) ||
          ($file =~ /\.shp$/i) ||
          ($file =~ /\.shx$/i) ||
          ($file =~ /\.jpeg$/i))
         {
            next;
         }
      GetCommentType($file);
      ProcessFile($file);
   }
}

# ==============================================================================
sub IsMarkingLine
{
   my $line = shift;

   my $isMarkingLine = 0;
   if ($line =~ /\s*$checkComment/)
   {
      if (($line =~ /boeing/i) ||
          ($line =~ /Infoscitex/i) ||
          ($line =~ /copyright/i) ||
          ($line =~ /UNCLASSIFIED/i))
         {
            $isMarkingLine = 1;
         }
   }
   return $isMarkingLine;
}

# ==============================================================================
sub GetCommentType
{
   my $file = shift;
   if (($file =~ /\.atg$/i) ||
       ($file =~ /\.cpp$/i) ||
       ($file =~ /\.frag$/i) ||
       ($file =~ /\.hpp$/i) ||
       ($file =~ /\.rc$/i) ||
       ($file =~ /\.vert$/i) ||
       ($file =~ /\.utpack$/i) ||
       ($file =~ /\.cxx$/i) ||
       ($file =~ /\.hxx$/i) ||
       ($file =~ /\.h$/i) ||
       ($file =~ /\.frame$/i) ||
       ($file =~ /\.c$/i))
   {
      $checkComment="\\\/\\\/";
   }
   elsif($file =~ /\.bat$/i)
   {
      $checkComment="rem";
   }
   elsif($file =~ /\.m$/i)
   {
      $checkComment="\\\%";
   }
   elsif($file =~ /\.mod$/i)
   {
      $checkComment="\\\!";
   }
   elsif($file =~ /\.vim$/i)
   {
      $checkComment="\"";
   }
   else
   {
      $checkComment = "\#";
   }
}
# ==============================================================================
sub ProcessFile
{
   my $file = shift;
   open IFILE, "<$file" or die "Unable to open $file\n";

   # Search the first 20 lines for a non-standard file header. If one doesn't exist then it is created.

   my $prefix = "";
   my $stars  = "";
   my $line   = "";
   my $lineCount = 0;
   my $foundStars = 0;
   my $skippingHeader = 0;
   
   while ($line = <IFILE>)
   {
      # Start and end of our standard marking is '// ***** ....' or '# ***** ....'

      if ($line =~ /^\s*$checkComment\s*\*+\s*$/)
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
         if (($line !~ /^\s*$checkComment/) &&
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
   rename "$file.new", "$file" or die "***** ERROR: Unable to rename $file.new to $file\n";
}
