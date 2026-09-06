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

#include "SiteConfig.hpp"

#include <iostream>

#include "InputConfig.hpp"
#include "Platform.hpp"
#include "RunConfig.hpp"
#include "Simulation.hpp"
#include "Task.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "Utilities.hpp"
#include "WsfPlatform.hpp"

namespace engage
{

// =================================================================================================
SiteConfig::SiteConfig()
   : mLauncherType("LAUNCHER_TYPE")
   , mTrackerType("TRACKER_TYPE")
   , mSiteList()
   , mPointCount(0)
   , mGridDownRangeMin(0.0)
   , mGridDownRangeMax(0.0)
   , mGridDownRangeStep(0.0)
   , mGridDownRangeUnits("m")
   , mGridCrossRangeMin(0.0)
   , mGridCrossRangeMax(0.0)
   , mGridCrossRangeStep(0.0)
   , mGridCrossRangeUnits("m")
   , mGridPointCount(0)
   , mGridDownRangeCount(0)
   , mGridCrossRangeCount(0)
{
}

// =================================================================================================
//! This is called when all of the input has been read.
bool SiteConfig::Complete(bool aSiteGridUsed)
{
   bool ok(true);

   // If using a site grid, determine the number of points required, and fill the site list with
   // sites that have the appropriate location for placement in a grid
   if (aSiteGridUsed)
   {
      double count;
      count               = (mGridDownRangeMax - mGridDownRangeMin + (0.5 * mGridDownRangeStep)) / mGridDownRangeStep;
      mGridDownRangeCount = static_cast<int>(count + 1);
      count = (mGridCrossRangeMax - mGridCrossRangeMin + (0.5 * mGridCrossRangeStep)) / mGridCrossRangeStep;
      mGridCrossRangeCount = static_cast<int>(count + 1);

      mPointCount = std::max(1, mGridCrossRangeCount * mGridDownRangeCount);

      for (int i = 0; i < mPointCount; ++i)
      {
         mSiteList.push_back(Site());
         double posX, posY;
         GetRunPosition(i, posX, posY);
         mSiteList.back().mX = posX;
         mSiteList.back().mY = posY;
      }

      return ok;
   }
   else
   {
      // If the site list is empty then the default of a single site at (0, 0, 0) is employed.
      if (mSiteList.empty())
      {
         mSiteList.push_back(Site());
      }
      mPointCount = static_cast<int>(mSiteList.size());

      return ok;
   }
}

// =================================================================================================
//! Determine the site location for a specific index
//! Note that altitude is not determined, as currently a site is fixed at altitude 0
void SiteConfig::GetRunPosition(int aSiteIndex, double& aX, double& aY) const
{
   int pointIndex    = aSiteIndex;
   int altitudeIndex = pointIndex / (mGridDownRangeCount * mGridCrossRangeCount);
   pointIndex -= altitudeIndex * (mGridDownRangeCount * mGridCrossRangeCount);
   int downRangeIndex  = pointIndex / mGridCrossRangeCount;
   int crossRangeIndex = pointIndex - (downRangeIndex * mGridCrossRangeCount);

   aX = mGridDownRangeMin + (mGridDownRangeStep * downRangeIndex);
   aY = mGridCrossRangeMin + (mGridCrossRangeStep * crossRangeIndex);
}

// =================================================================================================
bool SiteConfig::CreatePlatforms(Simulation& aSimulation, const Task& aTask) const
{
   int         siteIndex(aTask.GetSiteIndex());
   const Site& site(mSiteList[siteIndex]);

   bool      launcherOk(false);
   Platform* launcherPtr = new Platform(aTask.GetRunConfig(), aSimulation, mLauncherType, "launcher");
   aSimulation.SetLauncher(launcherPtr);
   if (launcherPtr->IsValid())
   {
      launcherPtr->SetLocationXYZ(site.mX, site.mY, site.mZ);
      launcherPtr->SetSpeedAndOrientationXYZ(site.mSpeed, site.mHeading, 0.0, 0.0);
      launcherOk = launcherPtr->AddToSimulation(0.0, aSimulation);
   }

   bool      trackerOk(false);
   Platform* trackerPtr = new Platform(aTask.GetRunConfig(), aSimulation, mTrackerType, "tracker");
   aSimulation.SetTracker(trackerPtr);
   if (trackerPtr->IsValid())
   {
      trackerPtr->SetLocationXYZ(site.mX, site.mY, site.mZ);
      trackerPtr->SetSpeedAndOrientationXYZ(site.mSpeed, site.mHeading, 0.0, 0.0);
      trackerOk = trackerPtr->AddToSimulation(0.0, aSimulation);
   }
   return (launcherOk && trackerOk);
}

// =================================================================================================
bool SiteConfig::ProcessInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());
   if (command == "launcher_type")
   {
      aInput.ReadValue(mLauncherType);
   }
   else if (command == "tracker_type")
   {
      aInput.ReadValue(mTrackerType);
   }
   else if (command == "sites")
   {
      mSiteList.clear();
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand(command))
      {
         if (command == "xyz")
         {
            Site site;
            aInput.ReadValueOfType(site.mX, UtInput::cLENGTH);
            aInput.ReadValueOfType(site.mY, UtInput::cLENGTH);
            aInput.ReadValueOfType(site.mZ, UtInput::cLENGTH);
            mSiteList.push_back(site);
         }
         else if (command == "heading")
         {
            if (mSiteList.empty())
            {
               throw UtInput::OutOfContext(aInput);
            }
            Site& site(mSiteList.back());
            aInput.ReadValueOfType(site.mHeading, UtInput::cANGLE);
            aInput.ValueInClosedRange(site.mHeading, -UtMath::cTWO_PI, UtMath::cTWO_PI);
            UtMath::NormalizeAngle0_TwoPi(site.mHeading);
         }
         else if (command == "speed")
         {
            if (mSiteList.empty())
            {
               throw UtInput::OutOfContext(aInput);
            }
            Site& site(mSiteList.back());
            aInput.ReadValueOfType(site.mSpeed, UtInput::cSPEED);
            aInput.ValueGreaterOrEqual(site.mSpeed, 0.0);
         }
         else
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   // TODO - NEED TO ADD THE VARIOUS ARRAY OPTIONS
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
SiteConfig::Site::Site()
   : mX(0.0)
   , mY(0.0)
   , mZ(0.0)
   , mSpeed(0.0)
   , mHeading(90.0 * UtMath::cRAD_PER_DEG)
{
}

// =================================================================================================
bool SiteConfig::ProcessSiteGridInput(UtInput& aInput)
{
   bool        myCommand(true);
   std::string command(aInput.GetCommand());

   if (command == "down_range")
   {
      aInput.ReadValue(command);
      aInput.StringEqual(command, "from");
      aInput.ReadValueOfType(mGridDownRangeMin, UtInput::cLENGTH);
      aInput.ReadValue(command);
      aInput.StringEqual(command, "to");
      aInput.ReadValueOfType(mGridDownRangeMax, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mGridDownRangeMax, mGridDownRangeMin);
      aInput.ReadValue(command);
      aInput.StringEqual(command, "by");
      aInput.ReadValue(mGridDownRangeStep);
      aInput.ReadValue(mGridDownRangeUnits);
      mGridDownRangeStep = aInput.ConvertValue(mGridDownRangeStep, mGridDownRangeUnits, UtInput::cLENGTH);
      aInput.ValueGreater(mGridDownRangeStep, 0.0);
   }
   else if (command == "cross_range")
   {
      aInput.ReadValue(command);
      aInput.StringEqual(command, "from");
      aInput.ReadValueOfType(mGridCrossRangeMin, UtInput::cLENGTH);
      aInput.ReadValue(command);
      aInput.StringEqual(command, "to");
      aInput.ReadValueOfType(mGridCrossRangeMax, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mGridCrossRangeMax, mGridCrossRangeMin);
      aInput.ReadValue(command);
      aInput.StringEqual(command, "by");
      aInput.ReadValue(mGridCrossRangeStep);
      aInput.ReadValue(mGridCrossRangeUnits);
      mGridCrossRangeStep = aInput.ConvertValue(mGridCrossRangeStep, mGridCrossRangeUnits, UtInput::cLENGTH);
      aInput.ValueGreater(mGridCrossRangeStep, 0.0);
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
bool SiteConfig::Site::ProcessInput(UtInput& aInput)
{
   bool        myCommand(false);
   std::string command(aInput.GetCommand());
   if (command == "x")
   {
   }
   else if (command == "y")
   {
   }
   else if (command == "z")
   {
   }
   else if (command == "speed")
   {
   }
   else if (command == "heading")
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

} // namespace engage
