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

# Extract all of the events out of a WSF event log that involve 
# a specified player.
# The selected events are written exactly as read - no combining of multi-line events
# is performed (use ev-combine.pl for that).
#
# The result is written to standard output.

die("Usage: perl extract.pl input-file player-name\n") if ($#ARGV < 1);

$infilename = $ARGV[0];
$playerName = $ARGV[1];

open(INFH, $infilename) or die("** ERROR: File \"$infilename\" could not be opened\n");

while (<INFH>)
{
    chomp;                             # Removes new lines from $_
    @field = split;                    # Break out the fields in $_
    $selected = 0;
    if (($#field >= 2) && ($field[2] =~ $playerName))
    {
        $selected = 1;
    }
    elsif (($#field >= 3) && ($field[3] =~ $playerName))
    {
        $selected = 1;
    }
    elsif (($#field >= 4) && ($field[4] =~ $playerName))
    {
        $selected = 1;
    }
    $done = 0;
    do
    {
        print "$_\n" if $selected;
        # If this line doesn't end with a '\', read the next line.
        $done = 1;
        if (/\\$/)
        {
            $done = 0;
            $_ = <INFH>;
            chomp;
        }
    } until $done;
}
