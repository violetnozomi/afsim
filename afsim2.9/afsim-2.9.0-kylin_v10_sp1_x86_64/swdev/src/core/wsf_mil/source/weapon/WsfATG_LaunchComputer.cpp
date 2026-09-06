// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfATG_LaunchComputer.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtVec3.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfWeapon.hpp"

WsfATG_LaunchComputer::WsfATG_LaunchComputer()
   : WsfLaunchComputer()
   , mWeaponLARs()
   , mBestFitLAR(0)
   , mLastUpdate(0.0)
{
}

WsfATG_LaunchComputer::WsfATG_LaunchComputer(const WsfATG_LaunchComputer& aSrc)
   : WsfLaunchComputer(aSrc)
   , mWeaponLARs(aSrc.mWeaponLARs)
   , mBestFitLAR(aSrc.mBestFitLAR)
   , mLastUpdate(aSrc.mLastUpdate)
   , mDebugLARs(aSrc.mDebugLARs)
{
   for (auto& mWeaponLAR : mWeaponLARs)
   {
      mWeaponLAR = new WsfATG_WeaponLAR(*mWeaponLAR);
   }
}

// virtual
WsfATG_LaunchComputer::~WsfATG_LaunchComputer()
{
   for (auto& mWeaponLAR : mWeaponLARs)
   {
      delete mWeaponLAR;
   }
}

// virtual
WsfLaunchComputer* WsfATG_LaunchComputer::Clone() const
{
   return new WsfATG_LaunchComputer(*this);
}
namespace
{
bool LAR_Compare(WsfATG_WeaponLAR* lhs, WsfATG_WeaponLAR* rhs)
{
   return *lhs < *rhs;
}
} // namespace
// virtual
bool WsfATG_LaunchComputer::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   bool result = WsfLaunchComputer::Initialize(aSimTime, aWeaponPtr);

   if (mWeaponLARs.empty())
   {
      if (!ComputerGenerationMode())
      {
         auto out = ut::log::error()
                    << "The WsfATG_LaunchComputer in platform does not contain any LARs for launch evaluation.";
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
         result = false;
      }
   }
   else if ((mWeaponLARs.size() > 1) && (aWeaponPtr->GetUpdateInterval() == 0))
   {
      auto out = ut::log::error() << "The WsfATG_LaunchComputer's parent weapon requires a defined update_interval.";
      out.AddNote() << "Weapon: " << aWeaponPtr->GetName();
      return false;
   }
   else
   {
      for (auto& mWeaponLAR : mWeaponLARs)
      {
         if (mDebugLARs)
         {
            mWeaponLAR->SetDebugEnabled(true);
         }

         if (!mWeaponLAR->Initialize(*aWeaponPtr->GetSimulation()))
         {
            result = false;
         }
      }
      // Sort in increasing order of delta altitude.
      std::sort(mWeaponLARs.begin(), mWeaponLARs.end(), LAR_Compare);

      // If there is only one LAR, use it, regardless (there has to be at least one,
      // or we would already have failed Initialize().
      mBestFitLAR = 0;

      if (mWeaponLARs.size() > 1)
      {
         auto   launchVelNED = GetPlatform()->GetVelocityNED();
         double speed2D      = sqrt(launchVelNED[0] * launchVelNED[0] + launchVelNED[1] * launchVelNED[1]);
         // Will select closest LAR to current conditions.
         SelectLAR(GetPlatform()->GetAltitude(), speed2D);
      }

      if (mDebugLARs)
      {
         ut::log::debug() << "ATG LAR initialized to " << mBestFitLAR;
      }
   }

   mLastUpdate = aSimTime;
   return result;
}

void WsfATG_LaunchComputer::Update(double aSimTime)
{
   // The purpose of this function is to continuously update
   // the mBestFitLAR iterator, based on current flight conditions.
   // (A rather slow update rate is probably acceptable for this launch computer.)
   if (mLastUpdate < aSimTime)
   {
      if (mWeaponLARs.size() > 1)
      {
         double lat;
         double lon;
         double alt;
         double speed2D;
         double launchVelNED[3];
         GetPlatform()->GetLocationLLA(lat, lon, alt);
         GetPlatform()->GetVelocityNED(launchVelNED);
         speed2D = sqrt(launchVelNED[0] * launchVelNED[0] + launchVelNED[1] * launchVelNED[1]);

         // Will select closest LAR to current conditions.
         int lastBestFitLAR = mBestFitLAR;
         SelectLAR(alt, speed2D);
         mLastUpdate = aSimTime;

         if (mDebugLARs && (lastBestFitLAR != mBestFitLAR))
         {
            auto out = ut::log::debug() << "ATG LAR changed.";
            out.AddNote() << "Time (s): " << aSimTime;
            out.AddNote() << "From Index: " << lastBestFitLAR;
            out.AddNote() << "To Index  : " << mBestFitLAR;
         }
      }
   }
}

