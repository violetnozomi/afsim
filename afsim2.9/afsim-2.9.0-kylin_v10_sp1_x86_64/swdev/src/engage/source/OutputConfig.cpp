// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#include "OutputConfig.hpp"

#include "Simulation.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtStringUtil.hpp"

namespace engage
{

// =================================================================================================
OutputConfig::OutputConfig()
   : mFileName()
   , mFormat(cMINIMAL)
   , mPhase(cALL)
   , mEventOutput(false)
   , mSummaryOutput(false)
   , mRateTableName("default")
   , mRateTable()
   , mOutputItems()
{
}

// =================================================================================================
bool OutputConfig::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "file")
   {
      aInput.ReadValueQuoted(mFileName);
   }
   else if (command == "format")
   {
      throw UtInput::UnknownCommand(aInput); // TODO-UNKNOWN FOR NOW...
   }
   else if (command == "phase")
   {
      std::string phase;
      UtStringUtil::ToLower(phase);
      aInput.ReadValue(phase);
      if (phase == "all")
      {
         mPhase = cALL;
      }
      else if (phase == "acquiring")
      {
         mPhase = cACQUIRING;
      }
      else if (phase == "tracking")
      {
         mPhase = cTRACKING;
      }
      else if (phase == "flying")
      {
         mPhase = cFLYING;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "event_output")
   {
      aInput.ReadValue(mEventOutput);
   }
   else if (command == "summary_output")
   {
      aInput.ReadValue(mSummaryOutput);
   }
   else if (command == "rate_table_name")
   {
      aInput.ReadValue(mRateTableName);
   }
   else if (command == "items")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         OutputItem outputItem;
         if (!outputItem.ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
         mOutputItems.push_back(outputItem);
      }
   }
   else if (command == "events")
   {
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         EventItem eventItem;
         if (!eventItem.ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
         mEventItems.push_back(eventItem);
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
//! Determine if periodic output is to performed at the current time.
//! @param aPhase     [input] The current simulation phase.
//! @param aSimTime   [input] The current simulation time.
//! @param aLastOutputTime [input] The last simulation time when periodic output was last performed.
//! @returns 'true' if periodic output should be performed, or false if not.
bool OutputConfig::DoPeriodicOutput(Phase aPhase, double aSimTime, double aLastOutputTime) const
{
   // Periodic output is performed only if all of the following conditions are met.
   // 1) A file has been specified.
   // 2) This output object is 'active' for the current simulation phase.
   // 3) The output period has expired (i.e.: it's time to output the next sample).

   if (mFileName.empty())
   {
      return false;
   }

   if ((mPhase != aPhase) && (mPhase != cALL))
   {
      return false;
   }

   double outputPeriod     = mRateTable.GetPeriod(aSimTime);
   double nextOutputTime   = aLastOutputTime + outputPeriod;
   double timeToNextOutput = nextOutputTime - aSimTime;
   if (timeToNextOutput > 1.0E-8)
   {
      return false;
   }

   // All conditions have passed so periodic output for this time is valid.
   return true;
}

} // namespace engage

//
/*
Most of the variables output by default are the same for both output formats. If you want to print only the default
variables in the default style (column output), you will not have to make any adjustments. If you want to tailor the
output to display specific variables, or to output in block format, there are a few PROGC variables whose values will
need to be changed. Some of these changes are common to both styles of output and will be discussed here. Changes that
apply only to one style or the other will be discussed in the appropriate sections. The following is a list of the
variables that may need to be modified. They are input with the PROGC overlay. Variable    Effects  Default  Notes FLVAR
Floating point output   See below FXVAR    Integer output    Blank    Not output unless requested TZFACT   Multiplies
floating point values See Column Output Section  Only used for column output style PTRFXV   Location of integer
variables    21, 21, 21, 7*0   Adjusted only if want to output integers 21 = no integers to print PTRCHV   Printing of
STATUS (which has meaning only in acq and track phases)  3, 5, 1, 7*0   3 = print for acquisition mode 5 = print for
tracking mode 1 = don't print If you want to change the default output list, use FLVAR to add or remove floating point
output variables and FXVAR to add integer output variables. There are default output variables for the acquisition,
track, flyout, and search phases of the simulation. With one exception, they are all contained in FLVAR.

---------------------------------

These variables are stored, in order, in the input variable FLVAR(1) to FLVAR(54). See Section 5 for more details.

Variable    Description
ACC1        Sensed acceleration in fin plane 1
ACC2        Sensed acceleration in fin plane 2
AINERT      Missile moment of inertia
ALPHA1      Fin plane 1 angle of attack
ALPHA2      Fin plane 2 angle of attack
AZERR       Azimuth error due to multipath and clutter
CG          Missile CG offset along roll axis
CL1         Coefficient of lift in plane 1
CL2         Coefficient of lift in plane 2
CM1         Moment in plane 1
CM2         Moment in plane 2
CON1        Fin 1 deflection
CON2        Fin 2 deflection
EK1         Autopilot command in fin plane 1
EK2         Autopilot command in fin plane 2
ELERR       Elevation error due to multipath and clutter
FA1         Body aero force vector
FA2         Body aero force vector
FA3         Body aero force vector
FD          Force due to drag (kg)
FINDEL      Fin deflection
FMACH       Missile mach number
FMASS       Missile mass
FTIME       Missile flight time
GDCMP       Pitch guidance command
GDCMY       Yaw guidance command
GEE         Missile g's HP Pitch guidance error
HY          Yaw guidance error
OMEG1       Body rate
OMEG2       Body rate
OMEG3       Body rate
OMEGD2      Body angular acceleration
OMEGD3      Body angular acceleration
PHI         Missile roll angle in radians
PSI         Missile heading angle in radians
Q           Dynamic pressure
SNR         Signal to noise ratio
THET        Missile pitch angle in radians
THRUST      Missile thrust
TIME        Target flight time (SAM's run time)
V           Missile velocity (magnitude)
X           Missile location x
XDDOT       Missile acceleration x
XDOT        Missile velocity x
XT          Target location x
Y           Missile location y
YDDOT       Missile acceleration y
YDOT        Missile velocity y
YT          Target location y
Z           Missile location z
ZDDOT       Missile acceleration z
ZDOT        Missile velocity z
ZT          Target location z
ZTER        Altitude of terrain at missile location

*** Acquisition mode variables.

The default output variables for the acquisition phase are, in order in FLVAR(61) to FLVAR(72) (1-based)

TIME     Target flight time (SAMS run time)
RGATES   [1]   Range gate center
RGTERR   [1]   Range tracking error
VCOOUT   [1]   Doppler filter center frequency
DGTERR   [1]   Doppler tracking error
BOREAZ   [1]   Boresight azimuth angle
AZTERR   [1]   Azimuth tracking error
BOREEL   [1]   Boresight elevation angle
ELTERR   [1]   Elevation tracking error
SNMDB    [1]   Measured signal to noise ratio in dB
SCRDB    [1]   Signal to clutter ratio in dB
RCSDB    [1]   Radar cross section seen by sensor in dB
STATUS   [1]   Status of acquisition (NO DET / DETECT) (not in FLVAR)

FLVAR(73) to FLVAR(79) may also be defined by the user (19 variables total)
FLVAR(80) must be zero.

*** Track mode

Stored in FLVAR(81) to FLVAR(92) (1-based)

TIME     Target flight time (SAMS run time)
RGATES   [2]   Range gate center
RGTERR   [2]   Range tracking error
VCOOUT   [2]   Doppler filter center frequency
DGTERR   [2]   Doppler tracking error
BOREAZ   [2]   Boresight azimuth angle
AZTERR   [2]   Azimuth tracking error
BOREEL   [2]   Boresight elevation angle
ELTERR   [2]   Elevation tracking error
SNMDB    [2]   Measured signal to noise ratio in dB
SCRDB    [2]   Signal to clutter ratio in dB
RCSDB    [2]   Radar cross section seen by sensor in dB
STATUS   [2]   Status of acquisition (NO TRK / TRACK) (not in FLVAR)

FLVAR(93) to FLVAR(99) may also be defined by the user. (19 variables total)
FLVAR(100) must be zero.

*** Flyout mode variables

The default output variables for the flyout phase are (in order) in FLVAR(101) to FLVAR(115):

TIME           Target flight time (SAMS run time)
FTIME          Missile flight time
X              Missile location in x
Y              Missile location in y
Z              Missile location in z
V              Missile velocity (magnitude)
GEE            Missile g force
SNR            Signal-to-noise ratio
XT             Target location in x
YT             Target location in y
ZT             Target location in z
RTM            Distance from missile to target
FMACH          Missile mach number
AZERR          Azimuth error of tracker/seeker
ELERR          Elevation error of tracker/seeker

FLVAR(116) to FLVAR(200) may also be defined by the user.
FLVAR(201) must be zero.

Note that the following Fortran formats are used when these are displayed for column output.
The first line is for the acquisition phase, second line is for track phase, and the third is for the missile flyout
phase. Lines 4-10 need to be defined if you are using the user output file option. They will be referred to as line
formats to distinguish them from the block and column output formats. (1X,F9.3,4F12.2,2(F8.2,F10.4),3(1X,F6.1),1X,A6)
(1X,F9.3,4F12.2,2(F8.2,F10.4),3(1X,F6.1),1X,A6)
(1X,2F7.2, 2F9.1, 2F8.1, F7.3, F8.2, 7(1X,F10.2))
(1X, F7.2, 9(1X,F12.2))

FLVAR is an array containing these output variables in specific locations. The acquisition mode variables are located in
FLVAR(61) to FLVAR(72). Track mode variables are located in FLVAR(81) to FLVAR(92). Flyout variables are located in
FLVAR(101) to FLVAR(115).  When you use FLVAR to specify inputs, you must specify the location of the first variable
that you are replacing. ESAMS assumes that any following variables overwrite the default variables in sequence. For
instance, if you were to include the line

flvar <103> xdot, ydot, zdot

in the input file, then the variables displayed for flyout will be TIME, FTIME, XDOT, YDOT, and ZDOT, followed by V,
GEE, and the rest of the following default values. If you want only the first five of these variables output, then
include a 0 at the end of the input line as in the example below.

flvar <103> xdot, ydot, zdot, 0

The 0 signals to ESAMS that it has reached the end of the variables that you want output for the flyout phase. Note that
the subscript for an input variable is denoted with angled bracket, < >. At least one space between the variable name
and the brackets is necessary. For output, you may choose any variable from the lists in Appendix C: Output Variables.
Also, you are not limited to only 12 or 15 output variables. If you like, you can output up to 19 variables for the
acquisition and tracking phases and up to 100 for the flyout phase. You just need to specify where you want to start
overwriting the default values, and you must make sure that there is at least one space in FLVAR between the end of one
list and the start of the next. For instance, if you include the line

flvar <73> hpanga, hpange

then the acquisition phase output list will include all of the default values with HPANGA and
HPANGE added onto the end. Also, this list ends at FLVAR(74) and the next list (for the tracking phase) doesn't start
until FLVAR(81), so there is definitely a blank value between the two lists.

You can define up to 20 integer variables for output by modifying FXVAR. You will also have to adjust PTRFXV. As with
PRINTM, PTRFXV has ten slots, one for each output file. By default, the first three correspond, in order, to the
acquisition, track, and flyout phases. PTRFXV tells ESAMS where in FXVAR to look for the start of the integer variable
list for each output file. Let's say that you want to print out the integer variable MASKED during the tracking phase
and the integer variable LOALT during the flyout phase. You can accomplish this by adding the following four lines of
input to the PROGC overlay:

Here the track phase variable is in FXVAR(1) and the flyout phase variable is in FXVAR(5). There should be at least one
space in FXVAR between the end of one list and beginning of the next list; in this example there are three spaces,
FXVAR(2) to FXVAR(4). The output will contain the default floating point output with the integer variables added to the
end. Adding the following lines gives the same result:

PTRFXV   <2>   1 5
FXVAR    <1>   MASKED, 0, R2, LOALT

These give the same result because the input logic reads in arrays sequentially; the number following the value put into
the second slot is automatically put into the third slot. The "R2" is a repeat count telling ESAMS to put the previous
value, 0, into the next two slots.

When you change the default input for the column format, you may need to change the line format of the output. This is
discussed in the Column subsection, page 51.

Sometimes you may not want to output the character variable STATUS. This is accomplished by modifying the values of
PTRCHV. Like PTRFXV, PTRCHV has ten slots, one for each output file. To turn it off, simply set the value in the slot
corresponding to the desired output file to 1. For instance, if you want to leave it out entirely, you add the following
to the PROGC overlay: PTRCHV 1 1

If an array is input without mentioning a subscript, ESAMS starts with subscript 1. This line tells ESAMS that PTRCHV(1)
and PTRCHV(2) are both set to 1.
*/
