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

#ifndef WSFSTANDARDOPTICALSIGNATURE_HPP
#define WSFSTANDARDOPTICALSIGNATURE_HPP

#include <memory>
#include <vector>

class UtAzElLookup;
class UtAzElTable;
#include "wsf_mil_export.h"

#include "UtAzElTypes.hpp"
#include "WsfOpticalSignature.hpp"

//! A collection of one or more optical signature tables that represent the optical signature of a platform.

class WSF_MIL_EXPORT WsfStandardOpticalSignature : public WsfOpticalSignature
{
public:
   static WsfOpticalSignature* ObjectFactory(const std::string& aTypeName);

   WsfStandardOpticalSignature() = default;

   WsfOpticalSignature* Clone() const override;

   bool InitializeType() override;

   std::vector<WsfStringId> GetStateNames() const override;

   bool ProcessInput(UtInput& aInput) override;

   float GetSignature(double aSimTime, WsfStringId aStateId, double aAzimuth, double aElevation) override;

   class State
   {
   public:
      State(WsfStringId aStateId, UtAzElTable* aTablePtr);

      WsfStringId  mStateId;
      UtAzElTable* mTablePtr;
   };

   const std::vector<State>& GetStates() const;

private:
   using StateIndex = std::vector<State>::size_type;

   class SharedData
   {
      using InterpolationType = ut::azel::InterpolationType;

   public:
      SharedData() = default;
      ~SharedData();

      bool InitializeType(WsfObject& aBase);

      bool IsA_ValidState(WsfStringId aId) const;

      bool ProcessInput(UtInput& aInput, WsfObject& aBase);

      bool AddTable(UtAzElTable* aTablePtr, WsfStringId aStateId);

      void SelectTable(UtAzElLookup& aContext, WsfStringId aStateId);

      WsfStringId        mCurrentStateId{"default"};
      StateIndex         mDefaultStateIndex{0};
      std::vector<State> mStates;

      bool              mInterpolateTables{true};
      InterpolationType mInterpolationType{InterpolationType::cLinear};
   };

   std::shared_ptr<SharedData> mSharedDataPtr{new SharedData()};
};

#endif
