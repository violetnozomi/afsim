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

#ifndef WSFBRAWLERFUEL_HPP
#define WSFBRAWLERFUEL_HPP

#include "wsf_brawler_export.h"

#include "WsfFuel.hpp"

class WSF_BRAWLER_EXPORT WsfBrawlerFuel : public WsfFuel
{
public:
   WsfBrawlerFuel(WsfScenario& aScenario);
   WsfBrawlerFuel(WsfBrawlerFuel&&) = default;
   WsfBrawlerFuel& operator=(const WsfBrawlerFuel&) = delete;
   WsfBrawlerFuel& operator=(WsfBrawlerFuel&&) = default;
   ~WsfBrawlerFuel() override;

   WsfFuel* Clone() const override;
   bool     Initialize(double aSimTime) override;
   bool     Initialize2(double aSimTime) override;
   bool     ProcessInput(UtInput& aInput) override;

   // The primary difference between WsfFuel and WsfBrawlerFuel is that the latter
   // calculates the fuel consumption rate based on aero tables from a model file.
   double CalcConsumptionRate() override;

protected:
   WsfBrawlerFuel(const WsfBrawlerFuel& aSrc);

private:
   // Function from BrawlerMover
   bool LoadAeroConfig(std::string aAeroFileAbsolutePath);

   // chkrng function from BrawlerMover
   void CheckRange(double aValue, double aMinRange, double aMaxRange, const std::string& mErrorText);

   // fflo function from BrawlerMover
   double CalculateFuelFlow(const double& aThrottleSetting, const double& aMach, const double& aAlt_m);

#define mmach1 (25)         // mmach1
#define mhts (21)           // mhts
#define undnum (-999.99e30) // undnum


   double mPrevFuelFlow;
   double mLastUpdate_s;
   // BrawlerMover Equivalent variables
   int    mMachArraySize;           // nmach1
   int    mAltArraySize;            // <= nhts
   double mFuelFlowAdjusted;        // ff_adj
   double mThrottleMaxValue;        // throtm
   double mMachValues[mmach1];      // mchval
   double mHeightVector[mhts];      // htvec[mhts]
   double mFlowIdle[mmach1 * mhts]; // floidl[mmach1 * mhts];
   double mFlowMil[mmach1 * mhts];  // flomil[mmach1 * mhts];
   double mFlowMax[mmach1 * mhts];  // flomax[mmach1 * mhts];
   double mAcFuelCap{0.0};
};

#endif
