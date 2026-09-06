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

#ifndef WSFSTANDARDINFRAREDSIGNATURE_HPP
#define WSFSTANDARDINFRAREDSIGNATURE_HPP

#include "wsf_mil_export.h"

#include <memory>
#include <vector>

class UtAzElLookup;
class UtAzElTable;

#include "WsfEM_Types.hpp"
#include "WsfInfraredSignature.hpp"

//! The standard representation of infrared signature using a collection of tables.

class WSF_MIL_EXPORT WsfStandardInfraredSignature : public WsfInfraredSignature
{
public:
   static WsfInfraredSignature* ObjectFactory(const std::string& aTypeName);

   WsfStandardInfraredSignature() = default;

   WsfInfraredSignature* Clone() const override;

   bool InitializeType() override;

   std::vector<WsfStringId> GetStateNames() const override;

   bool ProcessInput(UtInput& aInput) override;

   float GetSignature(double                    aSimTime,
                      WsfStringId               aStateId,
                      WsfEM_Types::InfraredBand aBand,
                      double                    aAzimuth,
                      double                    aElevation) override;

   //! A 'Table' represents the signature for a given frequency band.
   class Table
   {
   public:
      Table() = default;
      Table(UtAzElTable* aTablePtr, bool aIsACopy);

      //! A pointer to the actual table data for this table.
      UtAzElTable* mTablePtr{nullptr};

      //! The following is 'true' if this entry is just a copy of some other 'Table'
      //! object.  The original 'Table' object owns the memory pointed to by mTablePtr
      //! and is responsible for deleting it.
      bool mIsACopy{false};
   };

   using BandIndex = std::vector<Table>::size_type;

   //! A 'State' represents all of the tables for a given signature state.
   class State
   {
   public:
      State(WsfStringId aStateId);

      //! The string ID of the state name.
      WsfStringId mStateId;

      //! The tables for the various bands
      std::vector<Table> mBands;
   };

   using StateIndex = std::vector<State>::size_type;

   const std::vector<State>& GetStates() const;

private:
   //! The data that is shared between all instances of a signature.
   class SharedData
   {
   public:
      SharedData() = default;
      ~SharedData();

      bool InitializeType(WsfObject& aBase);

      bool IsA_ValidState(WsfStringId aId) const;

      bool ProcessInput(UtInput& aInput, WsfObject& aBase);

      bool AddTable(UtAzElTable* aTablePtr, WsfStringId aState, WsfEM_Types::InfraredBand aBand);

      void SelectTable(UtAzElLookup& aContext, WsfStringId aState, WsfEM_Types::InfraredBand aBand);

      bool AssignDefaultTable(WsfObject& aBase, State& aState, WsfEM_Types::InfraredBand aBand);

      WsfStringId               mCurrentStateId{"default"};
      WsfEM_Types::InfraredBand mCurrentBand{WsfEM_Types::cIRBAND_DEFAULT};

      StateIndex         mDefaultStateIndex{0};
      std::vector<State> mStates;

      bool mInterpolateTables{true};
   };

   std::shared_ptr<SharedData> mSharedDataPtr{new SharedData()};
};

#endif
