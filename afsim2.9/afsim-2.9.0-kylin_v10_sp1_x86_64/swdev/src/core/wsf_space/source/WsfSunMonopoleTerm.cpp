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

#include "WsfSunMonopoleTerm.hpp"

#include <sstream>
#include <string>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "WsfDE_FileManager.hpp"
#include "WsfOrbitalDynamics.hpp"

WsfSunMonopoleTerm::WsfSunMonopoleTerm()
   : WsfOrbitalDynamicsTerm{}
{
   mEarthPtr = UtSolarSystem::GetPlanet(UtSolarSystem::Planet::cEARTH);
}

bool WsfSunMonopoleTerm::ProcessInput(UtInput& aInput)
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
         throw UtInput::BadValue{aInput, err.str()};
      }
   }
   else
   {
      retval = false;
   }
   return retval;
}

bool WsfSunMonopoleTerm::Initialize(const WsfOrbitalDynamics& aDynamics)
{
   if (mDataSource == DataSource::cDE_FILE)
   {
      mDataFile = WsfDE_FileManager::Get(aDynamics.GetSimulation()).GetOrLoadFile(mDataFileName);
   }
   return WsfOrbitalDynamicsTerm::Initialize(aDynamics);
}

UtVec3d WsfSunMonopoleTerm::ComputeAcceleration(double /*aMass*/,
                                                const UtCalendar& aTime,
                                                const UtVec3d&    aPosition,
                                                const UtVec3d& /*aVelocity*/) const
{
   // Get position of sun
   UtVec3d sunPos = GetSunLocationECI(aTime);

   // Acceleration due to the Sun.
   UtVec3d solarAcc = aPosition - sunPos; // Starts as displacement
   double  radius   = solarAcc.Magnitude();
   double  radcubed = radius * radius * radius;
   solarAcc *= -mGravitationalParameter / radcubed; // Now becomes acceleration

   // The ECI frame is not inertial enough when considering the effect of the
   // Sun. This computes the acceleration of the ECI frame induced by the Sun.
   UtVec3d earthAcc      = -sunPos;
   double  radEarth      = earthAcc.Magnitude();
   double  radEarthCubed = radEarth * radEarth * radEarth;
   earthAcc *= -mGravitationalParameter / radEarthCubed;

   return solarAcc - earthAcc;
}

UtVec3d WsfSunMonopoleTerm::GetSunLocationECI(const UtCalendar& aEpoch) const
{
   UtVec3d sunPos{};
   if (mDataSource == DataSource::cDE_FILE)
   {
      // Get TDB Time
      UtCalendar timeCorrected = aEpoch;
      // NOTE: We are sure that the call to GetDynamics() will return a valid pointer because
      // the source of the data cannot be set to a JPL DE file via a script object. That is
      // the only case for which GetDynamics() will return a nullptr.
      GetDynamics()->GetTimeConstants(timeCorrected);
      WsfDE_File::Date epoch{timeCorrected.GetJulianTDB_Date(), 0.0};

      // Get the ephemeris point
      UtVec3d sunVel{};
      mDataFile->GetPlanetEphemeris(epoch, WsfDE_File::Body::cSUN, WsfDE_File::Body::cEARTH, sunPos, sunVel);

      // Convert km -> m
      sunPos.Multiply(1000.0);
   }
   else
   {
      UtVec3d locEc;
      mEarthPtr->GetLocationEcliptic(aEpoch, locEc);
      locEc.Multiply(-1.0);
      UtSolarSystem::ConvertEclipticToEquatorial(aEpoch, locEc, sunPos);
   }
   return sunPos;
}
