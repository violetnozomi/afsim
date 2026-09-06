// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalLaunchComputer.hpp"

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "UtCalendar.hpp"
#include "UtCentralBody.hpp"
#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtOrbitalElements.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfDateTime.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfWeapon.hpp"

// =================================================================================================
WsfOrbitalLaunchComputer::WsfOrbitalLaunchComputer()
   : WsfLaunchComputer()
   , mLEO_DataFileName()
   , mLEO_Data()
   , mLastResult()
{
}

// =================================================================================================
// Required script upcasting works
const char* WsfOrbitalLaunchComputer::GetScriptClassName() const
{
   return "WsfOrbitalLaunchComputer";
}

// =================================================================================================
WsfLaunchComputer* WsfOrbitalLaunchComputer::Clone() const
{
   return new WsfOrbitalLaunchComputer(*this);
}

// =================================================================================================
bool WsfOrbitalLaunchComputer::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "leo_data")
   {
      aInput.ReadValueQuoted(mLEO_DataFileName);
      mLEO_DataFileName = aInput.LocateFile(mLEO_DataFileName);
   }
   else
   {
      myCommand = WsfLaunchComputer::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
bool WsfOrbitalLaunchComputer::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   bool ok(true);

   // aWeaponPtr is 0 if the launch computer is being called independently for analysis.
   if (aWeaponPtr != nullptr)
   {
      ok = WsfLaunchComputer::Initialize(aSimTime, aWeaponPtr);
   }

   if (!mLEO_DataFileName.empty())
   {
      ok &= ReadOrbits(mLEO_DataFileName, mLEO_Data);
   }
   return ok;
}

// =================================================================================================
double WsfOrbitalLaunchComputer::EstimatedTimeToIntercept(double          aSimTime,
                                                          const WsfTrack& aTrack,
                                                          double          aLaunchDelayTime) // override
{
   return 0.0; // TODO
}

