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

#include "WsfBallisticLaunchComputer.hpp"

#include <cassert>

#include "WsfPlatform.hpp"
#include "WsfStringId.hpp"

WsfBallisticLaunchComputer::WsfBallisticLaunchComputer()
   : WsfLaunchComputer()
   , mAltsSet(false)
   , mSpdsSet(false)
   , mTgtsSet(false)
   , mSized(false)
   , mNumLaunchAlts(0)
   , mNumLaunchSpds(0)
   , mNumTargetAlts(0)
   , mMinLaunchAlt(0.0)
   , mDeltaLaunchAlt(0.0)
   , mMinLaunchSpd(0.0)
   , mDeltaLaunchSpd(0.0)
   , mMinTargetAlt(0.0)
   , mDeltaTargetAlt(0.0)
   , mLaunchAlts()
   , mLaunchSpds()
   , mTargetAlts()
   , mTOFs()
   , mRanges()
   , mLaunchAltLE()
   , mLaunchSpdLE()
   , mTargetAltLE()
{
}

// virtual
WsfLaunchComputer* WsfBallisticLaunchComputer::Clone() const
{
   return new WsfBallisticLaunchComputer(*this);
}

// virtual
bool WsfBallisticLaunchComputer::Initialize(double aSimTime, WsfWeapon* aWeaponPtr)
{
   bool result = true;
   if (!WsfLaunchComputer::Initialize(aSimTime, aWeaponPtr))
   {
      result = false;
   }
   return result;
}

// virtual
bool WsfBallisticLaunchComputer::ProcessInput(UtInput& aInput)
{
   const std::string scream("WsfBallisticLaunchComputer:  Must set launch alts, speeds, and target alts before TOF and "
                            "DownRange data arrays.");

   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "launch_altitudes")
   {
      aInput.ReadValueOfType(mMinLaunchAlt, UtInput::cLENGTH);
      aInput.ReadValueOfType(mDeltaLaunchAlt, UtInput::cLENGTH);
      aInput.ValueGreater(mDeltaLaunchAlt, 0.0);
      aInput.ReadValue(mNumLaunchAlts);
      aInput.ValueGreater(mNumLaunchAlts, 1);
      mAltsSet = true;
      mSized   = false;
   }
   else if (command == "target_altitudes")
   {
      aInput.ReadValueOfType(mMinTargetAlt, UtInput::cLENGTH);
      aInput.ReadValueOfType(mDeltaTargetAlt, UtInput::cLENGTH);
      aInput.ValueGreater(mDeltaTargetAlt, 0.0);
      aInput.ReadValue(mNumTargetAlts);
      aInput.ValueGreater(mNumTargetAlts, 1);
      mTgtsSet = true;
      mSized   = false;
   }
   else if (command == "launch_speeds")
   {
      aInput.ReadValueOfType(mMinLaunchSpd, UtInput::cSPEED);
      aInput.ReadValueOfType(mDeltaLaunchSpd, UtInput::cSPEED);
      aInput.ValueGreater(mDeltaLaunchSpd, 0.0);
      aInput.ReadValue(mNumLaunchSpds);
      aInput.ValueGreater(mNumLaunchSpds, 1);
      mSpdsSet = true;
      mSized   = false;
   }
   else if (command == "times_of_flight_values")
   {
      if (SizedProperly())
      {
         // Do nothing;
      }
      else if (SizeFullySpecified())
      {
         LoadIndepValues();
      }
      else
      {
         throw UtInput::BadValue(aInput, scream);
      }

      double tof;
      for (int i = 0; i < mNumLaunchAlts; ++i)
      {
         for (int j = 0; j < mNumLaunchSpds; ++j)
         {
            for (int k = 0; k < mNumTargetAlts; ++k)
            {
               aInput.ReadValue(tof);
               mTOFs.Set(tof, i, j, k);
            }
         }
      }
   }
   else if (command == "downrange_values")
   {
      if (SizedProperly())
      {
         // Do nothing;
      }
      else if (SizeFullySpecified())
      {
         LoadIndepValues();
      }
      else
      {
         throw UtInput::BadValue(aInput, scream);
      }

      double downRange;
      for (int i = 0; i < mNumLaunchAlts; ++i)
      {
         for (int j = 0; j < mNumLaunchSpds; ++j)
         {
            for (int k = 0; k < mNumTargetAlts; ++k)
            {
               aInput.ReadValue(downRange);
               mRanges.Set(downRange, i, j, k);
            }
         }
      }
   }
   else
   {
      myCommand = WsfLaunchComputer::ProcessInput(aInput);
   }

   return myCommand;
}


void WsfBallisticLaunchComputer::LoadIndepValues()
{
   if (mSized)
   {
   }
   else if (SizeFullySpecified())
   {
      double maxLnchAlt = mMinLaunchAlt + (mNumLaunchAlts - 1) * mDeltaLaunchAlt;
      double maxLnchSpd = mMinLaunchSpd + (mNumLaunchSpds - 1) * mDeltaLaunchSpd;
      double maxTgtAlt  = mMinTargetAlt + (mNumTargetAlts - 1) * mDeltaTargetAlt;

      mLaunchAlts.SetValues(mMinLaunchAlt, maxLnchAlt, mNumLaunchAlts);
      mLaunchSpds.SetValues(mMinLaunchSpd, maxLnchSpd, mNumLaunchSpds);
      mTargetAlts.SetValues(mMinTargetAlt, maxTgtAlt, mNumTargetAlts);

      mTOFs.Resize(mNumLaunchAlts, mNumLaunchSpds, mNumTargetAlts);
      mRanges.Resize(mNumLaunchAlts, mNumLaunchSpds, mNumTargetAlts);

      mAltsSet = false;
      mSpdsSet = false;
      mTgtsSet = false;
      mSized   = true;
   }
}

void WsfBallisticLaunchComputer::GetBombFlight(double aLnchAlt, double aLnchSpeed, double aTgtAlt, double& aTOF, double& aDownRange)
{
   // Here is where we take a known altitudes and speed, and do table lookups to
   // get the expected TOF and DownRange travel of the bomb during descent.
   mLaunchAltLE.Lookup(mLaunchAlts, aLnchAlt);
   mLaunchSpdLE.Lookup(mLaunchSpds, aLnchSpeed);
   mTargetAltLE.Lookup(mTargetAlts, aTgtAlt);

   aTOF       = TblEvaluate(mTOFs, mLaunchAltLE, mLaunchSpdLE, mTargetAltLE);
   aDownRange = TblEvaluate(mRanges, mLaunchAltLE, mLaunchSpdLE, mTargetAltLE);
}

// virtual
bool WsfBallisticLaunchComputer::InitializeTTIData()
{
   // Nothing to do here, return true
   return true;
}

// virtual
bool WsfBallisticLaunchComputer::WithinConstraintsAt(double aSimTime, double aFutureInterceptTime, const WsfTrack& aTrack)
{
   assert(false);
   return false;
}

// virtual
double WsfBallisticLaunchComputer::EstimatedTimeToIntercept(double aSimTime, const WsfTrack& aTrack, double aLaunchDelayTime)
{
   double timeToIntercept = cFOREVER;
   InitializeResults(aSimTime, aTrack);
   assert(false);
   UpdateResults(aSimTime, timeToIntercept, aTrack);
   return timeToIntercept;
}
