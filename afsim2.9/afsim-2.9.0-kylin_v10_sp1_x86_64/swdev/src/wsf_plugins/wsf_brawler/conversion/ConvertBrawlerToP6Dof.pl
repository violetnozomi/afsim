# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
#!/usr/bin/perl
use strict;
use warnings;
#use POSIX;          #for floor()
use Math::Complex;  #for sqrt()

my $file = $ARGV[0] or die "Need to name BRAWLER aircraft file in the command line input\n";
open(my $data, '<', $file) or die "Could not open '$file' $!\n";

my $line = <$data>;
chomp $line;
my @fields = split " " , $line;
my $aircraft_name = $fields[0];
print "aircraft name: $aircraft_name\n";

#my $ROWS = $fields[0];
#my $COLS = $fields[1];
#print "ROWS '$ROWS'\n";
#print "COLS '$COLS'\n";

sub get_line_fields
{
   #using global vars: line, data, & fields
   if (defined($line = <$data>))
   {
      chomp $line;
      @fields = split " " , $line;
      return 1;
   }
   return 0;
}

sub print_2d {
   my @array_2d=@_;
   for(my $i = 0; $i <= $#array_2d; $i++){
      for(my $j = 0; $j <= $#{$array_2d[0]} ; $j++){
         print "$array_2d[$i][$j] ";
      }
      print "\n\n";
   }
}

#&get_line_fields($data);
#
#while (defined($fields = get_line_fields($data)))
#{
#}

#sizes of PERF arrays and tables
my $NFM1   = 0;
my $NFM2   = 0;
my $NALPH1 = 0;
my $NFM3   = 0;
my $NALPH2 = 0;
my $NMACH1 = 0;
my $NHTS   = 0;
#PERF constants
my $RFAREA = 0;
my $ALPHA0 = 0;
my $FMMAX  = 0;
my $CASMAX = 0;
my $ABDT   = 0;
my $DTGGUP = 0;
my $DTGGDN = 0;
my $ABCORl = 0;
my $DDMM   = 0;
my $DDTD   = 0;
#PERF arrays and tables
my @FMVEC1;
my @CLMAX;
my @FMVEC2;
my @ALFV1;
my @CLIFT;
my @FMVEC3;
my @ALFV2;
my @CDRAG;
my @MCHVAL;
my @HTVEC;
my @TIDLE;
my @TMIL;
my @THRMX;
my @FLOIDL;
my @FLOMIL;
my @FLOMAX;
my @GCAP;
my @GCAP2;

#PHYS constants
my @PAREA;
my @VAREA;
my $DESWT = 0;

#util
my $r = 0;
my $c = 0;
my @matrix;
my $val = 0;


#while (defined($fields = get_line_fields($data)))


# read in the brawler file
#while ($line = <$data>) {
#   chomp $line;
#   my @fields = split " " , $line;

