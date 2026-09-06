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

#include "WsfBrawlerFuel.hpp"

#include "BrawlerUtil.hpp"
#include "UtAtmosphere.hpp"
#include "UtInputFile.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

WsfBrawlerFuel::WsfBrawlerFuel(WsfScenario& aScenario)
   : WsfFuel(aScenario)
   , mPrevFuelFlow(0.0)
   , mLastUpdate_s(0.0)
   , mMachArraySize(0)
   , mAltArraySize(0)
   , mFuelFlowAdjusted(1.0)
   , mThrottleMaxValue(3.0)
{
   // brawler stores slugs,  AFSIM stores kg, pass in kg to AFSIM parent class
   // SetInitialQuantity(6100.0);  //in case nothing is defined

   memset(mMachValues, 0, sizeof(mMachValues));
   memset(mHeightVector, 0, sizeof(mHeightVector));
   memset(mFlowIdle, 0, sizeof(mFlowIdle));
   memset(mFlowMil, 0, sizeof(mFlowMil));
   memset(mFlowMax, 0, sizeof(mFlowMax));
}

WsfBrawlerFuel::WsfBrawlerFuel(const WsfBrawlerFuel& aSrc)
   : WsfFuel(aSrc)
   , mPrevFuelFlow(aSrc.mPrevFuelFlow)
   , mLastUpdate_s(aSrc.mLastUpdate_s)
   , mMachArraySize(aSrc.mMachArraySize)
   , mAltArraySize(aSrc.mAltArraySize)
   , mFuelFlowAdjusted(aSrc.mFuelFlowAdjusted)
   , mThrottleMaxValue(aSrc.mThrottleMaxValue)
{
   for (int i = 0; i < mMachArraySize; ++i)
   {
      mMachValues[i] = aSrc.mMachValues[i];
   }

   for (int i = 0; i < mAltArraySize; ++i)
   {
      mHeightVector[i] = aSrc.mHeightVector[i];
   }

   int len = mMachArraySize * mAltArraySize;

   for (int i = 0; i < len; ++i)
   {
      mFlowIdle[i] = aSrc.mFlowIdle[i];
      mFlowMil[i]  = aSrc.mFlowMil[i];
      mFlowMax[i]  = aSrc.mFlowMax[i];
   }
}

WsfBrawlerFuel::~WsfBrawlerFuel() {}

WsfFuel* WsfBrawlerFuel::Clone() const
{
   return new WsfBrawlerFuel(*this);
}

bool WsfBrawlerFuel::Initialize(double aSimTime)
{
   mLastUpdate_s = aSimTime;
   if (mAcFuelCap == 0.0 && GetInitialQuantity() == 0.0)
   {
      SetInitialQuantity(6100.0);
   }
   return WsfFuel::Initialize(aSimTime);
}

bool WsfBrawlerFuel::Initialize2(double aSimTime)
{
   return WsfFuel::Initialize2(aSimTime);
}

bool WsfBrawlerFuel::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "aero_file")
   {
      std::string filename;
      aInput.ReadValue(filename);
      filename = aInput.LocateFile(filename);
      if (!LoadAeroConfig(filename))
      {
         throw UtInput::BadValue(aInput, "Could not load WSF_BRAWLER_MOVER aero_file!");
      }
   }
   else if (command == "initial_quantity_ratio")
   {
      double ratio;
      aInput.ReadValue(ratio);
      aInput.ValueLessOrEqual(ratio, 2.0);
      SetInitialQuantity(UtMath::cKG_PER_SLUG * mAcFuelCap * ratio / 32.17405);
   }
   else
   {
      myCommand = WsfFuel::ProcessInput(aInput);
   }
   return myCommand;
}

double WsfBrawlerFuel::CalcConsumptionRate()
{
   double    throttle = 1.0;
   WsfMover* moverPtr = GetPlatform()->GetMover();
   if (moverPtr != nullptr)
   {
      throttle = moverPtr->GetThrottlePosition();
   }
   // NAS - if mover is off, don't decrement fuel
   if (moverPtr && !moverPtr->IsTurnedOn())
   {
      return 0.0;
   }
   double alt_m = GetPlatform()->GetAltitude();
   double mach  = GetPlatform()->GetSpeed() / WsfFuel::GetScenario().GetAtmosphere().SonicVelocity(alt_m);
   return CalculateFuelFlow(throttle, mach, alt_m) * UtMath::cKG_PER_SLUG;
}

