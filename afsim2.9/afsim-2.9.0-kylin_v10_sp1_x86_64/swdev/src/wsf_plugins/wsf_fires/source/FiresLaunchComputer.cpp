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

#include "FiresLaunchComputer.hpp"

#include "FiresTableLoader.hpp"
#include "FiresTables.hpp"
#include "UtInput.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfTerrain.hpp"
#include "WsfTrack.hpp"

namespace Fires
{

FiresLaunchComputer::FiresLaunchComputer(WsfScenario* aScenarioPtr)
   : WsfLaunchComputer()
   , mScenarioPtr(aScenarioPtr)
   , mType(nullptr)
   , mDebug(false)
   , mGeom()
   , mMinRange(0.0)
   , mMaxRange(0.0)
{
   mFiresTablesPtr = static_cast<FiresTables*>(mScenarioPtr->FindExtension("wsf_fires"));
}

WsfLaunchComputer* FiresLaunchComputer::Clone() const
{
   return new FiresLaunchComputer(*this);
}

// virtual
bool FiresLaunchComputer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "debug")
   {
      aInput.ReadValue(mDebug);
   }
   else if ((command == "fires_table") || (command == "fires_elevation_angle_table") ||
            (command == "fires_maximum_ordinate_table"))
   {
      myCommand = false;
      while (true)
      {
         FiresTable* tablePtr = FiresTableLoader::ProcessTable(aInput);
         if (tablePtr != nullptr)
         {
            FiresTables::Find(WsfScenario::FromInput(aInput))->FireTables()[tablePtr->GetName()] = tablePtr;
            myCommand                                                                            = true;
         }
         else
         {
            break;
         }
      }
   }
   else if (command == "minimum_range")
   {
      aInput.ReadValueOfType(mMinRange, UtInput::cLENGTH);
   }
   else if (command == "maximum_range")
   {
      aInput.ReadValueOfType(mMaxRange, UtInput::cLENGTH);
   }
   else
   {
      myCommand = WsfLaunchComputer::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
double FiresLaunchComputer::EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime)
{
   double timeToIntercept = cFOREVER;

   // Initial location is the platform's current location.
   double locWCS[3];
   GetPlatform()->GetLocationWCS(locWCS);

   if (aTrack.LocationValid())
   {
      double targetLocWCS[3];
      aTrack.GetLocationWCS(targetLocWCS);

      // Find the impact range
      double targetVecNED[3];
      mGeom.SetLocationWCS(locWCS);
      mGeom.ConvertWCSToNED(targetLocWCS, targetVecNED);

      // In this case the range is only the horizontal range
      targetVecNED[2] = 0.0; // no down component

      double impactRange = UtVec3d::Magnitude(targetVecNED);
      double tof         = 0.0;
      bool   ok          = false;
      if (mFiresTablesPtr->UsesMaxOrd(mType))
      {
         double maxOrd;
         ok = mFiresTablesPtr->GetMaxOrdAndTOF(mType, impactRange, maxOrd, tof);
      }
      else
      {
         double elevationAngle;
         ok = mFiresTablesPtr->GetElevationAngleAndTOF(mType, impactRange, elevationAngle, tof);
      }
      if (ok)
      {
         timeToIntercept = tof + aLaunchDelayTime;
         SetTimeOfFlight(tof);
         SetInterceptTime(timeToIntercept + aSimTime);
         SetTargetImpactPointWCS(targetLocWCS);
         UpdateResults(aSimTime, timeToIntercept, aTrack);
      }
   }

   return timeToIntercept;
}

bool FiresLaunchComputer::InitializeTTIData()
{
   mType                = GetWeapon()->GetLaunchedPlatformType();
   bool        ok       = true;
   FiresTable* tablePtr = mFiresTablesPtr->GetTable(mType);
   if (tablePtr != nullptr)
   {
      if (mMinRange == 0.0)
      {
         mMinRange = tablePtr->GetMinimumRange();
      }
      if (mMaxRange == 0.0)
      {
         mMaxRange = tablePtr->GetMaximumRange();
      }
   }
   else
   {
      ok = false;
   }
   return ok;
}

} // namespace Fires
