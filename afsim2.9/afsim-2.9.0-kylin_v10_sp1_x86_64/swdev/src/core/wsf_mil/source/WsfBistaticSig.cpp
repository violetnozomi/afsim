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

#include "WsfBistaticSig.hpp"

#include <fstream>
#include <sstream>

#include "UtAzElTableLoader.hpp"
#include "UtAzElUtil.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtStringUtil.hpp"
#include "UtVec3.hpp"
#include "WsfEM_Util.hpp"
#include "WsfException.hpp"
#include "WsfStringId.hpp"

// =================================================================================================
// virtual
WsfBistaticSig::~WsfBistaticSig()
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
// virtual
WsfBistaticSig* WsfBistaticSig::Clone() const
{
   throw WsfUncloneableException(); // Signatures cannot be cloned
}

// =================================================================================================
//! Initialize the bistatic signature.
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
// virtual
bool WsfBistaticSig::Initialize()
{
   bool ok = true;
   // Make sure the signature state 'default' exists.
   StateIndex stateIndex;
   for (stateIndex = 0; stateIndex < mStates.size(); ++stateIndex)
   {
      if (mStates[stateIndex].mStateId == "default")
      {
         break;
      }
   }

   mDefaultStateIndex = stateIndex;
   if (stateIndex >= mStates.size())
   {
      ok = false;
      ut::log::info() << "The bistatic signature state 'default' does not exist.";
   }

   // Ensure each state has a signature for the 'cDEFAULT' polarization.
   for (stateIndex = 0; stateIndex < mStates.size(); ++stateIndex)
   {
      if (mStates[stateIndex].mPolarization[WsfEM_Types::cPOL_DEFAULT].empty())
      {
         ok       = false;
         auto out = ut::log::warning() << "The 'default' polarization does not exist for bistatic state.";
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

      for (unsigned int polarization = 0; polarization < WsfEM_Types::cPOL_COUNT; ++polarization)
      {
         for (auto& table : mStates[stateIndex].mPolarization[polarization])
         {
            if (!table.mTablePtr->Validate(table.mTablePtr->mTgtToXmtrAz.GetSize(), table.mTablePtr->mTgtToXmtrEl.GetSize()))
            {
               ok &= false;
               auto out = ut::log::warning() << "Bistatic signature state does not have the required minimum of at "
                                                "least two azimuth/elevation sets defined.";
               out.AddNote() << "State: " << mStates[stateIndex].mStateId;
               out.AddNote() << "Polarization: "
                             << WsfEM_Util::EnumToString(static_cast<WsfEM_Types::Polarization>(polarization));
            }

            size_t dim1;
            size_t dim2;
            table.mTablePtr->mAzElTables.GetSize(dim1, dim2);
            for (size_t i = 0; i < dim1; ++i)
            {
               for (size_t j = 0; j < dim2; ++j)
               {
                  table.mTablePtr->mAzElTables.Get(i, j)->SetInterpolationFlag(mInterpolateTables);
                  table.mTablePtr->mAzElTables.Get(i, j)->SetInterpolationType(mInterpolationType);
               }
            }
         }
      }
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfBistaticSig::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "interpolate_transmitter_angles")
   {
      aInput.ReadValue(mInterpTgtToXmtrAngles);
   }
   else if (command == "transmitter_angles_interpolation_type")
   {
      std::string sIType;
      aInput.ReadValue(sIType);
      if (!ut::azel::util::StringToEnum(mInterpTgtToXmtrType, sIType))
      {
         std::string msg = "Invalid interpolation type: " + sIType + " - type must be Linear or Logarithmic";
         throw UtInput::BadValue(aInput, msg);
      }
   }
   else if (command == "state")
   {
      ProcessInputState(aInput);
   }
   else if (command == "polarization")
   {
      ProcessInputPolarization(aInput);
   }
   else if (command == "frequency_limit")
   {
      ProcessInputFrequencyLimit(aInput);
   }
   else if (command == "azimuth")
   {
      ProcessInputAzimuth(aInput);
   }
   else if (command == "elevation")
   {
      ProcessInputElevation(aInput);
   }
   else if (command == "interpolate_tables")
   {
      aInput.ReadValue(mInterpolateTables);
   }
   else if (command == "interpolation_type")
   {
      std::string sIType;
      aInput.ReadValue(sIType);
      if (!ut::azel::util::StringToEnum(mInterpolationType, sIType))
      {
         std::string msg = "Invalid interpolation type: " + sIType + " - type must be Linear or Logarithmic";
         throw UtInput::BadValue(aInput, msg);
      }
   }
   else
   {
      myCommand = ProcessInputTableData(aInput);
   }
   return myCommand;
}

// =================================================================================================
void WsfBistaticSig::ProcessInputState(UtInput& aInput)
{
   std::string stateName;
   aInput.ReadValue(stateName);
   mCurrentStateId = stateName;
   // Make sure the new state has not been used
   for (auto& state : mStates)
   {
      if (mCurrentStateId == state.mStateId)
      {
         throw UtInput::BadValue(aInput, "Duplicate signature state: " + stateName);
      }
   }

   // Reset current polarization, frequency limit, azimuth and elevation when new state encountered.
   mCurrentPolarization   = WsfEM_Types::cPOL_DEFAULT;
   mCurrentFrequencyLimit = 1.0E+30;
   mCurrentTgtToXmtrAz    = -UtMath::cPI;
   mCurrentTgtToXmtrEl    = -UtMath::cPI_OVER_2;
   mCurrentAzimuthIndex   = -1;
   mCurrentElevationIndex = -1;
}

// =================================================================================================
void WsfBistaticSig::ProcessInputPolarization(UtInput& aInput)
{
   std::string polarization;
   aInput.ReadValue(polarization);
   if (!WsfEM_Util::StringToEnum(mCurrentPolarization, polarization))
   {
      throw UtInput::BadValue(aInput, "Invalid polarization: " + polarization);
   }

   // Reset current frequency limit, azimuth and elevation when new polarization encountered.
   mCurrentFrequencyLimit = 1.0E+30;
   mCurrentTgtToXmtrAz    = -UtMath::cPI;
   mCurrentTgtToXmtrEl    = -UtMath::cPI_OVER_2;
   mCurrentAzimuthIndex   = -1;
   mCurrentElevationIndex = -1;

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
void WsfBistaticSig::ProcessInputFrequencyLimit(UtInput& aInput)
{
   aInput.ReadValueOfType(mCurrentFrequencyLimit, UtInput::cFREQUENCY);
   aInput.ValueGreater(mCurrentFrequencyLimit, 0.0);

   // Reset the current azimuth and elevation when new polarization encountered.
   mCurrentTgtToXmtrAz    = -UtMath::cPI;
   mCurrentTgtToXmtrEl    = -UtMath::cPI_OVER_2;
   mCurrentAzimuthIndex   = -1;
   mCurrentElevationIndex = -1;

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
void WsfBistaticSig::ProcessInputAzimuth(UtInput& aInput)
{
   aInput.ReadValueOfType(mCurrentTgtToXmtrAz, UtInput::cANGLE);
   aInput.ValueInClosedRange(mCurrentTgtToXmtrAz, -UtMath::cPI, UtMath::cPI);

   ++mCurrentAzimuthIndex;

   // Make sure the azimuths are monotonically increasing.
   if (!mStates.empty())
   {
      State&    state  = mStates.back();
      TableSet& tables = state.mPolarization[mCurrentPolarization];
      if (!tables.empty())
      {
         AzElAzElTable* azElAzElTable = tables.back().mTablePtr;
         if ((azElAzElTable != nullptr) && (mCurrentAzimuthIndex > 0))
         {
            if (mCurrentTgtToXmtrAz <= azElAzElTable->mTgtToXmtrAz.Get(mCurrentAzimuthIndex - 1))
            {
               throw UtInput::BadValue(aInput, "azimuth values must increase monotonically");
            }
         }
      }
   }
   mCurrentElevationIndex = -1;
}

// =================================================================================================
void WsfBistaticSig::ProcessInputElevation(UtInput& aInput)
{
   aInput.ReadValueOfType(mCurrentTgtToXmtrEl, UtInput::cANGLE);
   aInput.ValueInClosedRange(mCurrentTgtToXmtrEl, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);

   ++mCurrentElevationIndex;

   // Make sure the elevations are monotonically increasing.
   if (!mStates.empty())
   {
      State&    state  = mStates.back();
      TableSet& tables = state.mPolarization[mCurrentPolarization];
      if (!tables.empty())
      {
         AzElAzElTable* azElAzElTable = tables.back().mTablePtr;
         if (azElAzElTable != nullptr)
         {
            if (mCurrentAzimuthIndex > 0)
            {
               if (mCurrentTgtToXmtrEl != azElAzElTable->mTgtToXmtrEl.Get(mCurrentElevationIndex))
               {
                  throw UtInput::BadValue(aInput, "elevation values must  be equally spaces among azimuths");
               }
            }
            else if (mCurrentElevationIndex > 0)
            {
               if (mCurrentTgtToXmtrEl <= azElAzElTable->mTgtToXmtrEl.Get(mCurrentElevationIndex - 1))
               {
                  throw UtInput::BadValue(aInput, "elevation values must increase monotonically");
               }
            }
         }
      }
   }
}

// =================================================================================================
bool WsfBistaticSig::ProcessInputTableData(UtInput& aInput)
{
   UtAzElTableLoader::PolFreqTableMapType tableMap;
   std::string                            tableUnits("m^2");
   bool                                   myCommand = UtAzElTableLoader::ProcessTable(aInput, tableMap, tableUnits);
   if (myCommand && (!tableMap.empty()))
   {
      UtStringUtil::ToLower(tableUnits);

      auto polIt = tableMap.begin();
      for (; polIt != tableMap.end(); ++polIt)
      {
         auto freqIt = polIt->second.begin();
         for (; freqIt != polIt->second.end(); ++freqIt)
         {
            if (polIt->first == "default")
            {
               // Don't convert table values because default table contains pointers
               // to tables in another polarization, and we don't want to do the
               // conversion twice.
            }
            else if (tableUnits == "dbsm")
            {
               // Input values are in dBsm
               freqIt->second->ConvertValuesFromDB(1.0F);
            }
            else
            {
               // Input values were dimensional
               try
               {
                  double multiplier = aInput.ConvertValue(1.0, tableUnits, UtInput::cAREA);
                  freqIt->second->MultiplyValues(static_cast<float>(multiplier));
               }
               catch (...)
               {
                  delete freqIt->second;
                  throw;
               }
            }

            if (!polIt->first.empty())
            {
               WsfEM_Util::StringToEnum(mCurrentPolarization, polIt->first);
            }

            if (!AddTable(freqIt->second,
                          mCurrentStateId,
                          mCurrentPolarization,
                          (freqIt->first == 0) ? mCurrentFrequencyLimit : freqIt->first,
                          mCurrentTgtToXmtrAz,
                          mCurrentTgtToXmtrEl,
                          polIt->first == "default")) // Default polarization created as a copy of vertical or horizontal
            {
               delete freqIt->second;
               throw UtInput::BadValue(aInput, "Failed to add table to bistatic signature.");
            }
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Add a new table to the bistatic signature.
//!
//! @param aTablePtr       Pointer to the table to be added.
//! @param aStateId        String ID of the state name for which the table applies.
//! @param aPolarization   Signal polarization to which the table applies.
//! @param aFrequencyLimit Upper bound of the frequency to which the table applies.
//! @param aIsACopy        If 'false' then the table data is owned by the object.
//!                        If 'true' then it is owned by some other instance of this object.
//! @returns true if table added or false if a table for the requested state/polarization
//! has already been defined.
bool WsfBistaticSig::AddTable(AzElAzElTable*            aTablePtr,
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
//! Add a new table to the bistatic signature.
//!
//! @param aTablePtr       Pointer to the table to be added.
//! @param aStateId        String ID of the state name for which the table applies.
//! @param aPolarization   Signal polarization to which the table applies.
//! @param aFrequencyLimit Upper bound of the frequency to which the table applies.
//! @param aTgtToXmtrAz    The lower bound target->transmitter azimuth   angle to which the table applies.
//! @param aTgtToXmtrEl    The lower bound target->transmitter elevation angle to which the table applies.
//! @param aIsACopy        If 'false' then the table data is owned by the object.
//!                        If 'true' then it is owned by some other instance of this object.
//! @returns true if table added or false if a table for the requested state/polarization
//! has already been defined.
bool WsfBistaticSig::AddTable(UtAzElTable*              aTablePtr,
                              WsfStringId               aStateId,
                              WsfEM_Types::Polarization aPolarization,
                              double                    aFrequencyLimit,
                              double                    aTgtToXmtrAz,
                              double                    aTgtToXmtrEl,
                              bool                      aIsACopy)
{
   bool ok = true;

   AzElAzElTable* currentContext(nullptr);

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
      currentContext = new AzElAzElTable();
      Table table(aFrequencyLimit, currentContext, aIsACopy);
      mStates.back().mPolarization[aPolarization].push_back(table);
   }
   else if (mStates[stateIndex].mPolarization[aPolarization].empty())
   {
      // Creating a new polarization within the current state.
      currentContext = new AzElAzElTable();
      Table table(aFrequencyLimit, currentContext, aIsACopy);
      mStates[stateIndex].mPolarization[aPolarization].push_back(table);
   }
   else
   {
      Table& table = mStates[stateIndex].mPolarization[aPolarization].back();

      // Creating a new frequency limit within the current polarization.
      // Frequencies must be monotonically ascending.
      if (aFrequencyLimit > table.mFrequencyLimit)
      {
         currentContext = new AzElAzElTable();
         Table table2(aFrequencyLimit, currentContext, aIsACopy);
         mStates[stateIndex].mPolarization[aPolarization].push_back(table2);
      }
      else if (aFrequencyLimit == table.mFrequencyLimit)
      {
         currentContext = mStates[stateIndex].mPolarization[aPolarization].back().mTablePtr;
      }
      else if (aFrequencyLimit < table.mFrequencyLimit)
      {
         TableIndex tableIndex;
         for (tableIndex = 0; tableIndex < mStates[stateIndex].mPolarization[aPolarization].size(); ++tableIndex)
         {
            if (aFrequencyLimit == mStates[stateIndex].mPolarization[aPolarization][tableIndex].mFrequencyLimit)
            {
               currentContext = mStates[stateIndex].mPolarization[aPolarization][tableIndex].mTablePtr;
               break;
            }
         }

         if (currentContext == nullptr)
         {
            ok = false;
         }
      }
   }

   if (ok)
   {
      if (currentContext == nullptr)
      {
         currentContext = mStates[stateIndex].mPolarization[aPolarization].back().mTablePtr;
      }

      // double azIndex(0);
      if (currentContext->mTgtToXmtrAz.GetSize() == 0)
      {
         currentContext->mTgtToXmtrAz.Resize(currentContext->mTgtToXmtrAz.GetSize() + 1, mCurrentTgtToXmtrAz);
      }
      else if (mCurrentTgtToXmtrAz > currentContext->mTgtToXmtrAz.Get(currentContext->mTgtToXmtrAz.GetSize() - 1))
      {
         // Monotonically increasing azimuth.
         currentContext->mTgtToXmtrAz.Resize(currentContext->mTgtToXmtrAz.GetSize() + 1, mCurrentTgtToXmtrAz);
         // azIndex = currentContext->mTgtToXmtrAz.GetSize();
      }
      // else if (mCurrentTgtToXmtrAz == currentContext->mTgtToXmtrAz.Get(currentContext->mTgtToXmtrAz.GetSize() - 1))
      //{
      //    // For an index that already exists
      //    //azIndex = currentContext->mTgtToXmtrAz.GetSize() - 1;
      // }
      else if (mCurrentTgtToXmtrAz < currentContext->mTgtToXmtrAz.Get(currentContext->mTgtToXmtrAz.GetSize() - 1))
      {
         // Azimuth value is not monotonically increasing
         ok = false;
      }

      // double elIndex(0);
      if (currentContext->mTgtToXmtrEl.GetSize() == 0)
      {
         currentContext->mTgtToXmtrEl.Resize(currentContext->mTgtToXmtrEl.GetSize() + 1, mCurrentTgtToXmtrEl);
      }
      else if (currentContext->mTgtToXmtrAz.GetSize() > 1)
      {
         if (mCurrentTgtToXmtrEl != currentContext->mTgtToXmtrEl.Get(mCurrentElevationIndex))
         {
            ok = false;
         }
      }
      else if ((mCurrentTgtToXmtrEl > currentContext->mTgtToXmtrEl.Get(currentContext->mTgtToXmtrEl.GetSize() - 1)))
      {
         // Monotonically increasing elevation.
         currentContext->mTgtToXmtrEl.Resize(currentContext->mTgtToXmtrEl.GetSize() + 1, mCurrentTgtToXmtrEl);
         // elIndex = currentContext->mTgtToXmtrEl.GetSize();
      }
      // else if (mCurrentTgtToXmtrEl == currentContext->mTgtToXmtrEl.Get(mCurrentElevationIndex - 1))
      //{
      //    // For an index that already exists
      //    //elIndex = currentContext->mTgtToXmtrEl.GetSize() - 1;
      // }
      else if (mCurrentTgtToXmtrEl < currentContext->mTgtToXmtrEl.Get(currentContext->mTgtToXmtrEl.GetSize() - 1))
      {
         // Elevation value is not monotonically increasing
         ok = false;
      }

      if (ok)
      {
         currentContext->mAzElTables.Resize(currentContext->mTgtToXmtrAz.GetSize(), currentContext->mTgtToXmtrEl.GetSize());
         currentContext->mAzElTables.Set(aTablePtr, currentContext->mTgtToXmtrAz.GetSize() - 1, mCurrentElevationIndex);
      }
   }
   return ok;
}

// =================================================================================================
//! Get the bistatic signature for a given set of conditions.
//!
//! @param aContext      [output] The updated table lookup context representing the selected signature table.
//! @param aStateId      [input] The string ID representing the signature state to be used.
//! @param aPolarization [input] The polarization of the signal.
//! @param aFrequency    [input] The frequency of the signal (Hz).
//! @param aTgtToXmtrAz  [input] The azimuth   of the transmitter with respect to the target.
//! @param aTgtToXmtrEl  [input] The elevation of the transmitter with respect to the target.
//! @param aTgtToRcvrAz  [input] The azimuth   of the receiver    with respect to the target.
//! @param aTgtToRcvrEl  [input] The elevation of the receiver    with respect to the target.
//! @returns The bistatic cross section (m^2)
float WsfBistaticSig::GetSignature(UtAzElLookup&             aContext,
                                   WsfStringId               aStateId,
                                   WsfEM_Types::Polarization aPolarization,
                                   double                    aFrequency,
                                   double                    aTgtToXmtrAz,
                                   double                    aTgtToXmtrEl,
                                   double                    aTgtToRcvrAz,
                                   double                    aTgtToRcvrEl)
{
   AzElAzElTable azElAzElTable;
   SelectTable(azElAzElTable, aStateId, aPolarization, aFrequency);

   size_t tgtToXmtrAzIndex;
   size_t tgtToXmtrElIndex;
   if (mInterpTgtToXmtrAngles)
   {
      azElAzElTable.mTgtToXmtrAzLU.Lookup(azElAzElTable.mTgtToXmtrAz, aTgtToXmtrAz);
      azElAzElTable.mTgtToXmtrElLU.Lookup(azElAzElTable.mTgtToXmtrEl, aTgtToXmtrEl);
      tgtToXmtrAzIndex = azElAzElTable.mTgtToXmtrAzLU.GetIndex();
      tgtToXmtrElIndex = azElAzElTable.mTgtToXmtrElLU.GetIndex();
   }
   else
   {
      azElAzElTable.mTgtToXmtrAzU.Lookup(azElAzElTable.mTgtToXmtrAz, aTgtToXmtrAz);
      azElAzElTable.mTgtToXmtrElU.Lookup(azElAzElTable.mTgtToXmtrEl, aTgtToXmtrEl);
      tgtToXmtrAzIndex = azElAzElTable.mTgtToXmtrAzU.GetIndex();
      tgtToXmtrElIndex = azElAzElTable.mTgtToXmtrElU.GetIndex();
   }

   UtAzElLookup t00;
   UtAzElLookup t01;
   UtAzElLookup t10;
   UtAzElLookup t11;
   azElAzElTable.mAzElTables.Get(tgtToXmtrAzIndex, tgtToXmtrElIndex)->GetContext(t00);
   azElAzElTable.mAzElTables.Get(tgtToXmtrAzIndex, tgtToXmtrElIndex + 1)->GetContext(t01);
   azElAzElTable.mAzElTables.Get(tgtToXmtrAzIndex + 1, tgtToXmtrElIndex)->GetContext(t10);
   azElAzElTable.mAzElTables.Get(tgtToXmtrAzIndex + 1, tgtToXmtrElIndex + 1)->GetContext(t11);

   float f00 = t00.Lookup(aTgtToRcvrAz, aTgtToRcvrEl);
   float f01 = t01.Lookup(aTgtToRcvrAz, aTgtToRcvrEl);
   float f10 = t10.Lookup(aTgtToRcvrAz, aTgtToRcvrEl);
   float f11 = t11.Lookup(aTgtToRcvrAz, aTgtToRcvrEl);

   float r1;
   float r2;
   if (mInterpTgtToXmtrAngles)
   {
      r1 = static_cast<float>(azElAzElTable.mTgtToXmtrAzLU.GetRatio());
      r2 = static_cast<float>(azElAzElTable.mTgtToXmtrElLU.GetRatio());
   }
   else
   {
      r1 = static_cast<float>(azElAzElTable.mTgtToXmtrAzU.GetRatio());
      r2 = static_cast<float>(azElAzElTable.mTgtToXmtrElU.GetRatio());
   }

   aContext = t00;

   switch (mInterpTgtToXmtrType)
   {
   case InterpolationType::cLogarithmic:
   {
      double r12 = static_cast<double>(r1 * r2);
      double t1  = pow(f11, r12);                 // f11 ^ (r1*r2)
      double t2  = pow(f10, (r1 - r12));          // f10 ^ (r1*(1-r2))
      double t3  = pow(f01, (r2 - r12));          // f01 ^ (r2*(1-r1))
      double t4  = pow(f00, (1 - r1 - r2 + r12)); // f00 ^ ((1-r1)*(1-r2))
      return static_cast<float>(t1 * t2 * t3 * t4);
   }
   case InterpolationType::cLinear:
      return f00 + ((f10 - f00) * r1) + ((f01 - f00) + (f11 - f10 - (f01 - f00)) * r1) * r2;
   default:
      throw std::logic_error("Invalid interpolation type");
   }
}

// =================================================================================================
//! Select a bistatic signature table for a given signature state, signal polarization and
//! signal frequency.
//!
//! @param aContext      [output] The updated table lookup context representing the selected
//!                               signature table.
//! @param aStateId      [input] The string ID representing the signature state to be used.
//! @param aPolarization [input] The polarization of the signal.
//! @param aFrequency    [input] The frequency of the signal.
void WsfBistaticSig::SelectTable(AzElAzElTable&            aContext,
                                 WsfStringId               aStateId,
                                 WsfEM_Types::Polarization aPolarization,
                                 double                    aFrequency)
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

   TableSet&  tables = mStates[stateIndex].mPolarization[aPolarization];
   TableIndex tableIndex;
   for (tableIndex = 0; tableIndex < tables.size(); ++tableIndex)
   {
      if (aFrequency < tables[tableIndex].mFrequencyLimit)
      {
         aContext = *tables[tableIndex].mTablePtr;
         return;
      }
   }

   // A signature with the appropriate frequency limit did not exist for the requested
   // polarization.  Simply use the last signature for the last frequency.

   if (!tables.empty())
   {
      aContext = *tables.back().mTablePtr;
      return;
   }

   // This should NEVER happen because Initialize() has guaranteed that every polarization
   // at least one table.  But we do this just to cover ourselves...

   WsfBistaticSig* defaultSigPtr = GetDefaultSig();
   aContext                      = *defaultSigPtr->mStates[0].mPolarization[WsfEM_Types::cPOL_DEFAULT].back().mTablePtr;
}

// =================================================================================================
//! Provide a default signature to be used if a signature is not defined.
//! @return Pointer to the default signature (1000 m^2).
// static
WsfBistaticSig* WsfBistaticSig::GetDefaultSig()
{
   static WsfBistaticSig defaultSig;
   static bool           firstTime = true;

   if (firstTime)
   {
      // Create a default signature of 1000 m^2

      firstTime               = false;
      AzElAzElTable* tablePtr = new AzElAzElTable();
      tablePtr->CreateConstantTable(1000.0);

      defaultSig.AddTable(tablePtr, WsfStringId("default"), WsfEM_Types::cPOL_DEFAULT, 1.0E+30);
   }
   return &defaultSig;
}

// =================================================================================================
//! If a signature has not been defined for a given polarization then use the signature
//! for the default polarization.
//!
//! @param aState [input]        The state containing the polarization to be checked.
//! @param aPolarization [input] The polarization within the state to be checked.
// private
void WsfBistaticSig::UseDefaultPolarization(State& aState, int aPolarization)
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
//! Checks to make the entered state Id exists, returns true if it exists, false otherwise.
//! @param aId the name Id of the state.
//! @return Returns true if provided state exists.
bool WsfBistaticSig::IsA_ValidState(WsfStringId aId)
{
   if (aId == "default")
   {
      return true;
   } // default is always true
   bool found = false;
   for (auto& state : mStates)
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
//! The following is a convenience method to create a minimal
//! table that contains a constant value.
void WsfBistaticSig::AzElAzElTable::CreateConstantTable(float aValue)
{
   mTgtToXmtrAz.Set(static_cast<float>(-UtMath::cPI), 0);
   mTgtToXmtrAz.Set(static_cast<float>(UtMath::cPI), 1);
   mTgtToXmtrEl.Set(static_cast<float>(-UtMath::cPI_OVER_2), 0);
   mTgtToXmtrEl.Set(static_cast<float>(UtMath::cPI_OVER_2), 1);

   UtAzElTable* tablePtr = new UtAzElTable();
   tablePtr->CreateConstantTable(aValue);

   mAzElTables.Set(tablePtr, 0, 0);
   mAzElTables.Set(tablePtr, 0, 1);
   mAzElTables.Set(tablePtr, 1, 0);
   mAzElTables.Set(tablePtr, 1, 1);

   Validate(2, 2); // Just for completeness (ensures the table context is built)
}

// =================================================================================================
//! Ensure the table contains valid data.  This should ALWAYS be called right
//! after loading the data into the table.
//!
//! @returns 'true' if everything is OK, otherwise 'false'.
bool WsfBistaticSig::AzElAzElTable::Validate(size_t aTgtToRcvrAzCount, size_t aTgtToRcvrElCount)
{
   bool ok = false;

   size_t tgtToXmtrAzCount = mTgtToXmtrAz.GetSize();
   size_t tgtToXmtrElCount = mTgtToXmtrEl.GetSize();

   if ((tgtToXmtrAzCount >= 2) && (tgtToXmtrElCount >= 2) && (tgtToXmtrAzCount == aTgtToRcvrAzCount) &&
       (tgtToXmtrElCount == aTgtToRcvrElCount) && (mTgtToXmtrEl.Get(0) >= -(UtMath::cPI_OVER_2 + 1.0E-6)) &&
       (mTgtToXmtrEl.Get(tgtToXmtrElCount - 1) <= (UtMath::cPI_OVER_2 + 1.0E-6)) &&
       (mTgtToXmtrAz.Get(0) >= -(UtMath::cPI + 1.0E-6)) &&
       (mTgtToXmtrAz.Get(tgtToXmtrAzCount - 1) <= (UtMath::cPI + 1.0E-6)))
   {
      ok = true;
      for (size_t i = 1; i < tgtToXmtrElCount; ++i)
      {
         if (mTgtToXmtrEl.Get(i) <= mTgtToXmtrEl.Get(i - 1))
         {
            ok = false;
         }
      }

      for (size_t i = 1; i < tgtToXmtrAzCount; ++i)
      {
         if (mTgtToXmtrAz.Get(i) <= mTgtToXmtrAz.Get(i - 1))
         {
            ok = false;
         }
      }

      for (size_t i = 0; i < tgtToXmtrAzCount; ++i)
      {
         for (size_t j = 0; j < tgtToXmtrElCount; ++j)
         {
            ok &= mAzElTables.Get(i, j) != nullptr;
         }
      }
   }
   // mContext.SetTable(this);            // Point the lookup context at this table.
   return ok;
}

// =================================================================================================
//! Constructor to create a table reference.
//! @param aFrequencyLimit the maximum frequency to which this table applies.
//! @param aTablePtr Pointer to the table data that defines the signature.
//! @param aIsACopy If 'false' then the table data is owned by the object.
//! If 'true' then it is owned by some other instance of this object. (This is
//! used to propagate default data to other polarizations).
WsfBistaticSig::Table::Table(double aFrequencyLimit, AzElAzElTable* aTablePtr, bool aIsACopy)
   : mFrequencyLimit(aFrequencyLimit)
   , mTablePtr(aTablePtr)
   , mIsACopy(aIsACopy)
{
}

// =================================================================================================
WsfBistaticSig::State::State(WsfStringId aStateId)
   : mStateId(aStateId)
{
}
