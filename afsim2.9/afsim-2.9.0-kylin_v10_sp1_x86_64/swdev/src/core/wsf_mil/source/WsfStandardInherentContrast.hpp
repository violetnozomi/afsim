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

#ifndef WSFSTANDARDINHERENTCONTRAST_HPP
#define WSFSTANDARDINHERENTCONTRAST_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <vector>

#include "UtAzElTypes.hpp"

class UtAzElLookup;
class UtAzElTable;
#include "WsfInherentContrast.hpp"

//! A collection of one or more inherent contrast tables that represent the inherent contrast of a platform.

class WSF_MIL_EXPORT WsfStandardInherentContrast : public WsfInherentContrast
{
public:
   static WsfInherentContrast* ObjectFactory(const std::string& aTypeName);

   WsfStandardInherentContrast() = default;

   WsfStandardInherentContrast* Clone() const override;

   bool InitializeType() override;

   std::vector<WsfStringId> GetStateNames() const override;

   bool ProcessInput(UtInput& aInput) override;

   float GetInherentContrast(WsfStringId aStateId, double aAzimuth, double aElevation) override;

private:
   class State
   {
   public:
      State(WsfStringId aStateId, UtAzElTable* aTablePtr);

      WsfStringId  mStateId;
      UtAzElTable* mTablePtr;
   };

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

      WsfStringId mCurrentStateId{"default"};

      StateIndex         mDefaultStateIndex{0};
      std::vector<State> mStates;

      bool              mInterpolateTables{true};
      InterpolationType mInterpolationType{InterpolationType::cLinear};
   };

   std::shared_ptr<SharedData> mSharedDataPtr{new SharedData()};
};

#endif