// virtual
bool WsfATG_LaunchComputer::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "launch_acceptable_region")
   {
      UtInputBlock inputBlock(aInput);
      std::string  larName;
      aInput.ReadValue(larName);
      std::unique_ptr<WsfATG_WeaponLAR> lar(new WsfATG_WeaponLAR);
      lar->SetName(larName);

      while (inputBlock.ReadCommand())
      {
         if (!lar->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mWeaponLARs.push_back(lar.release());
   }
   else if (command == "debug_lars")
   {
      mDebugLARs = true;
   }
   else
   {
      myCommand = WsfLaunchComputer::ProcessInput(aInput);
   }

   return myCommand;
}

// virtual
double WsfATG_LaunchComputer::EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime)
{
   InitializeResults(aSimTime, aTrack);

   // NOTE:  This is a "kludge" awaiting a better long-term fix.
   // This class does not yet save off the time of flight for a successful weapon release.
   // Currently we just use an (constant - incorrect) estimated time.  To be fixed later,
   // but will require that the "Generator" Weapon Tool be upgraded to save off more information.
   double estTOF             = 20.0; // seconds
   double estTimeToIntercept = cFOREVER;
   if (WithinConstraintsAt(aSimTime, aSimTime + aLaunchDelayTime + estTOF, aTrack))
   {
      estTimeToIntercept = aLaunchDelayTime + estTOF;
   }
   UpdateResults(aSimTime, estTimeToIntercept, aTrack);
   return estTimeToIntercept;
}

//! Select the LAR that most closely applies to the current launch condition, and use it.
// virtual
void WsfATG_LaunchComputer::SelectLAR(double aLaunchAlt, double aLaunchSpeed)
{
   // ASSUMPTION:  With an ATG Engagement, the LAR expands with increasing altitude above the target.
   // If this is so, then it is conservative to use a LAR for an altitude less than the current
   // launch altitude, so bias toward that direction.  But the LAR is also affected by launch speed,
   // so pick the launch speed closest to the current one.
   if (mWeaponLARs.size() <= 1)
   {
      return;
   }

   // MINIMUM Figure Of Merit intends to match the LAR altitude and speed criteria as best it can.
   double bestFOM = 9999999999.9;
   mBestFitLAR    = 0;
   for (size_t i = 0; i < mWeaponLARs.size(); ++i)
   {
      double altDelta = aLaunchAlt - mWeaponLARs[i]->DeltaAltitude();
      if (altDelta < 0.0)
      {
         altDelta *= -4.0; // bias towards a LAR for LOWER altitudes.
      }
      double spdDelta = fabs(aLaunchSpeed - mWeaponLARs[i]->LaunchSpeed());
      double fom      = altDelta + spdDelta;
      if (fom < bestFOM)
      {
         mBestFitLAR = (int)i;
         bestFOM     = fom;
      }
   }
}

// Trivial method implemented here to bypass the base class method.
// protected virtual
bool WsfATG_LaunchComputer::InitializeTTIData()
{
   return true; // Simply ignore the base class method
}

// Override of the base class method.
// protected virtual
bool WsfATG_LaunchComputer::WithinConstraintsAt(double aSimTime, double aFutureInterceptTime, const WsfTrack& aTrack)
{
   // First, we must pass the base class "gross" constraints, before we check the LAR:
   bool inConstraints = WsfLaunchComputer::WithinConstraintsAt(aSimTime, aFutureInterceptTime, aTrack);

   if (inConstraints)
   {
      double futureLocWCS[3];
      if (aTrack.GetExtrapolatedLocationWCS(aFutureInterceptTime, futureLocWCS))
      {
         double offsetNED[3];
         GetWeapon()->GetPlatform()->ConvertWCSToNED(futureLocWCS, offsetNED);

         double velNED[3];
         GetWeapon()->GetPlatform()->GetVelocityNED(velNED);

         double speed = UtVec3d::Magnitude(velNED);
         double hdgNED;
         double pitchNotUsed;
         double rollNotUsed;
         GetWeapon()->GetPlatform()->GetOrientationNED(hdgNED, pitchNotUsed, rollNotUsed);

         double launchGamma = 0.0;
         if (speed > 10)
         {
            // If forward motion is "significant" use course angle rather than heading.
            hdgNED      = atan2(velNED[1], velNED[0]);
            launchGamma = -asin(velNED[2] / speed);
         }

         double currentLocWCS[3];
         GetWeapon()->GetPlatform()->GetLocationWCS(currentLocWCS);

         inConstraints =
            mWeaponLARs[mBestFitLAR]->IsInside(futureLocWCS, currentLocWCS, hdgNED, speed, offsetNED[2], launchGamma);
      }
      else
      {
         inConstraints = false;
      }
   }
   return inConstraints;
}
