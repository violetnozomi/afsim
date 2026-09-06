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

#ifndef WSFBISTATICSIG_HPP
#define WSFBISTATICSIG_HPP

#include "wsf_mil_export.h"

#include <map>
#include <string>
#include <vector>

#include "TblLookup.hpp"
#include "UtAzElTable.hpp"
#include "UtAzElTypes.hpp"
#include "UtMath.hpp"
#include "WsfEM_Types.hpp"
#include "WsfObject.hpp"

//! A collection of one or more bistatic signature tables that represent the bistatic signature of a platform.

class WSF_MIL_EXPORT WsfBistaticSig : public WsfObject
{
public:
   class AzElAzElTable
   {
   public:
      AzElAzElTable()          = default;
      virtual ~AzElAzElTable() = default;

      void CreateConstantTable(float aValue);

      bool Validate(size_t aTgtToRcvrAzCount, size_t aTgtToRcvrElCount);

      TblIndVarU<double>       mTgtToXmtrAz;
      TblIndVarU<double>       mTgtToXmtrEl;
      TblDepVar2<UtAzElTable*> mAzElTables;

      TblLookupU<double> mTgtToXmtrAzU;
      TblLookupU<double> mTgtToXmtrElU;

      TblLookupLU<double> mTgtToXmtrAzLU;
      TblLookupLU<double> mTgtToXmtrElLU;
   };

   // PLT signature table format
   // Polarization -> Frequency -> AzEl Table
   using PolFreqTableMapType = std::map<std::string, std::map<double, AzElAzElTable*>>;

   WsfBistaticSig() = default;

   ~WsfBistaticSig() override;

   WsfBistaticSig* Clone() const override;

   virtual bool Initialize();

   bool ProcessInput(UtInput& aInput) override;

   bool AddTable(AzElAzElTable*            aTablePtr,
                 WsfStringId               aState,
                 WsfEM_Types::Polarization aPolarization,
                 double                    aFrequency,
                 bool                      aIsACopy = false);

   bool AddTable(UtAzElTable*              aTablePtr,
                 WsfStringId               aState,
                 WsfEM_Types::Polarization aPolarization,
                 double                    aFrequency,
                 double                    aTgtToXmtrAz,
                 double                    aTgtToXmtrEl,
                 bool                      aIsACopy = false);

   float GetSignature(UtAzElLookup&             aContext,
                      WsfStringId               aStateId,
                      WsfEM_Types::Polarization aPolarization,
                      double                    aFrequency,
                      double                    aTgtToXmtrAz,
                      double                    aTgtToXmtrEl,
                      double                    aTgtToRcvrAz,
                      double                    aTgtToRcvrEl);

   void SelectTable(AzElAzElTable& aContext, WsfStringId aState, WsfEM_Types::Polarization aPolarization, double aFrequency);

   static WsfBistaticSig* GetDefaultSig();

   bool IsA_ValidState(WsfStringId aId);

   // The following nested classes were 'private', but the Sun Workshop
   // compilers would not compile this file if they weren't 'public'.

   //! A 'Table' represents the signature for a given frequency limit.
   class Table
   {
   public:
      Table(double aFrequencyLimit, AzElAzElTable* aTablePtr, bool aIsACopy);

      //! The upper frequency to which this table applies.
      double mFrequencyLimit;

      //! A pointer to the actual table data for this table.
      AzElAzElTable* mTablePtr;

      //! The following is 'true' if this entry is just a copy of some other 'Table'
      //! object.  The original 'Table' object owns the memory pointed to by mTablePtr
      //! and is responsible for deleting it.
      bool mIsACopy;
   };

   //! A 'TableSet' is just a collection of 'Table's.
   //! This represents the collection of tables for a given polarization within
   //! a signature state.
   using TableSet = std::vector<Table>;

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

   //! Return the signatures for all of defined states.
   //! @note This method was created to allow External Services access to signature data. It should not be considered
   //! part of the public interface as it is exposing data that was once 'private'. Use at your own risk.
   const States& GetStates() const { return mStates; }

   using InterpolationType = ut::azel::InterpolationType;
   void SetInterpolateTables(bool aInterpolateTables) { mInterpolateTables = aInterpolateTables; }
   void SetInterpolationType(InterpolationType aInterpolationType) { mInterpolationType = aInterpolationType; }

   void SetInterpTgtToXmtrAngles(bool aInterpTgtToXmtrAngles) { mInterpTgtToXmtrAngles = aInterpTgtToXmtrAngles; }
   void SetInterpTgtToXmtrType(InterpolationType aIType) { mInterpTgtToXmtrType = aIType; }

private:
   // the following private methods are added to simplify ProcessInput
   void ProcessInputState(UtInput& aInput);
   void ProcessInputPolarization(UtInput& aInput);
   void ProcessInputFrequencyLimit(UtInput& aInput);
   void ProcessInputAzimuth(UtInput& aInput);
   void ProcessInputElevation(UtInput& aInput);
   bool ProcessInputTableData(UtInput& aInput);

   void UseDefaultPolarization(State& aState, int aPolarization);

   // Input processing aids
   WsfStringId               mCurrentStateId{"default"};
   WsfEM_Types::Polarization mCurrentPolarization{WsfEM_Types::cPOL_DEFAULT};
   double                    mCurrentFrequencyLimit{1.0E+30};
   double                    mCurrentTgtToXmtrAz{-UtMath::cPI};
   double                    mCurrentTgtToXmtrEl{-UtMath::cPI_OVER_2};
   int                       mCurrentAzimuthIndex{-1};
   int                       mCurrentElevationIndex{-1};

   bool              mInterpolateTables{true};
   InterpolationType mInterpolationType{InterpolationType::cLinear};
   bool              mInterpTgtToXmtrAngles{false};
   InterpolationType mInterpTgtToXmtrType{InterpolationType::cLinear};
   StateIndex        mDefaultStateIndex{0};
   States            mStates;
};

#endif
