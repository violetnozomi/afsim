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

#include "WsfStandardRadarSignature.hpp"

#include <algorithm>

#include "UtAzElLookup.hpp"
#include "UtAzElTable.hpp"
#include "UtAzElTableLoader.hpp"
#include "UtAzElUtil.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Util.hpp"

// =================================================================================================
//! Factory method called by WsfRadarSignatureTypes.
// static
WsfRadarSignature* WsfStandardRadarSignature::ObjectFactory(const std::string& aTypeName)
{
   WsfStandardRadarSignature* instancePtr = nullptr;
   if (aTypeName == "WSF_RADAR_SIGNATURE")
   {
      instancePtr = new WsfStandardRadarSignature();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfRadarSignature* WsfStandardRadarSignature::Clone() const
{
   return new WsfStandardRadarSignature(*this);
}

// =================================================================================================
// virtual
bool WsfStandardRadarSignature::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mSharedDataPtr->ProcessInput(aInput, *this))
   {
   }
   else
   {
      myCommand = WsfRadarSignature::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool WsfStandardRadarSignature::InitializeType()
{
   bool ok = WsfRadarSignature::InitializeType();
   ok &= mSharedDataPtr->InitializeType(*this);
   return ok;
}

// =================================================================================================
// virtual
std::vector<WsfStringId> WsfStandardRadarSignature::GetStateNames() const
{
   const auto& states = GetStates();
   return GetStateNamesP<State, &State::mStateId>(states.begin(), states.end());
}

// =================================================================================================
float WsfStandardRadarSignature::GetSignature(WsfStringId               aStateId,
                                              WsfEM_Types::Polarization aPolarization,
                                              double                    aFrequency,
                                              double                    aTgtToXmtrAz,
                                              double                    aTgtToXmtrEl,
                                              double                    aTgtToRcvrAz,
                                              double                    aTgtToRcvrEl,
                                              WsfEM_Xmtr* /* aXmtrPtr = 0*/,
                                              WsfEM_Rcvr* /* aRcvrPtr = 0*/)
{
   UtAzElLookup context{mSharedDataPtr->mInterpolateTables, mSharedDataPtr->mInterpolationType};

   float rcs = 1.0;
   if ((aTgtToXmtrAz == aTgtToRcvrAz) && (aTgtToXmtrEl == aTgtToRcvrEl))
   {
      // Monostatic
      if (mSharedDataPtr->mMonoStaticSigDefined)
      {
         mSharedDataPtr->SelectTable(context, aStateId, aPolarization, aFrequency);
         rcs = context.Lookup(aTgtToXmtrAz, aTgtToXmtrEl);
      }
   }
   else
   {
      mSharedDataPtr->SelectTable(context, aStateId, aPolarization, aFrequency);

      if (mSharedDataPtr->mUseBisectorForBistatic)
      {
         double sinAz           = sin(aTgtToXmtrAz);
         double cosAz           = cos(aTgtToXmtrAz);
         double sinEl           = sin(aTgtToXmtrEl);
         double cosEl           = cos(aTgtToXmtrEl);
         double tgtToXmtrVec[3] = {cosAz * cosEl, sinAz * cosEl, -sinEl};

         sinAz                  = sin(aTgtToRcvrAz);
         cosAz                  = cos(aTgtToRcvrAz);
         sinEl                  = sin(aTgtToRcvrEl);
         cosEl                  = cos(aTgtToRcvrEl);
         double tgtToRcvrVec[3] = {cosAz * cosEl, sinAz * cosEl, -sinEl};

         double bisectorVec[3];
         UtVec3d::Add(bisectorVec, tgtToXmtrVec, tgtToRcvrVec);
         double bisectorAz, bisectorEl;
         UtEntity::ComputeAzimuthAndElevation(bisectorVec, bisectorAz, bisectorEl);

         rcs = context.Lookup(bisectorAz, bisectorEl);
      }
      else
      {
         // Use the target->receiver angle
         rcs = context.Lookup(aTgtToRcvrAz, aTgtToRcvrEl);
      }
   }
   return rcs;
}

// =================================================================================================
// virtual
std::pair<float, float> WsfStandardRadarSignature::GetSignatureLimits(WsfStringId               aStateId,
                                                                      WsfEM_Types::Polarization aPolarization) const
{
   float minSig = std::numeric_limits<float>::max();
   float maxSig = std::numeric_limits<float>::min();
   for (const auto& table : mSharedDataPtr->SelectTableSet(aStateId, aPolarization))
   {
      auto& values = table.mTablePtr->mDataValues.GetValues();
      auto  minmax = std::minmax_element(values.begin(), values.end());
      minSig       = std::min(minSig, *minmax.first);
      maxSig       = std::max(maxSig, *minmax.second);
   }
   return {minSig, maxSig};
}

void WsfStandardRadarSignature::SelectTable(UtAzElLookup&             aContext,
                                            WsfStringId               aState,
                                            WsfEM_Types::Polarization aPolarization,
                                            double                    aFrequency)
{
   mSharedDataPtr->SelectTable(aContext, aState, aPolarization, aFrequency);
}

// =================================================================================================
//! @returns The signatures for all of defined states.
//! @note This method was created to allow applications access to
//! signature data. It should not be used to look up signatures.
const std::vector<WsfStandardRadarSignature::State>& WsfStandardRadarSignature::GetStates() const
{
   return mSharedDataPtr->mStates;
}

// =================================================================================================
//! Add a table to the definition.
//! @param aTablePtr Pointer to the table to be added.
//! @param aStateId  String ID of the state name for which the table applies.
//! @param aPolarization Signal polarization to which the table applies.
//! @param aFrequencyLimit Upper bound of the frequency to which the table applies.
//! @returns true if table added or false if a table for the requested state/polarization
//! @note This is provided to allow an application to dynamically create a signature. It is intended
//! only for very simple uses such as in sensor_plot RadarEnvelopeFunction. It is NOT a general
//! purpose method.
bool WsfStandardRadarSignature::AddTable(UtAzElTable*              aTablePtr,
                                         WsfStringId               aStateId,
                                         WsfEM_Types::Polarization aPolarization,
                                         double                    aFrequencyLimit)
{
   return mSharedDataPtr->AddTable(aTablePtr, aStateId, aPolarization, aFrequencyLimit);
}

// =================================================================================================
WsfStandardRadarSignature::SharedData::~SharedData()
{
   for (auto& state : mStates)
   {
      for (TableSet& tables : state.mPolarization)
      {
         for (auto& table : tables)
         {
            if (!table.mIsACopy)
            {
               delete table.mTablePtr;
            }
         }
      }
   }
}

// =================================================================================================
//! Initialize the radar signature.
//!
//! This *MUST* be called prior to calling SelectTable.  It performs the following:
//!
//! - Verifies that the signature state 'default' is defined.
//! - Verifies that the 'default' polarization signature is defined for each signature state.
//!
//! For every polarization within a signature state that does not have a signature defined,
//! this routine will create a copy of the signature for the 'default' polarization.  This
//! Speeds up signature selection in SelectTable.
//!
//! @returns 'true' if successful or 'false' if the required 'default' data is not present.
bool WsfStandardRadarSignature::SharedData::InitializeType(WsfObject& aBase)
{
   bool ok = true;

   // Make sure the signature state 'default' exists.

   StateIndex stateIndex;
   for (stateIndex = 0; stateIndex < mStates.size(); ++stateIndex)
   {
      if (mStates[stateIndex].mStateId == UtStringIdLiteral("default"))
      {
         break;
      }
   }

   mDefaultStateIndex = stateIndex;
   if (stateIndex >= mStates.size())
   {
      ok = false;
      ut::log::info() << "The signature state 'default' does not exist.";
   }

   // Ensure each state has a signature for the 'cDEFAULT' polarization.

   for (stateIndex = 0; stateIndex < mStates.size(); ++stateIndex)
   {
      if (mStates[stateIndex].mPolarization[WsfEM_Types::cPOL_DEFAULT].empty())
      {
         ok       = false;
         auto out = ut::log::info() << "The 'default' polarization does not exist for state.";
         out.AddNote() << "State: " << mStates[stateIndex].mStateId;
      }
      else
      {
         // For each polarization within a state that does not have defined signature,
         // use the signature for the 'cDEFAULT' polarization.

         UseDefaultPolarization(mStates[stateIndex], WsfEM_Types::cPOL_HORIZONTAL);
         UseDefaultPolarization(mStates[stateIndex], WsfEM_Types::cPOL_VERTICAL);
         UseDefaultPolarization(mStates[stateIndex], WsfEM_Types::cPOL_SLANT_45);
         UseDefaultPolarization(mStates[stateIndex], WsfEM_Types::cPOL_SLANT_135);
         UseDefaultPolarization(mStates[stateIndex], WsfEM_Types::cPOL_LEFT_CIRCULAR);
         UseDefaultPolarization(mStates[stateIndex], WsfEM_Types::cPOL_RIGHT_CIRCULAR);
      }

      // Ensure that the interpolation flag is set in each table's context
      for (auto& polarization : mStates[stateIndex].mPolarization)
      {
         for (auto& table : polarization)
         {
            table.mTablePtr->SetInterpolationFlag(mInterpolateTables);
            table.mTablePtr->SetInterpolationType(mInterpolationType);
         }
      }
   }
   return ok;
}

// =================================================================================================
bool WsfStandardRadarSignature::SharedData::IsA_ValidState(WsfStringId aId) const
{
   if (aId == UtStringIdLiteral("default"))
   {
      return true;
   } // default is always true
   bool found = false;
   for (const auto& state : mStates)
   {
      if (state.mStateId == aId)
      {
         found = true;
         break;
      }
   }
   return found;
}

// =================================================================================================
bool WsfStandardRadarSignature::SharedData::ProcessInput(UtInput& aInput, WsfObject& aBase)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "state")
   {
      ProcessInputState(aInput, aBase);
   }
   else if (command == "polarization")
   {
      ProcessInputPolarization(aInput, aBase);
   }
   else if (command == "frequency_limit")
   {
      ProcessInputFrequencyLimit(aInput, aBase);
   }
   else if (command == "use_bisector_for_bistatic")
   {
      aInput.ReadValue(mUseBisectorForBistatic);
   }
   else if (command == "interpolate_tables")
   {
      aInput.ReadValue(mInterpolateTables);
   }
   else if (command == "interpolation_type")
   {
      std::string iType;
      aInput.ReadValue(iType);
      if (!ut::azel::util::StringToEnum(mInterpolationType, iType))
      {
         throw UtInput::BadValue(aInput, "Invalid interpolation type: " + iType);
      }
   }
   else
   {
      myCommand = ProcessInputTableData(aInput, aBase);
   }
   return myCommand;
}