// =================================================================================================
bool WsfOrbitalLaunchComputer::ComputeLEO_Insertion(double aSimTime, double aAltitude, double aInclination)
{
   if (ShowResults())
   {
      auto out = ut::log::info() << "Computing LEO Insertion.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Altitude: " << aAltitude * 0.001 << " km";
      out.AddNote() << "Inclination: " << aInclination * UtMath::cDEG_PER_RAD << " deg";
   }

   // Build a list of the best orbits along each launch heading.
   // TODO It would probably be good to have some sort of index, but this isn't used that much.

   // A list of the *best* orbits along each launch heading.
   std::vector<OrbitListIter> orbits;

   double bestAltDelta     = 1.0E+30;
   double curLaunchHeading = 1.0E+30;
   for (std::list<Orbit>::const_iterator oli = mLEO_Data.begin(); oli != mLEO_Data.end(); ++oli)
   {
      const Orbit& o = *oli;
      if (curLaunchHeading != o.mLaunchHeading)
      {
         orbits.push_back(oli);
         curLaunchHeading = o.mLaunchHeading;
         bestAltDelta     = 1.0E+30;
      }
      double altDelta = fabs(aAltitude - o.mAlt);
      if (altDelta < bestAltDelta)
      {
         orbits.back() = oli;
         bestAltDelta  = altDelta;
      }
   }

   // Find the launch heading that has the best inclination.
   size_t bestIncIndex = orbits.size();
   double bestIncDelta = 1.0E+30;
   for (size_t i = 0; i < orbits.size(); ++i)
   {
      const Orbit& o        = *(orbits[i]);
      double       incDelta = fabs(aInclination - o.mInclination);
      if (incDelta < bestIncDelta)
      {
         bestIncIndex = i;
         bestIncDelta = incDelta;
      }
   }

   if (bestIncIndex >= orbits.size())
   {
      mLastResult = Orbit();
      ut::log::info() << "No solution for launch heading that has best inclination.";
   }

   // Determine the two launch headings that contain the desired inclination.

   size_t h1Index      = bestIncIndex;
   size_t h2Index      = bestIncIndex;
   bool   foundBracket = false;
   if (bestIncIndex > 0) // There is a previous launch heading.
   {
      --h1Index; // index of best entry for previous launch heading.
      OrbitListIter oli1 = orbits[h1Index];
      OrbitListIter oli2 = orbits[h2Index];
      double        inc1 = oli1->mInclination;
      double        inc2 = oli2->mInclination;
      if (((inc1 <= aInclination) && (aInclination <= inc2)) || ((inc2 <= aInclination) && (aInclination <= inc1)))
      {
         foundBracket = true;
      }
   }
   if ((!foundBracket) && ((bestIncIndex + 1) < orbits.size()))
   {
      ++h2Index; // index of best entry for next launch heading.
      OrbitListIter oli1 = orbits[h1Index];
      OrbitListIter oli2 = orbits[h2Index];
      double        inc1 = oli1->mInclination;
      double        inc2 = oli2->mInclination;
      if (((inc1 <= aInclination) && (aInclination <= inc2)) || ((inc2 <= aInclination) && (aInclination <= inc1)))
      {
         foundBracket = true;
      }
   }
   Orbit result;
   if (foundBracket)
   {
      // Two launch headings involved. Interpolate along each and then interpolate between.
      // if (DebugEnabled())
      //{
      //   cout << "***** Two Heading Interpolation: Line 1" << endl;
      //}
      Orbit result1;
      InterpolateAlongLaunchHeading(result1, orbits[h1Index], aAltitude);
      // if (DebugEnabled())
      //{
      //    cout << "***** Two Heading Interpolation: Line 2" << endl;
      // }
      Orbit result2;
      InterpolateAlongLaunchHeading(result2, orbits[h2Index], aAltitude);
      // if (DebugEnabled())
      //{
      //    cout << "***** Combined Interpolation" << endl;
      // }
      InterpolateOrbits(result, result1, result2, cINCLINATION, aInclination);
   }
   else
   {
      // Only a single launch heading involved.
      // if (DebugEnabled())
      //{
      //   cout << "***** Single Heading Interpolation" << endl;
      //}
      InterpolateAlongLaunchHeading(result, orbits[h1Index], aAltitude);
   }

   // Propagate the results into the launch computer.

   // Convert the launch heading into a platform-relative bearing.
   WsfWeapon* weaponPtr = GetWeapon();
   if (weaponPtr != nullptr)
   {
      WsfPlatform* platformPtr = weaponPtr->GetPlatform();
      weaponPtr->ClearCueing();
      weaponPtr->ClearTransientCue();
      double launchHeading = result.mLaunchHeading;
      double locNED[3]     = {100000.0 * cos(launchHeading), 100000.0 * sin(launchHeading), 0.0};
      double relLocWCS[3];
      platformPtr->ConvertNEDVectorToWCS(relLocWCS, locNED);
      double azECS;
      double elECS;
      platformPtr->ComputeAspect(relLocWCS, azECS, elECS);
      SetLauncherBearing(azECS);
   }
   SetLoftAngle(result.mLoftAngle);
   SetInterceptTime(result.mTimeOfFlight);
   WsfGeoPoint p(result.mLat, result.mLon, result.mAlt);
   SetInterceptPoint(p);

   mLastResult = result;
   if (ShowResults())
   {
      auto out = ut::log::info() << "Last Result:";
      mLastResult.Print(out);
   }
   return true;
}

// =================================================================================================
// Get the orbital elements for the last orbit determination.
void WsfOrbitalLaunchComputer::GetOrbitalElements(double aResults[6]) const
{
   UtCalendar calendar;
   // if ((GetWeapon() != 0) &&
   //     (GetWeapon()->GetSimulation() != 0))
   //{
   //    calendar = GetWeapon()->GetSimulation()->GetDateTime().GetStartDateAndTime();
   // }
   ComputeOrbitalElements(GetPlatform()->GetSimulation()->GetEnvironment().GetCentralBody(), aResults, mLastResult, calendar);
}

