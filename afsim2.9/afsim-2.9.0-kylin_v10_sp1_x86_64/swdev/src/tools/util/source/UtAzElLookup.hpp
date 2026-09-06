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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTAZELLOOKUP_HPP
#define UTAZELLOOKUP_HPP

#include "ut_export.h"

#include <map>

class UtAzElTable;
#include "TblLookup.hpp"

/**
   Maintains a context for looking up values from a signature table.

   A sensor will typically have one of these objects for each platform that it
   is trying to detect.  The sensor is responsible for detecting changes in
   the signature state of the target platform or if it changes anything
   internally that could potentially require a different signature table.  If
   such conditions occur then it should call the signature manager to select a
   different table.
*/

class UT_EXPORT UtAzElLookup
{
   using InterpolationType = ut::azel::InterpolationType;

public:
   UtAzElLookup() = default;
   UtAzElLookup(bool aInterpolate, InterpolationType aInterpolationType = InterpolationType::cLinear)
      : mInterpolate(aInterpolate)
      , mInterpolationType(aInterpolationType)
   {
   }

   virtual ~UtAzElLookup() = default;

   float Lookup(float aAzimuth, float aElevation);

   float Lookup(double aAzimuth, double aElevation)
   {
      return Lookup(static_cast<float>(aAzimuth), static_cast<float>(aElevation));
   }

   void SetTable(UtAzElTable* aTablePtr);

   void GetAzimuthSlice(std::map<float, float>& aSlice, float aElevation);

   void SetEqualInterval(bool aEqualInterval) { mEqualInterval = aEqualInterval; }
   bool IsEqualInterval() const { return mEqualInterval; }

   void SetInterpolate(bool aInterpolate) { mInterpolate = aInterpolate; }
   bool Interpolate() const { return mInterpolate; }

   void SetInterpolationType(InterpolationType aInterpolationType) { mInterpolationType = aInterpolationType; }
   InterpolationType GetInterpolationType() const { return mInterpolationType; }

private:
   UtAzElTable* mTablePtr{nullptr};

   TblLookupUB<float> mAzLookupU;
   TblLookupUB<float> mElLookupU;

   TblLookupLUB<float> mAzLookupLU;
   TblLookupLUB<float> mElLookupLU;

   TblLookupE<float>  mAzLookupE;
   TblLookupE<float>  mElLookupE;
   TblLookupLE<float> mAzLookupLE;
   TblLookupLE<float> mElLookupLE;

   bool              mInterpolate{true};
   InterpolationType mInterpolationType{InterpolationType::cLinear};
   bool              mEqualInterval{false};

   bool mIsSymmetric{false};
};

#endif