// =================================================================================================
void WsfStandardRadarSignature::SharedData::ProcessInputState(UtInput& aInput, WsfObject& aBase)
{
   std::string stateName;
   aInput.ReadValue(stateName);
   mCurrentStateId = WsfStringId(stateName);
   // Make sure the new state has not been used
   for (auto& state : mStates)
   {
      if (mCurrentStateId == state.mStateId)
      {
         throw UtInput::BadValue(aInput, "Duplicate signature state: " + stateName);
      }
   }

   // Reset polarization and frequency limit
   mCurrentPolarization   = WsfEM_Types::cPOL_DEFAULT;
   mCurrentFrequencyLimit = 1.0E+30;
}

// =================================================================================================
void WsfStandardRadarSignature::SharedData::ProcessInputPolarization(UtInput& aInput, WsfObject& aBase)
{
   std::string polarization;
   aInput.ReadValue(polarization);
   if (!WsfEM_Util::StringToEnum(mCurrentPolarization, polarization))
   {
      throw UtInput::BadValue(aInput, "Invalid polarization: " + polarization);
   }

   // Reset frequency limit when new polarization encountered.
   mCurrentFrequencyLimit = 1.0E+30;

   // Make sure the requested polarization does not already exist in the current state.

   if ((!mStates.empty()) && (mCurrentStateId == mStates.back().mStateId))
   {
      State& state = mStates.back();
      if (!state.mPolarization[mCurrentPolarization].empty())
      {
         throw UtInput::BadValue(aInput, "duplicate polarization with the state");
      }
   }
}