while (get_line_fields())
{
   if (scalar(@fields) <= 0)
   {
      #blank line, ignore it
   }
   elsif ($fields[0] eq "PERF")
   {
      print "starting processing the PERF block\n";
      while (get_line_fields())
      {
         if (scalar(@fields) <= 0) {
            #blank line, ignore it
         } elsif ($fields[0] eq "ENDBLOCK" && $fields[1] eq "PERF") {
            print "finished processing the PERF block\n";
            last;
         } elsif ($fields[0] eq "AROTYP") {
            if ($fields[1] != 1) {
               print "WARNING: Only AROTYP 1 is supported for now!\n";
            }
         } elsif ($fields[0] eq "PST_CAP") {
         } elsif ($fields[0] eq "AOA_TABLE") {
         } elsif ($fields[0] eq "NFM1") {
            $NFM1 = $fields[1];
         } elsif ($fields[0] eq "FMVEC1") {
            shift(@fields);
            $c = 0;
            foreach (@fields) { $FMVEC1[$c++] = $_; }
            while ($c < $NFM1) {
               get_line_fields();
               foreach (@fields) { $FMVEC1[$c++] = $_; }
            }
            #print "FMVEC1 = ";  print join(", ", @FMVEC1);  print "\n";
         } elsif ($fields[0] eq "CLMAX") {
            shift(@fields);
            $c = 0;
            foreach (@fields) { $CLMAX[$c++] = $_; }
            while ($c < $NFM1) {
               get_line_fields();
               foreach (@fields) { $CLMAX[$c++] = $_; }
            }
         } elsif ($fields[0] eq "ALPHA0") {
            $ALPHA0 = $fields[1];
         } elsif ($fields[0] eq "NFM2") {
            $NFM2 = $fields[1];
         } elsif ($fields[0] eq "FMVEC2") {
            shift(@fields);
            $c = 0;
            foreach (@fields) { $FMVEC2[$c++] = $_; }
            while ($c < $NFM2) {
               get_line_fields();
               foreach (@fields) { $FMVEC2[$c++] = $_; }
            }
         } elsif ($fields[0] eq "NALPH1") {
            $NALPH1 = $fields[1];
         } elsif ($fields[0] eq "ALFV1") {
            shift(@fields);
            $c = 0;
            foreach (@fields) { $ALFV1[$c++] = $_; }
            while ($c < $NALPH1) {
               get_line_fields();
               foreach (@fields) { $ALFV1[$c++] = $_; }
            }
         } elsif ($fields[0] eq "CLIFT") {
            #CLIFT[NALPH1][NFM2]
            shift(@fields);
            $c = 0;  $r = 0;
            foreach (@fields) {
               $CLIFT[$r][$c++] = $_;
               if ($c == $NFM2) { $c = 0;  $r++; }
            }
            while ($r < $NALPH1) {
               get_line_fields();
               foreach (@fields) {
                  $CLIFT[$r][$c++] = $_;
                  if ($c == $NFM2) { $c = 0;  $r++; }
               }
            }
            #print "CLIFT  = \n";  print_2d(@CLIFT);
         } elsif ($fields[0] eq "NFM3") {
            $NFM3 = $fields[1];
         } elsif ($fields[0] eq "FMVEC3") {
            shift(@fields);
            $c = 0;
            foreach (@fields) { $FMVEC3[$c++] = $_; }
            while ($c < $NFM3) {
               get_line_fields();
               foreach (@fields) { $FMVEC3[$c++] = $_; }
            }
         } elsif ($fields[0] eq "NALPH2") {
            $NALPH2 = $fields[1];
         } elsif ($fields[0] eq "ALFV2") {
            shift(@fields);
            $c = 0;
            foreach (@fields) { $ALFV2[$c++] = $_; }
            while ($c < $NALPH2) {
               get_line_fields();
               foreach (@fields) { $ALFV2[$c++] = $_; }
            }
         } elsif ($fields[0] eq "CDRAG") {
            #CDRAG[NALPH2][NFM3]
            shift(@fields);
            $c = 0;  $r = 0;
            foreach (@fields) {
               $CDRAG[$r][$c++] = $_;
               if ($c == $NFM3) { $c = 0;  $r++; }
            }
            while ($r < $NALPH2) {
               get_line_fields();
               foreach (@fields) {
                  $CDRAG[$r][$c++] = $_;
                  if ($c == $NFM3) { $c = 0;  $r++; }
               }
            }
            #print "CDRAG  = \n";  print_2d(@CDRAG);
         } elsif ($fields[0] eq "RFAREA") {
            $RFAREA = $fields[1];   #ignore other two fields
         } elsif ($fields[0] eq "NMACH1") {
            $NMACH1 = $fields[1];
         } elsif ($fields[0] eq "MCHVAL") {
            shift(@fields);
            $c = 0;
            foreach (@fields) { $MCHVAL[$c++] = $_; }
            while ($c < $NMACH1) {
               get_line_fields();
               foreach (@fields) { $MCHVAL[$c++] = $_; }
            }
         } elsif ($fields[0] eq "NHTS") {
            $NHTS = $fields[1];
         } elsif ($fields[0] eq "HTVEC") {
            shift(@fields);
            $c = 0;
            foreach (@fields) { $HTVEC[$c++] = $_; }
            while ($c < $NHTS) {
               get_line_fields();
               foreach (@fields) { $HTVEC[$c++] = $_; }
            }
         } elsif ($fields[0] eq "TIDLE") {
            # TIDLE[NHTS][NMACH1]
            shift(@fields);
            $c = 0;  $r = 0;
            foreach (@fields) {
               $TIDLE[$r][$c++] = $_;
               if ($c == $NMACH1) { $c = 0;  $r++; }
            }
            while ($r < $NHTS) {
               get_line_fields();
               foreach (@fields) {
                  $TIDLE[$r][$c++] = $_;
                  if ($c == $NMACH1) { $c = 0;  $r++; }
               }
            }
            #print "TIDLE  = \n";  print_2d(@TIDLE);
         } elsif ($fields[0] eq "TMIL") {
            # TMIL[NHTS][NMACH1]
            shift(@fields);
            $c = 0;  $r = 0;
            foreach (@fields) {
               $TMIL[$r][$c++] = $_;
               if ($c == $NMACH1) { $c = 0;  $r++; }
            }
            while ($r < $NHTS) {
               get_line_fields();
               foreach (@fields) {
                  $TMIL[$r][$c++] = $_;
                  if ($c == $NMACH1) { $c = 0;  $r++; }
               }
            }
            #print "TMIL  = \n";  print_2d(@TMIL);
         } elsif ($fields[0] eq "THRMX") {
            # THRMX[NHTS][NMACH1]
            shift(@fields);
            $c = 0;  $r = 0;
            foreach (@fields) {
               $THRMX[$r][$c++] = $_;
               if ($c == $NMACH1) { $c = 0;  $r++; }
            }
            while ($r < $NHTS) {
               get_line_fields();
               foreach (@fields) {
                  $THRMX[$r][$c++] = $_;
                  if ($c == $NMACH1) { $c = 0;  $r++; }
               }
            }
            #print "THRMX  = \n";  print_2d(@THRMX);
         } elsif ($fields[0] eq "FLOIDL") {
            # FLOIDL[NHTS][NMACH1]
            shift(@fields);
            $c = 0;  $r = 0;
            foreach (@fields) {
               $FLOIDL[$r][$c++] = $_;
               if ($c == $NMACH1) { $c = 0;  $r++; }
            }
            while ($r < $NHTS) {
               get_line_fields();
               foreach (@fields) {
                  $FLOIDL[$r][$c++] = $_;
                  if ($c == $NMACH1) { $c = 0;  $r++; }
               }
            }
            #print "FLOIDL  = \n";  print_2d(@FLOIDL);
         } elsif ($fields[0] eq "FLOMIL") {
            # FLOMIL[NHTS][NMACH1]
            shift(@fields);
            $c = 0;  $r = 0;
            foreach (@fields) {
               $FLOMIL[$r][$c++] = $_;
               if ($c == $NMACH1) { $c = 0;  $r++; }
            }
            while ($r < $NHTS) {
               get_line_fields();
               foreach (@fields) {
                  $FLOMIL[$r][$c++] = $_;
                  if ($c == $NMACH1) { $c = 0;  $r++; }
               }
            }
            #print "FLOMIL  = \n";  print_2d(@FLOMIL);
         } elsif ($fields[0] eq "FLOMAX") {
            # FLOMAX[NHTS][NMACH1]
            shift(@fields);
            $c = 0;  $r = 0;
            foreach (@fields) {
               $FLOMAX[$r][$c++] = $_;
               if ($c == $NMACH1) { $c = 0;  $r++; }
            }
            while ($r < $NHTS) {
               get_line_fields();
               foreach (@fields) {
                  $FLOMAX[$r][$c++] = $_;
                  if ($c == $NMACH1) { $c = 0;  $r++; }
               }
            }
            #print "FLOMAX  = \n";  print_2d(@FLOMAX);
         } elsif ($fields[0] eq "GCAP") {
            # GCAP [2][NMACH1]
            shift(@fields);
            $c = 0;  $r = 0;
            foreach (@fields) {
               $GCAP[$r][$c++] = $_;
               if ($c == $NMACH1) { $c = 0;  $r++; }
            }
            while ($r < 2) {
               get_line_fields();
               foreach (@fields) {
                  $GCAP[$r][$c++] = $_;
                  if ($c == $NMACH1) { $c = 0;  $r++; }
               }
            }
            #print "GCAP  = \n";  print_2d(@GCAP);
         } elsif ($fields[0] eq "GCAP2") {
            # GCAP2 [2][NMACH1]
            shift(@fields);
            $c = 0;  $r = 0;
            foreach (@fields) {
               $GCAP2[$r][$c++] = $_;
               if ($c == $NMACH1) { $c = 0;  $r++; }
            }
            while ($r < 2) {
               get_line_fields();
               foreach (@fields) {
                  $GCAP2[$r][$c++] = $_;
                  if ($c == $NMACH1) { $c = 0;  $r++; }
               }
            }
            #print "GCAP2  = \n";  print_2d(@GCAP2);
         } elsif ($fields[0] eq "FMMAX") {
            $FMMAX = $fields[1];
         } elsif ($fields[0] eq "CASMAX") {
            $CASMAX = $fields[1];
         } elsif ($fields[0] eq "ABDT") {
            $ABDT = $fields[1];
         } elsif ($fields[0] eq "DTGGUP") {
            $DTGGUP = $fields[1];
         } elsif ($fields[0] eq "DTGGDN") {
            $DTGGDN = $fields[1];
         } elsif ($fields[0] eq "ABCORl") {
            $ABCORl = $fields[1];
         } elsif ($fields[0] eq "DDMM") {
            $DDMM = $fields[1];
         } elsif ($fields[0] eq "DDTD") {
            $DDTD = $fields[1];
         } else {
            #dont worry about GTIDLE, GTMIL, GTHRMX, or other commands, for now
         }
      }
   }
   elsif ($fields[0] eq "PHYS")
   {
      print "starting processing the PHYS block\n";
      while (get_line_fields())
      {
         if (scalar(@fields) <= 0) {
            #blank line, ignore it
         } elsif ($fields[0] eq "ENDBLOCK" && $fields[1] eq "PHYS") {
            print "finished processing the PHYS block\n";
            last;
         } elsif ($fields[0] eq "PAREA") {
            $PAREA[0] = $fields[1];
            $PAREA[1] = $fields[2];
            $PAREA[2] = $fields[3];
         } elsif ($fields[0] eq "VAREA") {
            $VAREA[0] = $fields[1];
            $VAREA[1] = $fields[2];
            $VAREA[2] = $fields[3];
         } elsif ($fields[0] eq "DESWT") {
            $DESWT = $fields[1];
         } else {
            #ignore line, misunderstood
         }
      }
   }
   elsif ($fields[0] eq "RF")
   {
      print "found the RF block, ignoring for now\n";
   }
   elsif ($fields[0] eq "IR")
   {
      print "found the IR block, ignoring for now\n";
   }
   elsif ($fields[0] eq "END" && $fields[1] eq "AIRCRAFT")
   {
      print "found the END AIRCRAFT definition\n";
   }
   else
   {
      #unknown line, maybe comment?  skip it
   }

#   foreach (@fields)
#   {
#      $matrix[$r][$c] = $_;
#      # print $_;
#      $c++;
#      if ($c == $COLS)
#      {
#         $c = 0;
#         $r++;
#      }
#   }
}
close($data) || die "Couldn't close '$file' properly";