// =================================================================================================
// A utility method to compute the orbital elements from an Orbit and return them in a vector
// static
void WsfOrbitalLaunchComputer::ComputeOrbitalElements(const ut::CentralBody& aCentralBody,
                                                      double                 aResults[6],
                                                      const Orbit&           aOrbit,
                                                      UtCalendar&            aCalendar)
{
   UtEntity e;
   e.SetECI_ReferenceEpoch(aCalendar);
   e.SetTime(0.0);
   e.SetLocationLLA(aOrbit.mLat, aOrbit.mLon, aOrbit.mAlt);
   double speed     = aOrbit.mSpeedECI;
   double heading   = aOrbit.mHeading;
   double pitch     = aOrbit.mFlightPathAngle;
   double velD      = -speed * sin(pitch);
   double velN      = speed * cos(heading);
   double velE      = speed * sin(heading);
   double velNED[3] = {velN, velE, velD};
   e.SetVelocityNED(velNED);
   e.SetOrientationNED(heading, pitch, 0.0);
   double locECI[3];
   double velECI[3];
   e.GetLocationWCS(locECI);
   e.GetVelocityWCS(velECI);
   ComputeOrbitalElements(aCentralBody, aResults, locECI, velECI, aCalendar);
}

// =================================================================================================
// A utility method to compute the orbital elements and return them in a vector.
// static
void WsfOrbitalLaunchComputer::ComputeOrbitalElements(const ut::CentralBody& aCentralBody,
                                                      double                 aResults[6],
                                                      const double           aLocECI[3],
                                                      const double           aVelECI[3],
                                                      UtCalendar&            aCalendar)
{
   UtOrbitalElements oe(aCentralBody);
   oe.Publish(aLocECI, aVelECI, aCalendar);
   aResults[0] = oe.GetSemiMajorAxis();
   aResults[1] = oe.GetEccentricity();
   aResults[2] = oe.GetInclination();
   aResults[3] = oe.GetMeanAnomaly();
   aResults[4] = oe.GetRAAN();
   aResults[5] = oe.GetArgumentOfPeriapsis();
}

// =================================================================================================
// static
bool WsfOrbitalLaunchComputer::ReadOrbits(const std::string& aFileName, std::list<Orbit>& aOrbits)
{
   std::ifstream ifs(aFileName);
   if (!ifs.is_open())
   {
      auto out = ut::log::error() << "Unable to open file.";
      out.AddNote() << "File: " << aFileName;
   }

   int         lineCount(0);
   std::string line;
   while (std::getline(ifs, line))
   {
      ++lineCount;
      // Skip empty lines or comment lines
      if (line.empty() || (line[0] == '#'))
      {
         continue;
      }
      std::istringstream iss(line);
      Orbit              orbit;
      iss >> orbit.mLaunchHeading >> orbit.mLoftAngle >> orbit.mInclination >> orbit.mTimeOfFlight >> orbit.mLat >>
         orbit.mLon >> orbit.mAlt >> orbit.mSpeedECI >> orbit.mSpeedFixed >> orbit.mHeading >> orbit.mFlightPathAngle >>
         orbit.mResidualDeltaV >> orbit.mFuelRemaining >> orbit.mTimeRemaining;
      if (!iss.good())
      {
         auto out = ut::log::error() << "Error reading line of file.";
         out.AddNote() << "File: " << aFileName;
         out.AddNote() << "Line: " << lineCount;
         return false;
      }
      orbit.mLaunchHeading *= UtMath::cRAD_PER_DEG;
      orbit.mLoftAngle *= UtMath::cRAD_PER_DEG;
      orbit.mInclination *= UtMath::cRAD_PER_DEG;
      orbit.mAlt *= 1000.0;
      orbit.mHeading *= UtMath::cRAD_PER_DEG;
      orbit.mFlightPathAngle *= UtMath::cRAD_PER_DEG;
      aOrbits.push_back(orbit);
   }
   return true;
}

