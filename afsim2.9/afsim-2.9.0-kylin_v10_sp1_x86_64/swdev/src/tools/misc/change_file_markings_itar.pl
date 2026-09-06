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
# WARNING: The following files are not handled correctly by the script and will 
#          need to be hand-edited:
#          - cmake\Modules\doxygen\mainpage.txt
#          - wsf_plugins\mainpage.txt

use File::DosGlob qw(bsd_glob); # to get sane whitespace semantics

my $defaultOrganization = "nc";
my $organization = $defaultOrganization;  #organization may change from file to file
                                          #unless otherwise specified.
my $hasOrganization = 0;   #check if an organization was specified.
my $isNX = 0;              #check for non export option.
my $defaultYear = 2018;    #start year to be reset for before each file.
my $year = $defaultYear;   #year may change from file to file unless otherwise specified.
my $hasYear = 0;           #year has been entered
my $pattern;               #arguments or patterns to glob together
my $comment = "#";
my $closingComment = "";
my $checkComment = "\#";
my $checkComment2 = "//";
my $updatedByIST = 0;
my $argSize = scalar @ARGV;
for (my $i=0; $i < $argSize; $i = $i + 1)
{
   $pattern = $ARGV[$i];
   if($pattern eq "-?" ||
      $pattern eq "--help")
   {
      print ".../change_file_markings_itar.pl [-? | --help] [--boeing | --ist | --no-copyright]
            [--updated-by-ist] [--nx] [--year <year>] <reg_ex>\n";
      print "-? | --help      (optional)       Show help dialog.\n";
      print "--boeing | --ist                  Print legacy Boeing headers, print IST headers or\n";
      print "--no-copyright   (optional)       print no copyright.\n";
      print "                                  Default: If a file already has a header, print the same organization,\n";
      print "                                           If no header exists, no copyright will print.\n";
      print "--updated-by-ist (optional)       Print a disclaimer that IST updated the file.\n";
      print "--nx             (optional)       Print Non-exportable headers.\n";
      print "--year <year> (  (optional)       Change the year or range of years for the copyright.\n";
      print "                                  IST Default: 2018. Boeing Default: 2003-2015.\n";
      print "<reg_ex>         (required)       Change any file matching the given regular expression.\n";
      print "                                  Ex: \"*.txt\" changes all files in the directory that end in \".txt\"\n";
      exit;
   }
   if($pattern eq "--boeing")
   {
      $defaultOrganization = "The Boeing Company";
      if (not $hasYear)
      {
         $defaultYear = "2003-2015";
      }
      $hasOrganization = 1;
   }
   elsif($pattern eq "--ist")
   {
      $defaultOrganization = "Infoscitex, a DCS Company";
      if (not $hasYear)
      {
         $defaultYear = "2018";
      }
      $hasOrganization = 1;
   }
   elsif($pattern eq "--no-copyright")
   {
      $defaultOrganization = "nc";
      $organization = $defaultOrganization;
   }
   elsif($pattern eq "--nx")
   {
      $isNX = 1;
   }
   elsif($pattern eq "--year")
   {
      $hasYear = 1;
      if (($i + 1 ge $argSize) ||
           ($ARGV[$i+1] !~ /[0-9][0-9][0-9][0-9](-[0-9][0-9][0-9][0-9])?/))
      {
         print "***ERROR: Must enter a year or year range. Ex: 2012 or 2012-2015";
         exit;
      }
      $defaultYear = $ARGV[$i + 1];
      $i = $i + 1;
   }
   elsif($pattern eq "--updated-by-ist")
   {
      $updatedByIST = 1;
   }
   else
   {
      my @files = glob($pattern);
      my $file;
      foreach $file (@files)
      {
         if (($file =~ /\.ant$/i)  ||
             ($file =~ /\.png$/i)  ||
             ($file =~ /\.gif$/i)  ||
             ($file =~ /\.jpg$/i)  ||
             ($file =~ /\.ico$/i)  ||
             ($file =~ /\.csv$/i)  ||
             ($file =~ /\.zip$/i)  ||
             ($file =~ /\.ui$/i)   ||
             ($file =~ /\.pdf$/i)  ||
             ($file =~ /\.qrc$/i)  ||
             ($file =~ /\.dbf$/i)  ||
             ($file =~ /\.prj$/i)  ||
             ($file =~ /\.shp$/i)  ||
             ($file =~ /\.shx$/i)  ||
             ($file =~ /\.svg$/i)  ||
             ($file =~ /\.html$/i) ||
             ($file =~ /\.tspi$/i) ||
             ($file =~ /\.user$/i) ||
             ($file =~ /\.xml$/i)  ||
             ($file =~ /\.md$/i)   ||
             ($file =~ /\.pack$/i) ||
             ($file =~ /\.idx$/i)  ||
             ($file =~ /\.dt1$/i)  ||
             ($file =~ /\.flt$/i)  ||
             ($file =~ /\.hdr$/i)  ||
             ($file =~ /\.tif$/i)  ||
             ($file =~ /\.ttf$/i)  ||
             ($file =~ /\.jpeg$/i) ||
             ($file =~ /\.osgb$/i) ||
             ($file =~ /\.json$/i) ||
             ($file =~ /\.dat$/i)  ||
             ($file =~ /\.docx$/i) ||
             ($file =~ /\.pptx$/i) ||
             ($file =~ /\.wxs$/i)  ||
             ($file =~ /\.wav$/i)  ||
             ($file =~ /footer\.txt$/i) ||
             ($file =~ /index\.redirect$/i) ||
             ($file =~ /wix.template.in$/i) ||
             ($file =~ /cpack_license\.txt$/i) ||
             ($file =~ /\.expected_.*_map$/i))
         {
            next;
         }
         else
         {
            $year = $defaultYear;
            $organization = $defaultOrganization;
            GetCommentType($file);
            ProcessFile($file);
         }
      }
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
          ($line =~ /CUI/i) ||
          ($line =~ /UNCLASSIFIED/i))
      {
         $isMarkingLine = 1;
         GetYear($line);
         GetOrganization($line);
      }
   }
   return $isMarkingLine;
}