// =================================================================================================
void WsfStandardRadarSignature::SharedData::ProcessInputFrequencyLimit(UtInput& aInput, WsfObject& aBase)
{
   aInput.ReadValueOfType(mCurrentFrequencyLimit, UtInput::cFREQUENCY);
   aInput.ValueGreater(mCurrentFrequencyLimit, 0.0);

   // Make sure the frequency limits are monotonically increasing.

   if ((!mStates.empty()) && (mCurrentStateId == mStates.back().mStateId))
   {
      State&    state  = mStates.back();
      TableSet& tables = state.mPolarization[mCurrentPolarization];
      if ((!tables.empty()) && (mCurrentFrequencyLimit <= tables.back().mFrequencyLimit))
      {
         throw UtInput::BadValue(aInput, "frequency_limits must increase monotonically");
      }
   }
}

// =================================================================================================
bool WsfStandardRadarSignature::SharedData::ProcessInputTableData(UtInput& aInput, WsfObject& aBase)
{
   UtAzElTableLoader::PolFreqTableMapType tableMap;
   std::string                            tableUnits("m^2");
   bool                                   myCommand = UtAzElTableLoader::ProcessTable(aInput, tableMap, tableUnits);
   if (myCommand && !tableMap.empty())
   {
      UtStringUtil::ToLower(tableUnits);

      for (auto& polIt : tableMap)
      {
         for (auto freqIt : polIt.second)
         {
            if (polIt.first == "default")
            {
               // Don't convert table values because default table contains pointers
               // to tables in another polarization, and we don't want to do the
               // conversion twice.
            }
            else if (tableUnits == "dbsm")
            {
               // Input values are in dBsm
               freqIt.second->ConvertValuesFromDB(1.0F);
            }
            else
            {
               // Input values were dimensional
               try
               {
                  double multiplier = aInput.ConvertValue(1.0, tableUnits, UtInput::cAREA);
                  freqIt.second->MultiplyValues(static_cast<float>(multiplier));
               }
               catch (...)
               {
                  delete freqIt.second;
                  throw;
               }
            }

            if (!polIt.first.empty())
            {
               WsfEM_Util::StringToEnum(mCurrentPolarization, polIt.first);
            }
            if (!AddTable(freqIt.second,
                          mCurrentStateId,
                          mCurrentPolarization,
                          (freqIt.first == 0) ? mCurrentFrequencyLimit : freqIt.first,
                          polIt.first == "default")) // Default polarization created as a copy of vertical or horizontal
            {
               delete freqIt.second;
               throw UtInput::BadValue(aInput, "Failed to add table to radar signature.");
            }
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Add a new table to the radar signature.
//! @param aTablePtr Pointer to the table to be added.
//! @param aStateId  String ID of the state name for which the table applies.
//! @param aPolarization Signal polarization to which the table applies.
//! @param aFrequencyLimit Upper bound of the frequency to which the table applies.
//! @param aIsACopy If 'false' then the table data is owned by the object.
//!                 If 'true' then it is owned by some other instance of this object.
//! @returns true if table added or false if a table for the requested state/polarization
//! has already been defined.
bool WsfStandardRadarSignature::SharedData::AddTable(UtAzElTable*              aTablePtr,
                                                     WsfStringId               aStateId,
                                                     WsfEM_Types::Polarization aPolarization,
                                                     double                    aFrequencyLimit,
                                                     bool                      aIsACopy)
{
   bool ok = true;

   StateIndex stateIndex;
   for (stateIndex = 0; stateIndex < mStates.size(); ++stateIndex)
   {
      if (aStateId == mStates[stateIndex].mStateId)
      {
         break;
      }
   }

   if (stateIndex >= mStates.size())
   {
      // Creating a new state.
      mStates.emplace_back(aStateId);
      Table table(aFrequencyLimit, aTablePtr, aIsACopy);
      mStates.back().mPolarization[aPolarization].push_back(table);
   }
   else if (mStates[stateIndex].mPolarization[aPolarization].empty())
   {
      // Creating a new polarization within the current state.
      Table table(aFrequencyLimit, aTablePtr, aIsACopy);
      mStates[stateIndex].mPolarization[aPolarization].push_back(table);
   }
   else
   {
      // Creating a new frequency limit within the current polarization.
      // Frequencies must be monotonically ascending.
      if (aFrequencyLimit > mStates[stateIndex].mPolarization[aPolarization].back().mFrequencyLimit)
      {
         Table table(aFrequencyLimit, aTablePtr, aIsACopy);
         mStates[stateIndex].mPolarization[aPolarization].push_back(table);
      }
      else
      {
         ok = false;
      }
   }
   return ok;
}

// =================================================================================================
//! Select the table set for a given signature state and signal polarization.
//! @param aStateId      [input] The string ID representing the signature state to be used.
//! @param aPolarization [input] The polarization of the signal.
const WsfStandardRadarSignature::TableSet&
WsfStandardRadarSignature::SharedData::SelectTableSet(WsfStringId aStateId, WsfEM_Types::Polarization aPolarization) const
{
   // Locate the state among the defined states.

   StateIndex stateIndex;
   for (stateIndex = 0; stateIndex < mStates.size(); ++stateIndex)
   {
      if (aStateId == mStates[stateIndex].mStateId)
      {
         break;
      }
   }

   // If the requested state was not found then use the default state.

   if (stateIndex >= mStates.size())
   {
      stateIndex = mDefaultStateIndex;
   }

   // Within the state, search the polarization to locate the proper signature table.

   const TableSet& tables = mStates[stateIndex].mPolarization[aPolarization];
   return tables;
}

// =================================================================================================
//! Select a radar signature table for a given signature state, signal polarization and signal frequency.
//! @param aContext      [output] The updated table lookup context representing the selected
//!                               signature table.
//! @param aStateId      [input] The string ID representing the signature state to be used.
//! @param aPolarization [input] The polarization of the signal.
//! @param aFrequency    [input] The frequency of the signal.
void WsfStandardRadarSignature::SharedData::SelectTable(UtAzElLookup&             aContext,
                                                        WsfStringId               aStateId,
                                                        WsfEM_Types::Polarization aPolarization,
                                                        double                    aFrequency)
{
   const TableSet& tables = SelectTableSet(aStateId, aPolarization);
   TableIndex      tableIndex;
   for (tableIndex = 0; tableIndex < tables.size(); ++tableIndex)
   {
      if (aFrequency < tables[tableIndex].mFrequencyLimit)
      {
         tables[tableIndex].mTablePtr->GetContext(aContext);
         return;
      }
   }

   // A signature with the appropriate frequency limit did not exist for the requested
   // polarization.  Simply use the last signature for the last frequency.

   if (!tables.empty())
   {
      tables.back().mTablePtr->GetContext(aContext);
      return;
   }

   // This should NEVER happen because InitializeType() has guaranteed that every polarization
   // at least one table.
   ut::log::error() << "Signature not correctly initialized.";
}

// =================================================================================================
//! If a signature has not been defined for a given polarization then use the signature
//! for the default polarization.
//!
//! @param aState        [input] The state containing the polarization to be checked.
//! @param aPolarization [input] The polarization within the state to be checked.
// private
void WsfStandardRadarSignature::SharedData::UseDefaultPolarization(State& aState, int aPolarization)
{
   if (aState.mPolarization[aPolarization].empty())
   {
      // Copy the tables from the default polarization to the requested polarization.
      // Each table is marked as a 'reference' to indicate that the actual definition
      // occurs somewhere else.  This prevents the destructor from deleting it.

      TableSet& sourceTables = aState.mPolarization[WsfEM_Types::cPOL_DEFAULT];
      TableSet& targetTables = aState.mPolarization[aPolarization];
      for (auto& sourceTable : sourceTables)
      {
         Table table(sourceTable.mFrequencyLimit, sourceTable.mTablePtr, true);
         targetTables.push_back(table);
      }
   }
}

// =================================================================================================
//! Constructor to create a table reference.
//! @param aFrequencyLimit the maximum frequency to which this table applies.
//! @param aTablePtr Pointer to the table data that defines the signature.
//! @param aIsACopy If 'false' then the table data is owned by the object.
//! If 'true' then it is owned by some other instance of this object. (This is
//! used to propagate default data to other polarizations).
WsfStandardRadarSignature::Table::Table(double aFrequencyLimit, UtAzElTable* aTablePtr, bool aIsACopy)
   : mFrequencyLimit(aFrequencyLimit)
   , mTablePtr(aTablePtr)
   , mIsACopy(aIsACopy)
{
}

WsfStandardRadarSignature::State::State(WsfStringId aStateId)
   : mStateId(aStateId)
{
}
