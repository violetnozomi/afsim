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

#include "WsfJupiterMonopoleTerm.hpp"

#include <sstream>
#include <string>

#include "UtInput.hpp"
#include "WsfDE_FileManager.hpp"
#include "WsfOrbitalDynamics.hpp"

WsfJupiterMonopoleTerm::WsfJupiterMonopoleTerm()
   : WsfOrbitalDynamicsTerm{}
   , mGravitationalParameter{mJupiter.GetGravitationalParameter()}
{
}

bool WsfJupiterMonopoleTerm::ProcessInput(UtInput& aInput)
{
   bool        retval{true};
   std::string command = aInput.GetCommand();
   if (command == "gravitational_parameter")
   {
      aInput.ReadValue(mGravitationalParameter);
      aInput.ValueGreater(mGravitationalParameter, 0.0);
   }
   else if (command == "source")
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

bool WsfJupiterMonopoleTerm::Initialize(const WsfOrbitalDynamics& aDynamics)
{
   if (mDataSource == DataSource::cDE_FILE)
   {
      mDataFile = WsfDE_FileManager::Get(aDynamics.GetSimulation()).GetOrLoadFile(mDataFileName);
   }
   return WsfOrbitalDynamicsTerm::Initialize(aDynamics);
}

UtVec3d WsfJupiterMonopoleTerm::ComputeAcceleration(double /*aMass*/,
                                                    const UtCalendar& aTime,
                                                    const UtVec3d&    aPosition,
                                                    const UtVec3d& /*aVelocity*/) const
{
   // Get the position of Jupiter
   UtVec3d jupiterPos = GetJupiterLocationECI(aTime);

   // Acceleration due to Jupiter.
   UtVec3d jupiterAcc = aPosition - jupiterPos; // Starts as displacement
   double  radius     = jupiterAcc.Magnitude();
   double  radcubed   = radius * radius * radius;
   jupiterAcc *= -mGravitationalParameter / radcubed; // Now becomes acceleration

   // The ECI frame is not inertial enough when considering the effect of
   // Jupiter. This computes the acceleration of the ECI frame induced by Jupiter.
   UtVec3d earthAcc      = -jupiterPos;
   double  radEarth      = earthAcc.Magnitude();
   double  radEarthCubed = radEarth * radEarth * radEarth;
   earthAcc *= -mGravitationalParameter / radEarthCubed;

   return jupiterAcc - earthAcc;
}

UtVec3d WsfJupiterMonopoleTerm::GetJupiterLocationECI(const UtCalendar& aEpoch) const
{
   UtVec3d jupiterPos;
   if (mDataSource == DataSource::cDE_FILE)
   {
      // Get TDB time
      UtCalendar timeCorrected = aEpoch;
      // NOTE: We are sure that the call to GetDynamics() will return a valid pointer because
      // the source of the data cannot be set to a JPL DE file via a script object. That is
      // the only case for which GetDynamics() will return a nullptr.
      GetDynamics()->GetTimeConstants(timeCorrected);
      WsfDE_File::Date epoch{timeCorrected.GetJulianTDB_Date(), 0.0};

      // Get the ephemeris point
      UtVec3d jupiterVel{};
      mDataFile->GetPlanetEphemeris(epoch, WsfDE_File::Body::cJUPITER, WsfDE_File::Body::cEARTH, jupiterPos, jupiterVel);
      // Convert km -> m
      jupiterPos.Multiply(1000.0);
   }
   else
   {
      jupiterPos = mJupiter.GetLocationECI(aEpoch);
   }
   return jupiterPos;
}