bool WsfBrawlerFuel::LoadAeroConfig(std::string aAeroFileAbsolutePath)
{
   UtInput aInput;
   aInput.PushInput(ut::make_unique<UtInputFile>(aAeroFileAbsolutePath));

   std::string command;
   bool        foundEndComments = false;
   while (BRAWLER::TryReadCommand(aInput, command))
   {
      if (foundEndComments == false)
      {
         if (command == "END" && BRAWLER::TryReadCommand(aInput, command))
         {
            if (command == "COMMENTS")
            {
               foundEndComments = true;
            }
         }
      }
      else if (command == "NMACH1")
      {
         BRAWLER::ReadValue(aInput, mMachArraySize);
      }
      else if (command == "MCHVAL")
      {
         for (int i = 0; i < mMachArraySize; ++i)
         {
            BRAWLER::ReadValue(aInput, mMachValues[i]);
         }
      }
      else if (command == "NHTS")
      {
         BRAWLER::ReadValue(aInput, mAltArraySize);
      }
      else if (command == "HTVEC")
      {
         for (int i = 0; i < mAltArraySize; ++i)
         {
            BRAWLER::ReadValue(aInput, mHeightVector[i]);
         }
      }
      else if (command == "FLOIDL")
      {
         int len = mMachArraySize * mAltArraySize;
         for (int i = 0; i < len; ++i)
         {
            BRAWLER::ReadValue(aInput, mFlowIdle[i]);
         }
      }
      else if (command == "FLOMIL")
      {
         int len = mMachArraySize * mAltArraySize;
         for (int i = 0; i < len; ++i)
         {
            BRAWLER::ReadValue(aInput, mFlowMil[i]);
         }
      }
      else if (command == "FLOMAX")
      {
         int len = mMachArraySize * mAltArraySize;
         for (int i = 0; i < len; ++i)
         {
            BRAWLER::ReadValue(aInput, mFlowMax[i]);
         }
      }
      else if (command == "AC_FUEL_CAP")
      {
         BRAWLER::ReadValue(aInput, mAcFuelCap); // value in units of slugs*grav
         if (mAcFuelCap < 1.e+10 && GetInitialQuantity() == 0)
         {
            SetInitialQuantity(UtMath::cKG_PER_SLUG * mAcFuelCap / 32.17405);
         }
      }
      else
      {
         // Do nothing
      }
   }
   return true;
}

void WsfBrawlerFuel::CheckRange(double aValue, double aMinRange, double aMaxRange, const std::string& mErrorText)
{
   if (aMinRange <= aValue && aValue <= aMaxRange)
   {
      return;
   }
   BRAWLER::nabort(mErrorText);
}

double WsfBrawlerFuel::CalculateFuelFlow(const double& aThrottleSetting, const double& aMach, const double& aAlt_m)
{
   int    intThrottle        = 0;
   double throttleDifference = 0;
   double fuelFlowIdle       = 0;
   double fuelFlowMil        = 0;
   double throttle           = 0;
   double fuelFlowTotal      = 0; // return value

   throttle = aThrottleSetting;
   if (throttle == 0.0)
   {
      return 0.0;
   }

   // C     --CHECK FOR THRUST REVERSER EMPLOYMENT => mil power
   if (throttle <= 0.0 && throttle >= -1.0)
   {
      throttle = 2.0;
   }
   // C     --Limiting throttle...if a change is made here, make it also
   // C     --in subroutine THRUST
   if (throttle > mThrottleMaxValue)
   {
      throttle = mThrottleMaxValue;
   }

   CheckRange(throttle, 1.0, 3.0, "-thrtl in fflo-");
   intThrottle        = int(throttle);
   throttleDifference = throttle - double(intThrottle);

   // Brawler tables store alt in ft, need to convert aAlt_m to aAlt_ft for lookup in tables
   double alt_ft = aAlt_m * UtMath::cFT_PER_M;

   if (intThrottle == 1)
   {
      fuelFlowIdle = BRAWLER::tbl2(aMach, alt_ft, mFlowIdle, mMachValues, mMachArraySize, mHeightVector, mAltArraySize);
      fuelFlowMil  = BRAWLER::tbl2(aMach, alt_ft, mFlowMil, mMachValues, mMachArraySize, mHeightVector, mAltArraySize);
      fuelFlowTotal = fuelFlowIdle + throttleDifference * (fuelFlowMil - fuelFlowIdle);
   }
   else if (intThrottle == 2)
   {
      fuelFlowIdle  = BRAWLER::tbl2(aMach, alt_ft, mFlowMil, mMachValues, mMachArraySize, mHeightVector, mAltArraySize);
      fuelFlowMil   = BRAWLER::tbl2(aMach, alt_ft, mFlowMax, mMachValues, mMachArraySize, mHeightVector, mAltArraySize);
      fuelFlowTotal = fuelFlowIdle + throttleDifference * (fuelFlowMil - fuelFlowIdle);
   }
   else
   {
      fuelFlowTotal = BRAWLER::tbl2(aMach, alt_ft, mFlowMax, mMachValues, mMachArraySize, mHeightVector, mAltArraySize);
   }

   if (fuelFlowTotal < -1.0)
   {
      fuelFlowTotal = undnum;
   }
   else
   {
      fuelFlowTotal = fuelFlowTotal * mFuelFlowAdjusted;
   }
   return fuelFlowTotal;
}