// =================================================================================================
// static
bool WsfOrbitalLaunchComputer::WriteOrbits(WsfArticulatedPart*     aSystemPtr,
                                           const std::string&      aFileName,
                                           const std::list<Orbit>& aOrbits)
{
   std::ofstream ofs(aFileName);
   if (!ofs.is_open())
   {
      auto out = ut::log::error() << "Unable to open file.";
      out.AddNote() << "File: " << aFileName;
      return false;
   }

   time_t clock;
   time(&clock);
   // cppcheck-suppress localtimeCalled
   struct tm* tmPtr = localtime(&clock);
   if (aSystemPtr != nullptr)
   {
      double lat;
      double lon;
      double alt;
      aSystemPtr->GetPlatform()->GetLocationLLA(lat, lon, alt);
      ofs << "#Launch data for " << aSystemPtr->GetType() << std::fixed;
      ofs << " Launched from";
      ofs << " Latitude: " << std::setprecision(6) << lat;
      ofs << " Longitude: " << std::setprecision(6) << lon;
      ofs << " Altitude: " << std::setprecision(1) << alt;
      ofs << '\n';
   }
   ofs << "#Generated on " << asctime(tmPtr);
   ofs << "#Lau-Hdg -Loft- -Incl.- -Time-- ---Lat---- ----Lon---- --Alt-- -Speed-- -FSpeed- --Hdg-- "
          "-Angle- -DeltaV- -Fleft- -Tleft-\n";
   //     "#xxx.xxx xx.xxx xxxx.xx xxxx.xx xxx.xxxxxx xxxx.xxxxxx xxxx.xx xxxxx.xx xxxxx.xx xxxx.xx "
   //     "xxxx.xx xxxxx.xx xxxxx.x xxxx.xx\n";
   ofs << std::fixed;

   UtCalendar calendar;
   for (const Orbit& orbit : aOrbits)
   {
      double r0    = UtSphericalEarth::cEARTH_RADIUS;
      double oe[6] = {r0, 0.0, 0.0, 0.0, 0.0, 0.0};
      if (orbit.mTimeOfFlight > 0.0) // orbit insertion achieved
      {
         ComputeOrbitalElements(aSystemPtr->GetPlatform()->GetSimulation()->GetEnvironment().GetCentralBody(),
                                oe,
                                orbit,
                                calendar);
      }
      double a           = oe[0];
      double e           = oe[1];
      double b           = a * (1.0 - e);
      double inclination = orbit.mInclination;
      if (orbit.mInclination > UtMath::cTWO_PI)
      {
         inclination = oe[2];
      }

      // clang-format off
      ofs << std::fixed
          << std::setprecision(3) << std::setw(8) << orbit.mLaunchHeading * UtMath::cDEG_PER_RAD
          << std::setprecision(3) << std::setw(7) << orbit.mLoftAngle * UtMath::cDEG_PER_RAD
          << std::setprecision(2) << std::setw(8) << inclination * UtMath::cDEG_PER_RAD
          << std::setprecision(2) << std::setw(8) << orbit.mTimeOfFlight
          << std::setprecision(6) << std::setw(11) << orbit.mLat
          << std::setprecision(6) << std::setw(12) << orbit.mLon
          << std::setprecision(2) << std::setw(8) << orbit.mAlt * 0.001
          << std::setprecision(2) << std::setw(9) << orbit.mSpeedECI
          << std::setprecision(2) << std::setw(9) << orbit.mSpeedFixed
          << std::setprecision(2) << std::setw(8) << orbit.mHeading * UtMath::cDEG_PER_RAD
          << std::setprecision(2) << std::setw(8) << orbit.mFlightPathAngle * UtMath::cDEG_PER_RAD
          << std::setprecision(2) << std::setw(9) << orbit.mResidualDeltaV
          << std::setprecision(1) << std::setw(8) << orbit.mFuelRemaining
          << std::setprecision(2) << std::setw(8) << orbit.mTimeRemaining
          << std::setprecision(3)
          << " " << (a - r0) * 0.001 << " x " << (b - r0) * 0.001 << " km, e=" << e
          << '\n';
      // clang-format on
   }
   return true;
}

// =================================================================================================
namespace
{
inline double Interpolate(double aValue1, double aValue2, double aFactor)
{
   double result = aValue1 + ((aValue2 - aValue1) * aFactor);
   return result;
}
} // namespace

