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

#ifndef WSFNOISECLOUD_HPP
#define WSFNOISECLOUD_HPP

#include "wsf_export.h"

#include <map>

#include "UtInput.hpp"
#include "UtNoise.hpp"
#include "UtVec3.hpp"
#include "WsfObject.hpp"
#include "WsfObjectTypeList.hpp"
class WsfSimulation;

//! Noise clouds are used for weather effects on sensor and comm propagation.
//!
//! Noise clouds are built from noise functions as defined in UtNoise.
class WSF_EXPORT WsfNoiseCloud : public WsfObject
{
public:
   typedef std::map<WsfStringId, double> ModifierList;

   WsfNoiseCloud();
   WsfNoiseCloud& operator=(const WsfNoiseCloud&) = delete;
   ~WsfNoiseCloud() override;

   WsfNoiseCloud* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

   double CheckIntersections(double aSimTime, const double aLLA1[3], const double aLLA2[3]);

   void Initialize(WsfSimulation& aSimulation);

   //! Get the modifier list for WsfZoneAttenuation.
   virtual const ModifierList& GetModifierList() { return mModifierList; }

   static void GetCurrentTypes(std::vector<WsfNoiseCloud*>& aTypeList);

   double GetHeight() const { return mHeight; }

protected:
   //! Copy constructor (for Clone()).
   WsfNoiseCloud(const WsfNoiseCloud& aSrc);

private:
   ut::Random mRandom;
   UtNoise*   mNoiseFunctionPtr;

   int          mFrequency;
   int          mOctaves;
   double       mThreshold;
   double       mHeight;
   double       mThickness;
   double       mTileScalar;
   bool         mUseGlobalWind;
   ModifierList mModifierList;
   double       mLat;
   double       mLon;
   double       mAngularSpeed;
   UtVec3d      mWindAxis;
};

class WsfNoiseCloudTypes : public WsfObjectTypeList<WsfNoiseCloud>
{
public:
   using ObjectTypeList = WsfObjectTypeList<WsfNoiseCloud>;
   explicit WsfNoiseCloudTypes(WsfScenario& aScenario);
   static WSF_EXPORT WsfNoiseCloudTypes&       Get(WsfScenario& aScenario);
   static WSF_EXPORT const WsfNoiseCloudTypes& Get(const WsfScenario& aScenario);
};

#endif
