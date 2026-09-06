// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfMoonMonopoleTerm.hpp"

#include <sstream>
#include <string>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "WsfDE_FileManager.hpp"
#include "WsfOrbitalDynamics.hpp"

bool WsfMoonMonopoleTerm::ProcessInput(UtInput& aInput)
{
   bool        retval{true};
   std::string command = aInput.GetCommand();
   if (command == "gravitational_parameter")
   {
      aInput.ReadValue(mGravitationalParameter);
      aInput.ValueGreater(mGravitationalParameter, 0.0);
   }
   else if (command == "interpolation_interval")
   {
      double interval{};
      aInput.ReadValueOfType(interval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(interval, 0.0);
      mMoonPosition.SetInterval(interval);
   }
   else if (command == "source") // NO_DOC | FOR_TEST_ONLY
   {
      std::string source;
      aInput.ReadCommand(source);
      if (source == "default")
      {
         mDataSource = DataSource::cDEFAULT;
      }
      else if (source == "de_file")
      {
         aInput.ReadValueQuoted(mDataFileName);
         mDataSource = DataSource::cDE_FILE;
      }
      else
      {
         std::stringstream err{};
         err << "Unknown source type '" << source << "'";
         throw UtInput::BadValue(aInput, err.str());
      }
   }
   else
   {
      retval = false;
   }
   return retval;
}

UtVec3d WsfMoonMonopoleTerm::ComputeAcceleration(double /*aMass*/,
                                                 const UtCalendar& aTime,
                                                 const UtVec3d&    aPosition,
                                                 const UtVec3d& /*aVelocity*/) const
{
   // Get position of moon
   UtVec3d moonPos{};
   if (mDataSource == DataSource::cDE_FILE)
   {
      UtCalendar timeCorrected = aTime;
      // NOTE: We are sure that the call to GetDynamics() will return a valid pointer because
      // the source of the data cannot be set to a JPL DE file via a script object. That is
      // the only case for which GetDynamics() will return a nullptr.
      GetDynamics()->GetTimeConstants(timeCorrected);
      WsfDE_File::Date epoch{timeCorrected.GetJulianTDB_Date(), 0.0};

      // Call into the object
      UtVec3d moonVel{};
      mDataFile->GetPlanetEphemeris(epoch, WsfDE_File::Body::cMOON, WsfDE_File::Body::cEARTH, moonPos, moonVel);
      // Convert km -> m
      moonPos.Multiply(1000.0);
   }
   else
   {
      moonPos = mMoonPosition.GetPosition(aTime);
   }

   // Acceleration due to the Moon.
   UtVec3d lunarAcc = aPosition - moonPos; // Starts as displacement
   double  radius   = lunarAcc.Magnitude();
   double  radcubed = radius * radius * radius;
   lunarAcc *= -mGravitationalParameter / radcubed; // Now becomes acceleration

   // The ECI frame is not inertial enough when considering the effect of the
   // Moon. This computes the acceleration of the ECI frame induced by the Moon.
   UtVec3d earthAcc      = -moonPos;
   double  radEarth      = earthAcc.Magnitude();
   double  radEarthCubed = radEarth * radEarth * radEarth;
   earthAcc *= -mGravitationalParameter / radEarthCubed;

   return lunarAcc - earthAcc;
}

bool WsfMoonMonopoleTerm::Initialize(const WsfOrbitalDynamics& aDynamics)
{
   if (mDataSource == DataSource::cDE_FILE)
   {
      mDataFile = WsfDE_FileManager::Get(aDynamics.GetSimulation()).GetOrLoadFile(mDataFileName);
   }
   return WsfOrbitalDynamicsTerm::Initialize(aDynamics);
}

UtVec3d WsfMoonMonopoleTerm::GetMoonPositionECI(const UtCalendar& aTime) const
{
   return mMoonPosition.GetPosition(aTime);
}

WsfMoonMonopoleTerm::MoonPositionHelper::MoonPositionHelper()
{
   ComputeSpline();
}

UtVec3d WsfMoonMonopoleTerm::MoonPositionHelper::GetPosition(const UtCalendar& aTime) const
{
   if (mInterval > 0.0)
   {
      double dt = aTime.GetTimeSince(mBaseTime);

      if (dt < 0.0 || dt > 3.0 * mInterval)
      {
         dt = RedoSpline(aTime);
      }
      return mSpline(dt);
   }
   else
   {
      UtVec3d retval;
      UtMoon::GetLocationECI(aTime, retval);
      return retval;
   }
}

void WsfMoonMonopoleTerm::MoonPositionHelper::ComputeSpline() const
{
   UtCalendar when{mBaseTime};
   UtMoon::GetLocationECI(when, mF0);
   when.AdvanceTimeBy(mInterval);
   UtMoon::GetLocationECI(when, mF1);
   when.AdvanceTimeBy(mInterval);
   UtMoon::GetLocationECI(when, mF2);
   when.AdvanceTimeBy(mInterval);
   UtMoon::GetLocationECI(when, mF3);
   mSpline = UtCubicSpline::FourPoint(0.0, mF0, mInterval, mF1, 2.0 * mInterval, mF2, 3.0 * mInterval, mF3);
}

double WsfMoonMonopoleTerm::MoonPositionHelper::RedoSpline(const UtCalendar& aTime) const
{
   mBaseTime = aTime;
   ComputeSpline();
   return 0.0;
}