my $outfile = "BRAWLER_".$aircraft_name.".txt";
open(my $data_out, '>', $outfile);
print $data_out "\n";
print $data_out "p6dof_object_types\n";
print $data_out "\n";
my $engine_type = "BRAWLER_".$aircraft_name."_engine";
print $data_out "jet_engine_type ".$engine_type." P6DOF_JET_ENGINE\n";
print $data_out "\n";

#find closest points to mach 1 and 20k ft alt
my $diff = 99999999999999.9;
my $best_alt_index = 0;
for $r (0 .. ($NHTS-1)) {
   my $delta = abs(20000-$HTVEC[$r]);
   if ($delta < $diff) {
      $diff = $delta;
      $best_alt_index = $r;
   }
}
$diff = 99999999999999.9;
my $best_mach_index = 0;
for $r (0 .. ($NMACH1-1)) {
   my $delta = abs(1-$MCHVAL[$r]);
   if ($delta < $diff) {
      $diff = $delta;
      $best_mach_index = $r;
   }
}
$r = $best_alt_index;
$c = $best_mach_index;
#note BRAWLER fuel flow is in slugs/sec
my $pounds_per_slug = 32.1740486;
my $seconds_per_hour = 3600.0;
my $temp = ($pounds_per_slug * $seconds_per_hour * $FLOIDL[$r][$c]/$TIDLE[$r][$c]);
print $data_out "\ttsfc_idle_pph  $temp\t#using FLOIDL[$r][$c]/TIDLE[$r][$c], (mach ".sprintf("%.2f", $MCHVAL[$c]).", alt ".sprintf("%.1f", $HTVEC[$r])." ft)\n";
$temp = ($pounds_per_slug * $seconds_per_hour * $FLOMIL[$r][$c]/$TMIL[$r][$c]);
print $data_out "\ttsfc_mil_pph   $temp\t#using FLOMIL[$r][$c]/TMIL[$r][$c], (mach ".sprintf("%.2f", $MCHVAL[$c]).", alt ".sprintf("%.1f", $HTVEC[$r])." ft)\n";
$temp = ($pounds_per_slug * $seconds_per_hour * $FLOMAX[$r][$c]/$THRMX[$r][$c]);
print $data_out "\ttsfc_ab_pph    $temp\t#using FLOMAX[$r][$c]/THRMX[$r][$c], (mach ".sprintf("%.2f", $MCHVAL[$c]).", alt ".sprintf("%.1f", $HTVEC[$r])." ft)\n";
print $data_out "\n";
print $data_out "\trated_thrust_idle  ".sprintf("%.2f", $TIDLE[$r][$c])." lbf\t#using TIDLE[$r][$c], (mach ".sprintf("%.2f", $MCHVAL[$c]).", alt ".sprintf("%.1f", $HTVEC[$r])." ft)\n";
print $data_out "\trated_thrust_mil   ".sprintf("%.2f", $TMIL[$r][$c])." lbf\t#using TMIL[$r][$c], (mach ".sprintf("%.2f", $MCHVAL[$c]).", alt ".sprintf("%.1f", $HTVEC[$r])." ft)\n";
print $data_out "\trated_thrust_ab    ".sprintf("%.2f", $THRMX[$r][$c])." lbf\t#using THRMX[$r][$c], (mach ".sprintf("%.2f", $MCHVAL[$c]).", alt ".sprintf("%.1f", $HTVEC[$r])." ft)\n";
print $data_out "\n";
print $data_out "\tspin_up_mil_per_sec    ".sprintf("%.3f", $DTGGUP)."\t#DTGGUP\n";
print $data_out "\tspin_down_mil_per_sec  ".sprintf("%.3f", $DTGGDN)."\t#DTGGDN\n";
print $data_out "\tspin_up_ab_per_sec     ".sprintf("%.3f", $ABDT)."\t#ABDT\n";
print $data_out "\tspin_down_ab_per_sec   ".sprintf("%.3f", $ABDT)."\t#ABDT\n";
print $data_out "\t#current_fuel_feed      MainFuelTank\t#GUESS\n";
print $data_out "\n";
print $data_out "\t#using TIDLE[][$c], mach = ".sprintf("%.3f", $MCHVAL[$c]).", use other column if desired\n";
print $data_out "\tthrust_table_idle\n";
print $data_out "\t\t#alt_feet\tthrust_lbs\n";
for $r (0 .. ($NHTS-1))
{
   print $data_out "\t\t".sprintf("%.3f", $HTVEC[$r])."\t".sprintf("%.3f", $TIDLE[$r][$c])."\n";
}
print $data_out "\tend_thrust_table_idle\n";
print $data_out "\n";
print $data_out "\t#using TMIL[][$c], mach = ".sprintf("%.3f", $MCHVAL[$c]).", use other column if desired\n";
print $data_out "\tthrust_table_mil\n";
print $data_out "\t\t#alt_feet\tthrust_lbs\n";
for $r (0 .. ($NHTS-1))
{
   print $data_out "\t\t".sprintf("%.3f", $HTVEC[$r])."\t".sprintf("%.3f", $TMIL[$r][$c])."\n";
}
print $data_out "\tend_thrust_table_mil\n";
print $data_out "\n";
print $data_out "\t#using THRMX[][$c], mach = ".sprintf("%.3f", $MCHVAL[$c]).", use other column if desired\n";
print $data_out "\tthrust_table_ab\n";
print $data_out "\t\t#alt_feet\tthrust_lbs\n";
for $r (0 .. ($NHTS-1))
{
   print $data_out "\t\t".sprintf("%.3f", $HTVEC[$r])."\t".sprintf("%.3f", $THRMX[$r][$c])."\n";
}
print $data_out "\tend_thrust_table_ab\n";
print $data_out "\n";
print $data_out "\tthrottle_setting_mil ThrottleMilitary\t#GUESS\n";
print $data_out "\tthrottle_setting_ab  ThrottleAfterburner\t#GUESS\n";
print $data_out "\n";
print $data_out "end_jet_engine_type\n";
print $data_out "\n";
print $data_out "\n";
print $data_out "p6dof_object_type  BRAWLER_".$aircraft_name."  P6DOF_BASE_TYPE\n";
print $data_out "\n";
print $data_out "\tobject_graphical_type_num  1\n";
print $data_out "\t#use_spherical_earth        true\n";
print $data_out "\tempty_mass                 ".sprintf("%.3f", $DESWT*$pounds_per_slug)." lbs  #DESWT\n";
print $data_out "\n";
print $data_out "\tempty_moment_of_inertia_ixx  25000.00 slug*ft^2 #GUESS\n";
print $data_out "\tempty_moment_of_inertia_iyy  120000.0 slug*ft^2 #GUESS\n";
print $data_out "\tempty_moment_of_inertia_izz  140000.0 slug*ft^2 #GUESS\n";  
print $data_out "\n";
print $data_out "\tobservation_offset_x         0.0 m  #GUESS\n";
print $data_out "\tobservation_offset_y         0.0 m  #GUESS\n";
print $data_out "\tobservation_offset_z         0.0 m  #GUESS\n";
print $data_out "\teyept_offset_x               0.0 m  #GUESS\n";
print $data_out "\teyept_offset_y               0.0 m  #GUESS\n";
print $data_out "\teyept_offset_z               0.0 m  #GUESS\n";
print $data_out "\n";