// =================================================================================================
// Interpolate between two orbital launch points.
void WsfOrbitalLaunchComputer::InterpolateOrbits(Orbit&       aResult,
                                                 const Orbit& aOrbit1,
                                                 const Orbit& aOrbit2,
                                                 Variable     aFunction,
                                                 double       aValue) const
{
   // Determine the interpolation factor
   double v1 = aValue - 0.5;
   double v2 = aValue + 0.5;
   // double scale = 1.0;
   switch (aFunction)
   {
   case cALTITUDE:
      v1 = aOrbit1.mAlt;
      v2 = aOrbit2.mAlt;
      break;
   case cINCLINATION:
      v1 = aOrbit1.mInclination;
      v2 = aOrbit2.mInclination;
      // scale = UtMath::cDEG_PER_RAD;
      break;
   default: // should never happen
      break;
   }
   double factor = 0.0;
   double dv12   = v2 - v1;
   if (fabs(dv12) > 1.0E-8)
   {
      factor = (aValue - v1) / dv12;
      factor = UtMath::Limit(factor, 0.0, 1.0);
   }

   // Interpolate the variables
   aResult.mLaunchHeading   = Interpolate(aOrbit1.mLaunchHeading, aOrbit2.mLaunchHeading, factor);
   aResult.mLoftAngle       = Interpolate(aOrbit1.mLoftAngle, aOrbit2.mLoftAngle, factor);
   aResult.mInclination     = Interpolate(aOrbit1.mInclination, aOrbit2.mInclination, factor);
   aResult.mTimeOfFlight    = Interpolate(aOrbit1.mTimeOfFlight, aOrbit2.mTimeOfFlight, factor);
   aResult.mSpeedECI        = Interpolate(aOrbit1.mSpeedECI, aOrbit2.mSpeedECI, factor);
   aResult.mSpeedFixed      = Interpolate(aOrbit1.mSpeedFixed, aOrbit2.mSpeedFixed, factor);
   aResult.mHeading         = Interpolate(aOrbit1.mHeading, aOrbit2.mHeading, factor);
   aResult.mFlightPathAngle = Interpolate(aOrbit1.mFlightPathAngle, aOrbit2.mFlightPathAngle, factor);
   aResult.mResidualDeltaV  = Interpolate(aOrbit1.mResidualDeltaV, aOrbit2.mResidualDeltaV, factor);
   aResult.mFuelRemaining   = Interpolate(aOrbit1.mFuelRemaining, aOrbit2.mFuelRemaining, factor);
   aResult.mTimeRemaining   = Interpolate(aOrbit1.mTimeRemaining, aOrbit2.mTimeRemaining, factor);
   aResult.mFlightPathAngle = Interpolate(aOrbit1.mFlightPathAngle, aOrbit2.mFlightPathAngle, factor);
   aResult.mAlt             = Interpolate(aOrbit1.mAlt, aOrbit2.mAlt, factor);
   double orbit1LocWCS[3];
   double orbit2LocWCS[3];
   UtEntity::ConvertLLAToWCS(aOrbit1.mLat, aOrbit1.mLon, 0.0, orbit1LocWCS);
   UtEntity::ConvertLLAToWCS(aOrbit2.mLat, aOrbit2.mLon, 0.0, orbit2LocWCS);
   double resultLocWCS[3];
   for (int i = 0; i < 3; ++i)
   {
      resultLocWCS[i] = Interpolate(orbit1LocWCS[i], orbit2LocWCS[i], factor);
   }
   double dummy;
   UtEntity::ConvertWCSToLLA(resultLocWCS, aResult.mLat, aResult.mLon, dummy);
   // if (DebugEnabled())
   //{
   //    cout << "** Interpolate Between Two Points" << endl;
   //    cout << " v=" << aValue * scale << " v1=" << v1 * scale << " v2=" << v2 * scale
   //         << " dv12=" << dv12 * scale << " factor=" << factor << endl;
   //    cout << "** Point 1" << endl;
   //    aOrbit1.Print(cout);
   //    cout << "** Point 2" << endl;
   //    aOrbit2.Print(cout);
   //    cout << "** Result" << endl;
   //    aResult.Print(cout);
   // }
}

