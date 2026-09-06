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

#ifndef WSFSTANDARDRADARSIGNATURE_HPP
#define WSFSTANDARDRADARSIGNATURE_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

class UtAzElLookup;
class UtAzElTable;
#include "UtAzElTypes.hpp"
#include "WsfRadarSignature.hpp"

//! A collection of one or more radar signature tables that represent the radar signature of a platform.

class WSF_EXPORT WsfStandardRadarSignature : public WsfRadarSignature
{
public:
   static WsfRadarSignature* ObjectFactory(const std::string& aTypeName);

   WsfStandardRadarSignature() = default;

   WsfRadarSignature* Clone() const override;

   bool InitializeType() override;

   std::vector<WsfStringId> GetStateNames() const override;

   bool ProcessInput(UtInput& aInput) override;

   float GetSignature(WsfStringId               aStateId,
                      WsfEM_Types::Polarization aPolarization,
                      double                    aFrequency,
                      double                    aTgtToXmtrAz,
                      double                    aTgtToXmtrEl,
                      double                    aTgtToRcvrAz,
                      double                    aTgtToRcvrEl,
                      WsfEM_Xmtr*               aXmtrPtr = nullptr,
                      WsfEM_Rcvr*               aRcvrPtr = nullptr) override;

   std::pair<float, float> GetSignatureLimits(WsfStringId aStateId, WsfEM_Types::Polarization aPolarization) const override;

   // The following nested classes are public to allow some applications to access the raw signature data.

   //! A 'Table' represents the signature for a given frequency limit.
   class Table
   {
   public:
      Table(double aFrequencyLimit, UtAzElTable* aTablePtr, bool aIsACopy);

      //! The upper frequency to which this table applies.
      double mFrequencyLimit;

      //! A pointer to the actual table data for this table.
      UtAzElTable* mTablePtr;

      //! The following is 'true' if this entry is just a copy of some other 'Table'
      //! object.  The original 'Table' object owns the memory pointed to by mTablePtr
      //! and is responsible for deleting it.
      bool mIsACopy;
   };

   //! A 'TableSet' is just a collection of 'Table's.
   //! This represents the collection of tables for a given polarization within
   //! a signature state.
   using TableSet   = std::vector<Table>;
   using TableIndex = std::vector<Table>::size_type;

   //! A 'State' represents all of the tables for a given signature state.
   class State
   {
   public:
      State(WsfStringId aStateId);

      WsfStringId mStateId;

      //! There is one set of tables for each polarization.
      TableSet mPolarization[WsfEM_Types::cPOL_COUNT];
   };

   using States     = std::vector<State>;
   using StateIndex = std::vector<State>::size_type;

   const States& GetStates() const;

   bool AddTable(UtAzElTable* aTablePtr, WsfStringId aStateId, WsfEM_Types::Polarization aPolarization, double aFrequencyLimit);

   void SelectTable(UtAzElLookup& aContext, WsfStringId aState, WsfEM_Types::Polarization aPolarization, double aFrequency);

private:
   class SharedData
   {
      using InterpolationType = ut::azel::InterpolationType;

   public:
      SharedData() = default;
      ~SharedData();

      bool InitializeType(WsfObject& aBase);

      bool IsA_ValidState(WsfStringId aId) const;

      bool ProcessInput(UtInput& aInput, WsfObject& aBase);

      bool AddTable(UtAzElTable*              aTablePtr,
                    WsfStringId               aStateId,
                    WsfEM_Types::Polarization aPolarization,
                    double                    aFrequencyLimit,
                    bool                      aIsACopy = false);

      const TableSet& SelectTableSet(WsfStringId aStateId, WsfEM_Types::Polarization aPolarization) const;

      void SelectTable(UtAzElLookup& aContext, WsfStringId aState, WsfEM_Types::Polarization aPolarization, double aFrequency);

      void UseDefaultPolarization(State& aState, int aPolarization);

      // The following helper methods are all called by ProcessInput
      // They were added to simplify ProcessInput
      void ProcessInputState(UtInput& aInput, WsfObject& aBase);

      void ProcessInputPolarization(UtInput& aInput, WsfObject& aBase);

      void ProcessInputFrequencyLimit(UtInput& aInput, WsfObject& aBase);

      bool ProcessInputTableData(UtInput& aInput, WsfObject& aBase);

      // Input processing aids
      WsfStringId               mCurrentStateId{"default"};
      WsfEM_Types::Polarization mCurrentPolarization{WsfEM_Types::cPOL_DEFAULT};
      double                    mCurrentFrequencyLimit{1.0E+30};

      StateIndex mDefaultStateIndex{0};
      States     mStates;

      bool mUseBisectorForBistatic{true};
      bool mMonoStaticSigDefined{true};

      bool              mInterpolateTables{true};
      InterpolationType mInterpolationType{InterpolationType::cLinear};
   };

   std::shared_ptr<SharedData> mSharedDataPtr{new SharedData()};
};

#endif