# ==============================================================================
sub GetYear
{
   # If the year has not been entered into the command line,
   # read the current year from the copyright statement.
   my $line = shift;
   if (not $hasYear)
   {
      my $commentShift = length($comment) - 1;
      if ($line =~ /$checkComment2/ &&
          $comment ne $checkComment2)
      {
         $commentShift += 1;
      }
      if($line =~ /Copyright\s[12][0-9][0-9][0-9]-[12][0-9][0-9][0-9]/i)
      {
         $year = substr($line,12 + $commentShift,9);
      }
      elsif($line =~ /Copyright\s[12][0-9][0-9][0-9]/i)
      {
         $year = substr($line,12 + $commentShift,4);
      }
   }
}

# ==============================================================================
sub GetOrganization
{
   # If the organization has not been entered into the command line,
   # read the current organization from the copyright statement.
   my $line = shift;
   if (not $hasOrganization)
   {
      if($line =~ /boeing/i)
      {
         $organization = "The Boeing Company";
      }
      elsif($line =~ /infoscitex/i)
      {
         $organization = "Infoscitex, a DCS Company";
      }
   }
}

# ==============================================================================
sub GetCommentType
{
   my $file = shift;
   $closingComment = "";
   if (($file =~ /\.atg$/i) ||
       ($file =~ /\.cpp$/i) ||
       ($file =~ /\.frag$/i) ||
       ($file =~ /\.hpp$/i) ||
       ($file =~ /\.hpp.in$/i) ||
       ($file =~ /\.rc$/i) ||
       ($file =~ /\.rc.in$/i) ||
       ($file =~ /\.vert$/i) ||
       ($file =~ /\.utpack$/i) ||
       ($file =~ /\.cxx$/i) ||
       ($file =~ /\.hxx$/i) ||
       ($file =~ /\.h$/i) ||
       ($file =~ /\.frame$/i) ||
       ($file =~ /\.xenum$/i) ||
       ($file =~ /\.geom$/i) ||
       ($file =~ /\.c$/i))
   {
      $comment = "//";
      $checkComment="//";
   }
   elsif(($file =~ /\.js$/i) ||
         ($file =~ /\.js_t$/i))
   {
      $comment = "//";
      $checkComment="(#|//)";
   }
   elsif($file =~ /\.bat$/i)
   {
      $comment = "rem";
      $checkComment="rem";
   }
   elsif($file =~ /\.m$/i)
   {
      $comment = "%";
      $checkComment="\%";
   }
   elsif($file =~ /\.mod$/i)
   {
      $comment = "!";
      $checkComment="!";
   }
   elsif($file =~ /\.vim$/i)
   {
      $comment = "\"";
      $checkComment="\"";
   }
   elsif(($file =~ /\.css$/i) ||
         ($file =~ /\.css_t$/i))
   {
      $comment = "/\*";
      $checkComment = "/\*";
      $closingComment = "\*/";
   }
   elsif (($file =~ /\.rst$/i) ||
          ($file =~ /demo_template\.txt$/i) ||
          (($file =~ /\.txt$/i) && ($file =~ /[\\\/]+doc[\\\/]+/i)))
   {
      $comment = "..";
      $checkComment="\.\.";
   }
   elsif($file =~ /\.ini$/i)
   {
      $comment = ";";
      $checkComment = "(;|#)";
   }
   else
   {
      $comment = "#";
      $checkComment = "(#|//)";
   }
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
         GetYear($line);
         GetOrganization($line);
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
   if ($comment eq "rem")
   {
      print OFILE "\@echo off\n";
   }
   print OFILE "$comment ****************************************************************************\n";
   if ($isNX)
   {
      print OFILE "$comment CUI//REL TO USA ONLY\n";
   }
   else
   {
      print OFILE "$comment CUI\n";
   }

   print OFILE "$comment\n";
   print OFILE "$comment The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)\n";
   print OFILE "$comment\n";
   if ($organization ne "nc")
   {
      print OFILE "$comment Copyright $year $organization. All rights reserved.\n";
      print OFILE "$comment\n";
   }

   print OFILE "$comment The use, dissemination or disclosure of data in this file is subject to\n";
   print OFILE "$comment limitation or restriction. See accompanying README and LICENSE for details.\n";
   print OFILE "$comment ****************************************************************************$closingComment\n";

   if ($updatedByIST && $organization eq "The Boeing Company")
   {
      print OFILE "$comment ****************************************************************************\n";
      print OFILE "$comment Updated by Infoscitex, a DCS Company.\n";
      print OFILE "$comment ****************************************************************************\n";
   }

   if (not $skippingHeader)
   {
      print OFILE "\n";
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
   rename "$file.new", "$file" or warn "***** WARNING: Unable to rename $file.new to $file\n";
}