// =================================================================================================
bool WsfOrbitalLaunchComputer::InterpolateAlongLaunchHeading(Orbit& aResult, OrbitListIter aBestIter, double aAltitude) const
{
   if (aBestIter == mLEO_Data.end())
   {
      return false;
   }

   // Determine which entry (previous or next) on the same launch heading is adjacent to
   // the entry specified on input (which represents the best altitude selection).

   OrbitListIter oli(aBestIter); // Points to entry with best altitude along a launch heading
   const Orbit&  o = *oli;

   bool foundBracket(false);
   auto oli1 = oli;
   auto oli2 = oli;
   ++oli2;
   if (oli2 != mLEO_Data.end()) // there is a next entry
   {
      if (o.mLaunchHeading == oli2->mLaunchHeading) // and it has the same launch heading
      {
         double alt1 = o.mAlt;
         double alt2 = oli2->mAlt;
         if (((alt1 <= aAltitude) && (aAltitude <= alt2)) || ((alt2 <= aAltitude) && (aAltitude <= alt1)))
         {
            foundBracket = true;
         }
      }
   }
   if ((!foundBracket) && (oli1 != mLEO_Data.begin())) // There is a previous entry
   {
      --oli1;
      oli2 = oli;
      if (o.mLaunchHeading == oli1->mLaunchHeading) // and it has the same launch heading
      {
         double alt1 = oli1->mAlt;
         double alt2 = o.mAlt;
         if (((alt1 <= aAltitude) && (aAltitude <= alt2)) || ((alt2 <= aAltitude) && (aAltitude <= alt1)))
         {
            foundBracket = true;
         }
      }
   }
   if (foundBracket)
   {
      InterpolateOrbits(aResult, *oli1, *oli2, cALTITUDE, aAltitude);
   }
   else
   {
      aResult = *oli;
   }
   return true;
}

// =================================================================================================
// Nested class 'Orbit'
// =================================================================================================
WsfOrbitalLaunchComputer::Orbit::Orbit()
   : mLaunchHeading(0.0)
   , mLoftAngle(0.0)
   , mInclination(UtMath::cTWO_PI + 1.0)
   , mTimeOfFlight(0.0)
   , mLat(0.0)
   , mLon(0.0)
   , mAlt(0.0)
   , mSpeedECI(0.0)
   , mSpeedFixed(0.0)
   , mHeading(0.0)
   , mFlightPathAngle(0.0)
   , mResidualDeltaV(0.0)
   , mFuelRemaining(0.0)
   , mTimeRemaining(0.0)
{
}

// =================================================================================================
void WsfOrbitalLaunchComputer::Orbit::Print(ut::log::MessageStream& aOut) const
{
   aOut << std::fixed;
   aOut.AddNote() << "Launch Heading: " << std::setprecision(3) << mLaunchHeading * UtMath::cDEG_PER_RAD << " deg";
   aOut.AddNote() << "Loft Angle: " << std::setprecision(3) << mLoftAngle * UtMath::cDEG_PER_RAD << " deg";
   aOut.AddNote() << "Inclination: " << std::setprecision(2) << mInclination * UtMath::cDEG_PER_RAD << " deg";
   aOut.AddNote() << "Time of Flight: " << std::setprecision(2) << mTimeOfFlight;
   aOut.AddNote() << "Latitude: " << std::setprecision(6) << mLat;
   aOut.AddNote() << "Longitude: " << std::setprecision(6) << mLon;
   aOut.AddNote() << "Altitude: " << std::setprecision(2) << mAlt * 0.001;
   aOut.AddNote() << "Speed-ECI: " << std::setprecision(2) << mSpeedECI;
   aOut.AddNote() << "Speed-Fixed: " << std::setprecision(2) << mSpeedFixed;
   aOut.AddNote() << "Heading: " << std::setprecision(2) << mHeading * UtMath::cDEG_PER_RAD << " deg";
   aOut.AddNote() << "Flight Path Angle: " << std::setprecision(2) << mFlightPathAngle * UtMath::cDEG_PER_RAD << " deg";
   aOut.AddNote() << "Residual Delta V: " << std::setprecision(2) << mResidualDeltaV;
   aOut.AddNote() << "Fuel Remaining: " << std::setprecision(1) << mFuelRemaining;
   aOut.AddNote() << "Time Remaining: " << std::setprecision(2) << mTimeRemaining;
}
