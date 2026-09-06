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

#ifndef WSFSTANDARDOPTICALREFLECTIVITY_HPP
#define WSFSTANDARDOPTICALREFLECTIVITY_HPP

#include <memory>
#include <vector>

class UtAzElLookup;
class UtAzElTable;
#include "wsf_mil_export.h"

#include "UtAzElTypes.hpp"
#include "WsfOpticalReflectivity.hpp"

//! A collection of one or more reflectivity tables that represent the reflectivity of a platform.

class WSF_MIL_EXPORT WsfStandardOpticalReflectivity : public WsfOpticalReflectivity
{
public:
   static WsfOpticalReflectivity* ObjectFactory(const std::string& aTypeName);

   WsfStandardOpticalReflectivity() = default;

   WsfStandardOpticalReflectivity* Clone() const override;

   bool InitializeType() override;

   std::vector<WsfStringId> GetStateNames() const override;

   bool ProcessInput(UtInput& aInput) override;

   float GetOpticalReflectivity(double      aSimTime,
                                WsfStringId aStateId,
                                double      aWavelength,
                                double      aTgtToXmtrAz,
                                double      aTgtToXmtrEl,
                                double      aTgtToRcvrAz,
                                double      aTgtToRcvrEl) override;

   //! A table (i.e.: f(az,el)) for a given wavelength limit with a state.
   class Table
   {
   public:
      Table(double aWavelengthLimit, UtAzElTable* aTablePtr);

      double       mWavelengthLimit; //!< The upper wavelength to which this table applies.
      UtAzElTable* mTablePtr;        //!< A pointer to the actual table data for this table.
   };

   //! A 'TableSet' is just a collection of 'Table's.
   using TableSet = std::vector<Table>;

   //! 'State' represents all of the tables for a given state.
   class State
   {
   public:
      State(WsfStringId aStateId);

      WsfStringId mStateId;
      TableSet    mTables;
   };

   using States = std::vector<State>;

   const States& GetStates() const;

private:
   class SharedData
   {
      using InterpolationType = ut::azel::InterpolationType;

   public:
      SharedData() = default;
      ~SharedData();

      bool InitializeType(WsfStandardOpticalReflectivity& aObject);

      bool IsA_ValidState(WsfStringId aId) const;

      bool ProcessInput(UtInput& aInput, WsfStandardOpticalReflectivity& aObject);

      bool AddTable(UtAzElTable* aTablePtr, WsfStringId aStateId, double aWavelengthLimit);

      void SelectTable(UtAzElLookup& aContext, WsfStringId aState, double aWavelength);

      //! @name Input processing aids
      //@{
      WsfStringId mCurrentStateId{"default"};
      double      mCurrentWavelengthLimit{1.0E+30};
      //@}

      size_t mDefaultStateIndex{0};
      States mStates;

      bool              mInterpolateTables{true};
      InterpolationType mInterpolationType{InterpolationType::cLinear};

   private:
      // the following methods were added to simplify ProcessInput
      void ProcessInputState(UtInput& aInput, WsfStandardOpticalReflectivity& aObject);

      void ProcessInputWavelengthLimit(UtInput& aInput, WsfStandardOpticalReflectivity& aObject);

      void ProcessInputQuery(UtInput& aInput, WsfStandardOpticalReflectivity& aObject);

      bool ProcessInputTableData(UtInput& aInput, WsfStandardOpticalReflectivity& aObject);
   };

   std::shared_ptr<SharedData> mSharedDataPtr{new SharedData()};
};

#endif