print $data_out "\taero_data\n";
print $data_out "\n";
my $wing_chord = sqrt($RFAREA);
my $wing_span  = 2 * $wing_chord;
print $data_out "\t\twing_chord_ft\t".sprintf("%.3f",$wing_chord)."  #GUESS: known area, but assumes chord = half of span\n";
print $data_out "\t\twing_span_ft\t".sprintf("%.3f",$wing_span)."  #GUESS: known area, but assumes span = twice of chord\n";
print $data_out "\t\twing_area_sqft\t".sprintf("%.6f",$RFAREA)."  #RFAREA\n";
print $data_out "\n";
print $data_out "\t\tcl_alpha_mach_table\n";
print $data_out "\n";
print $data_out "\t\t\tirregular_table\n";
print $data_out "\t\t\tindependent_variable mach precision float\n";
print $data_out "\t\t\tindependent_variable alpha units rad\n";
print $data_out "\t\t\tdependent_variable precision float\n";
print $data_out "\n";
# CLIFT [NALPH1] [NFM2],   FMVEC2 [NFM2],   # ALFV1 [NALPH1]
for $c (0 .. ($NFM2-1)) {
   print $data_out "\t\t\tmach ".sprintf("%.6f",$FMVEC2[$c])."\n";
   print $data_out "\t\t\t\talpha ";
   for $r (0 .. ($NALPH1-1)) {
      print $data_out " ".sprintf("%.6f", $ALFV1[$r]);
   }
   print $data_out "\n\t\t\t\tvalues";
   for $r (0 .. ($NALPH1-1)) {
      print $data_out " ".sprintf("%.6f", $CLIFT[$r][$c]);
   }
   print $data_out "\n";
}
print $data_out "\t\t\tend_irregular_table\n";
print $data_out "\t\tend_cl_alpha_mach_table\n";
print $data_out "\n";
print $data_out "\t\tcd_alpha_mach_table\n";
print $data_out "\n";
print $data_out "\t\t\tirregular_table\n";
print $data_out "\t\t\tindependent_variable mach precision float\n";
print $data_out "\t\t\tindependent_variable alpha units rad\n";
print $data_out "\t\t\tdependent_variable precision float\n";
print $data_out "\n";
# CDRAG [NALPH2] [NFM3],   FMVEC3 [NFM3],   # ALFV2 [NALPH2]
for $c (0 .. ($NFM3-1)) {
   print $data_out "\t\t\tmach ".sprintf("%.6f",$FMVEC3[$c])."\n";
   print $data_out "\t\t\t\talpha ";
   for $r (0 .. ($NALPH2-1)) {
      print $data_out " ".sprintf("%.6f", $ALFV2[$r]);
   }
   print $data_out "\n\t\t\t\tvalues";
   for $r (0 .. ($NALPH2-1)) {
      print $data_out " ".sprintf("%.6f", $CDRAG[$r][$c]);
   }
   print $data_out "\n";
}
print $data_out "\t\t\tend_irregular_table\n";
print $data_out "\t\tend_cd_alpha_mach_table\n";
print $data_out "\t\tinclude BRAWLER_secondary_aero_data.txt\t#GUESS\n";
print $data_out "\tend_aero_data\n";
print $data_out "\tinclude BRAWLER_aero_components.txt\t\t#GUESS\n";
print $data_out "\tinclude BRAWLER_controls_providers.txt\t#GUESS\n";
print $data_out "\tinclude BRAWLER_flight_controls.txt\t\t#GUESS\n";
print $data_out "\n";
print $data_out "\tpropulsion_data\n";
print $data_out "\t\tjet_engine  BothEngines  ".$engine_type."\n";
print $data_out "\t\t\trel_pos_x  0.0 ft\t#GUESS\n";
print $data_out "\t\t\trel_pos_y  0.0 ft\t#GUESS\n";
print $data_out "\t\t\trel_pos_z  0.0 ft\t#GUESS\n";
print $data_out "\t\t\trel_yaw    0.0 deg\t#GUESS\n";
print $data_out "\t\t\trel_pitch  0.0 deg\t#GUESS\n";
print $data_out "\t\t\trel_roll   0.0 deg\t#GUESS\n";
print $data_out "\t\t\tfuel_feed  MainFuelTank\t#GUESS\n";
print $data_out "\t\tend_jet_engine\n";
print $data_out "\n";
#TODO - use brawler "AC_FUEL_CAP" value, if valid.  how do I know if valid?
print $data_out "\t\tfuel_tank\n";
print $data_out "\t\t\ttank_name              MainFuelTank\t#GUESS\n";
print $data_out "\t\t\tmax_fuel_quantity      13455.0 lbs\t#GUESS (wikipedia F15c)\n";
print $data_out "\t\t\tcurrent_fuel_quantity  13455.0 lbs\t#GUESS\n";
print $data_out "\t\t\tmax_flow_rate          40.0 lbs/sec\t#GUESS\n";
print $data_out "\t\t\tmax_fill_rate          40.0 lbs/sec\t#GUESS\n";
print $data_out "\t\t\tmax_xfer_rate          40.0 lbs/sec\t#GUESS\n";
print $data_out "\t\t\tcg_full_x              0.0 ft\t#GUESS\n";
print $data_out "\t\t\tcg_full_y              0.0 ft\t#GUESS\n";
print $data_out "\t\t\tcg_full_z              0.0 ft\t#GUESS\n";
print $data_out "\t\t\tcg_empty_x             0.0 ft\t#GUESS\n";
print $data_out "\t\t\tcg_empty_y             0.0 ft\t#GUESS\n";
print $data_out "\t\t\tcg_empty_z             0.0 ft\t#GUESS\n";
print $data_out "\t\tend_fuel_tank\n";
print $data_out "\n";
print $data_out "\t\t#throttle_setting_mil    ThrottleMilitary\n";
print $data_out "\t\t#throttle_setting_ab     ThrottleAfterburner\n";
print $data_out "\tend_propulsion_data\n";
print $data_out "\n";
print $data_out "end_p6dof_object_type\n";
print $data_out "\n";
print $data_out "end_p6dof_object_types\n";
#
#for $c (0 .. ($COLS-1))       # For each column...
#{
#   for $r (0 .. ($ROWS-1))    # For each row...
#   {
#      my $val = sprintf("%.6f", $matrix[$r][$c]);
#      print $data_out "$val ";
#   }
#   print $data_out "\n";
#}
close($data_out) || die "Couldn't close '$outfile' properly";
