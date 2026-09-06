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

#ifndef WSFSTANDARDACOUSTICSIGNATURE_HPP
#define WSFSTANDARDACOUSTICSIGNATURE_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <vector>

#include "TblLookup.hpp"
#include "WsfAcousticSignature.hpp"

//! A collection of one or more acoustic signature tables that represent the acoustic signature of a platform.

class WSF_MIL_EXPORT WsfStandardAcousticSignature : public WsfAcousticSignature
{
public:
   static WsfAcousticSignature* ObjectFactory(const std::string& aTypeName);

   static WsfAcousticSignature* CreateDefaultType();

   WsfStandardAcousticSignature();

   WsfAcousticSignature* Clone() const override;

   bool InitializeType() override;

   std::vector<WsfStringId> GetStateNames() const override;

   bool ProcessInput(UtInput& aInput) override;

   double GetNoisePressure(WsfStringId aStateId, double aFrequency) override;

   std::pair<double, double> GetNoisePressureLimits(WsfStringId aStateId) const override;

   // The standard acoustic signature does not currently implement Az/El lookup
   double GetNoisePressure(WsfStringId aStateId, double aFrequency, double aTgtToRcvrAz, double aTgtToRcvrEl) override
   {
      return GetNoisePressure(aStateId, aFrequency);
   }

   // This is used by WsfAcousticSensor to create an internal table.
   // note: The signature is assumed to have no requirement to be attached to a platform. Thus,
   //       Initialize(simTime, platformPtr) does not have to be called.
   bool AddStateEntry(WsfStringId aStateId, std::vector<double>& aFreqVector, std::vector<double>& aSigVector)
   {
      return mSharedDataPtr->AddStateEntry(aStateId, aFreqVector, aSigVector);
   }

   //! A 'Table' represents the signature.
   class Table
   {
   public:
      TblIndVarU<double>   mFreqs;
      TblDepVar1<double>   mSigs;
      TblLookupLUX<double> mFreqLookup;
   };

   //! A 'State' represents all of the tables for a given signature state.
   class State
   {
   public:
      State(WsfStringId aStateId, Table& aTable);

      //! The string ID of the state name.
      WsfStringId mStateId;

      Table mSigTable;
   };

   using StateIndex = std::vector<State>::size_type;

   const std::vector<State>& GetStates() const;

private:
   class SharedData
   {
   public:
      SharedData();
      ~SharedData();

      bool InitializeType(WsfObject& aBase);

      bool IsA_ValidState(WsfStringId aId) const;

      bool ProcessInput(UtInput& aInput, WsfObject& aBase);

      bool AddStateEntry(WsfStringId aStateId, std::vector<double>& aFreqVector, std::vector<double>& aSigVector);

      bool AssignDefaultTable(State& aState);

      WsfStringId        mCurrentStateId;
      double             mCorrectionFactor;
      StateIndex         mDefaultStateIndex;
      std::vector<State> mStates;
   };

   std::shared_ptr<SharedData> mSharedDataPtr;
};

#endif
