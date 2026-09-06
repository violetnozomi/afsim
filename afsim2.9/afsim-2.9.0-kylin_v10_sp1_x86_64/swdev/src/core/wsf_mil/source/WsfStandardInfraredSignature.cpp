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

#include "WsfStandardInfraredSignature.hpp"

#include <algorithm>

#include "UtAzElLookup.hpp"
#include "UtAzElTable.hpp"
#include "UtAzElTableLoader.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtStringUtil.hpp"
#include "WsfEM_Util.hpp"

// =================================================================================================
//! Factory method called by WsfInfraredSignatureTypes.
// static
WsfInfraredSignature* WsfStandardInfraredSignature::ObjectFactory(const std::string& aTypeName)
{
   WsfInfraredSignature* instancePtr = nullptr;
   if (aTypeName == "WSF_INFRARED_SIGNATURE")
   {
      instancePtr = new WsfStandardInfraredSignature();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfInfraredSignature* WsfStandardInfraredSignature::Clone() const
{
   return new WsfStandardInfraredSignature(*this);
}

// =================================================================================================
// virtual
bool WsfStandardInfraredSignature::InitializeType()
{
   bool ok = WsfInfraredSignature::InitializeType();
   ok &= mSharedDataPtr->InitializeType(*this);
   return ok;
}

// =================================================================================================
// virtual
std::vector<WsfStringId> WsfStandardInfraredSignature::GetStateNames() const
{
   const auto& states = GetStates();
   return GetStateNamesP<State, &State::mStateId>(states.begin(), states.end());
}

// =================================================================================================
// virtual
bool WsfStandardInfraredSignature::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mSharedDataPtr->ProcessInput(aInput, *this))
   {
   }
   else
   {
      myCommand = WsfInfraredSignature::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
float WsfStandardInfraredSignature::GetSignature(double                    aSimTime,
                                                 WsfStringId               aStateId,
                                                 WsfEM_Types::InfraredBand aBand,
                                                 double                    aAzimuth,
                                                 double                    aElevation)
{
   UtAzElLookup context;
   mSharedDataPtr->SelectTable(context, aStateId, aBand);
   return context.Lookup(aAzimuth, aElevation);
}

// =================================================================================================
//! @returns The signatures for all of defined states.
//! @note This method was created to allow applications access to
//! signature data. It should not be used to look up signatures.
const std::vector<WsfStandardInfraredSignature::State>& WsfStandardInfraredSignature::GetStates() const
{
   return mSharedDataPtr->mStates;
}

// =================================================================================================
WsfStandardInfraredSignature::SharedData::~SharedData()
{
   for (State& state : mStates)
   {
      for (auto& band : state.mBands)
      {
         if (!band.mIsACopy)
         {
            delete band.mTablePtr;
         }
      }
   }
}

// =================================================================================================
bool WsfStandardInfraredSignature::SharedData::InitializeType(WsfObject& aBase)
{
   bool ok = true;

   StateIndex stateIndex;

   // Ensure the 'default' state exists.
   mDefaultStateIndex = mStates.size();
   for (stateIndex = 0; stateIndex < mStates.size(); ++stateIndex)
   {
      State& state = mStates[stateIndex];
      if (state.mStateId == UtStringIdLiteral("default"))
      {
         mDefaultStateIndex = stateIndex;
      }
   }

   if (mDefaultStateIndex >= mStates.size())
   {
      ok       = false;
      auto out = ut::log::error() << "The signature state 'default' does not exist.";
      out.AddNote() << "Base Type: " << aBase.GetType();
   }

   if (ok)
   {
      // Make sure the default state is the first state. This is needed so defaults
      // can be properly assigned in the next loop.

      // swap(mStates[0], mStates[mDefaultStateIndex]);
      // mDefaultStateIndex = 0;

      // Ensure that every true band in every state has a definition.

      for (stateIndex = 0; stateIndex < mStates.size(); ++stateIndex)
      {
         if (!AssignDefaultTable(aBase, mStates[stateIndex], WsfEM_Types::cIRBAND_SHORT))
         {
            ok = false;
         }

         if (!AssignDefaultTable(aBase, mStates[stateIndex], WsfEM_Types::cIRBAND_MEDIUM))
         {
            ok = false;
         }

         if (!AssignDefaultTable(aBase, mStates[stateIndex], WsfEM_Types::cIRBAND_LONG))
         {
            ok = false;
         }

         if (!AssignDefaultTable(aBase, mStates[stateIndex], WsfEM_Types::cIRBAND_VERY_LONG))
         {
            ok = false;
         }

         // Ensure that the interpolation flag is set in each tables context
         for (auto& band : mStates[stateIndex].mBands)
         {
            if (band.mTablePtr)
            {
               band.mTablePtr->SetInterpolationFlag(mInterpolateTables);
            }
         }
      }
   }

   return ok;
}

// =================================================================================================
bool WsfStandardInfraredSignature::SharedData::IsA_ValidState(WsfStringId aId) const
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
bool WsfStandardInfraredSignature::SharedData::ProcessInput(UtInput& aInput, WsfObject& aBase)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "state")
   {
      std::string stateName;
      aInput.ReadValue(stateName);
      mCurrentStateId = WsfStringId(stateName);

      // Make sure the new state does not currently exist.

      for (auto& state : mStates)
      {
         if (mCurrentStateId == state.mStateId)
         {
            throw UtInput::BadValue(aInput, "Duplicate signature state: " + stateName);
         }
      }

      // Reset the band
      mCurrentBand = WsfEM_Types::cIRBAND_DEFAULT;
   }
   else if (command == "band")
   {
      std::string bandName;
      aInput.ReadValue(bandName);
      WsfEM_Types::InfraredBand band;
      if (!WsfEM_Util::StringToEnum(band, bandName))
      {
         throw UtInput::BadValue(aInput, "Invalid infrared band: " + bandName);
      }

      // Make sure the band doesn't currently exist.

      if ((!mStates.empty()) && (mCurrentStateId == mStates.back().mStateId))
      {
         if (mStates.back().mBands[band].mTablePtr != nullptr)
         {
            throw UtInput::BadValue(aInput, "Duplicate band: " + bandName);
         }
      }

      mCurrentBand = band;
   }
   else if (command == "interpolate_tables")
   {
      aInput.ReadValue(mInterpolateTables);
   }
   else if (command == "query")
   {
      if (!InitializeType(aBase))
      {
         auto out = ut::log::error() << "Could not initialize 'infrared_signature'.";
         out.AddNote() << "Base Type: " << aBase.GetType();
         throw UtInput::BadValue(aInput);
      }

      std::string               stateName;
      std::string               bandName;
      WsfEM_Types::InfraredBand band;
      double                    azimuth;
      double                    elevation;

      aInput.ReadValue(stateName);
      aInput.ReadValue(bandName);
      if (!WsfEM_Util::StringToEnum(band, bandName))
      {
         throw UtInput::BadValue(aInput, "Invalid infrared band: " + bandName);
      }
      aInput.ReadValueOfType(azimuth, UtInput::cANGLE);
      aInput.ReadValueOfType(elevation, UtInput::cANGLE);

      UtAzElLookup sigTable;
      SelectTable(sigTable, WsfStringId(stateName), band);
      float value = sigTable.Lookup(azimuth, elevation);
      auto  out   = ut::log::info() << "Signature Query:";
      out.AddNote() << "Type: " << aBase.GetType();
      out.AddNote() << "State: " << stateName;
      out.AddNote() << "Band: " << WsfEM_Util::EnumToString(band);
      out.AddNote() << "Az: " << azimuth * UtMath::cDEG_PER_RAD;
      out.AddNote() << "El: " << elevation * UtMath::cDEG_PER_RAD;
      out.AddNote() << "Value: " << value;
   }
   else
   {
      UtAzElTable* tablePtr = nullptr;
      std::string  tableUnits("watts/steradian");
      myCommand = UtAzElTableLoader::ProcessTable(aInput, tablePtr, tableUnits);
      if (myCommand && (tablePtr != nullptr))
      {
         try
         {
            UtStringUtil::ToLower(tableUnits);
            std::string::size_type slashPos = tableUnits.find('/');
            if ((slashPos != std::string::npos) && (slashPos != 0) && ((slashPos + 1) != tableUnits.size()))
            {
               std::string powerUnits(tableUnits.substr(0, slashPos));
               std::string angleUnits(tableUnits.substr(slashPos + 1));
               double      powerFactor = aInput.ConvertValue(1.0, powerUnits, UtInput::cPOWER);
               double      angleFactor = aInput.ConvertValue(1.0, angleUnits, UtInput::cSOLID_ANGLE);
               double      multiplier  = powerFactor / angleFactor;
               tablePtr->MultiplyValues(static_cast<float>(multiplier));
            }
            else
            {
               throw UtInput::BadValue(aInput, "Unknown radiant intensity units " + tableUnits);
            }
         }
         catch (...)
         {
            delete tablePtr;
            throw;
         }

         if (!AddTable(tablePtr, mCurrentStateId, mCurrentBand))
         {
            delete tablePtr;
            throw UtInput::BadValue(aInput, "Failed to add table to infrared signature.");
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Add a new table to the infrared signature.
//! @param aTablePtr Pointer to the table to be added.
//! @param aStateId  String ID of the state name for which the table applies.
//! @param aBand     The infrared band for which the table applies.
//! @returns true if table added or false if a table for the requested state/polarization
//! has already been defined.
bool WsfStandardInfraredSignature::SharedData::AddTable(UtAzElTable*              aTablePtr,
                                                        WsfStringId               aStateId,
                                                        WsfEM_Types::InfraredBand aBand)
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
      mStates.back().mBands[aBand].mTablePtr = aTablePtr;
      mStates.back().mBands[aBand].mIsACopy  = false;
   }
   else
   {
      // Creating a new band.  Make sure that it doesn't duplicate an existing band.
      if (mStates[stateIndex].mBands[aBand].mTablePtr != nullptr)
      {
         return false;
      }
      mStates[stateIndex].mBands[aBand].mTablePtr = aTablePtr;
      mStates[stateIndex].mBands[aBand].mIsACopy  = false;
   }
   return ok;
}

// =================================================================================================
//! Select a infrared signature table for a given signature state and band.
//! @param aContext [output] The updated table lookup context representing the selected
//! signature table.
//! @param aStateId [input] The string ID representing the signature state to be used.
//! @param aBand    [input] The band of the table to be used.
void WsfStandardInfraredSignature::SharedData::SelectTable(UtAzElLookup&             aContext,
                                                           WsfStringId               aStateId,
                                                           WsfEM_Types::InfraredBand aBand)
{
   // Locate the state among the defined states.

   StateIndex stateIndex = mDefaultStateIndex; // Assume the default state.
   for (StateIndex tempIndex = 0; tempIndex < mStates.size(); ++tempIndex)
   {
      if (aStateId == mStates[tempIndex].mStateId)
      {
         stateIndex = tempIndex;
         break;
      }
   }

   // Within the state, select the table for the correct band.

   if (aBand < WsfEM_Types::cIRBAND_COUNT)
   {
      Table& table = mStates[stateIndex].mBands[aBand];
      if (table.mTablePtr != nullptr)
      {
         table.mTablePtr->GetContext(aContext);
         return;
      }
   }

   // This should NEVER happen because InitializeType() has guaranteed that every state
   // has every band defined or has a default band defined.
   ut::log::error() << "Infrared Signature: Internal Error.";
   throw;
   // mDefaultSignaturePtr->mStates[0].mBands[WsfEM_Types::cIRBAND_DEFAULT].mTablePtr->GetContext(aContext);
}

// =================================================================================================
//! If necessary, assign a default table for the select state and band.
//! If the given state and band does not have a signature defined then try to
//! assign a signature according to the following precedence:
//! -) The default band from the selected state
//! -) The selected band from the default state
//! -) The default band from the default state.
// private
bool WsfStandardInfraredSignature::SharedData::AssignDefaultTable(WsfObject&                aBase,
                                                                  State&                    aState,
                                                                  WsfEM_Types::InfraredBand aBand)
{
   bool ok = true;
   if (aState.mBands[aBand].mTablePtr == nullptr)
   {
      State& defaultState = mStates[mDefaultStateIndex];
      if (aState.mBands[WsfEM_Types::cIRBAND_DEFAULT].mTablePtr != nullptr)
      {
         // Use the default band from the current state.
         aState.mBands[aBand].mTablePtr = aState.mBands[WsfEM_Types::cIRBAND_DEFAULT].mTablePtr;
         aState.mBands[aBand].mIsACopy  = true;
      }
      else if (defaultState.mBands[aBand].mTablePtr != nullptr)
      {
         // Use the selected band from the default state.
         aState.mBands[aBand].mTablePtr = defaultState.mBands[aBand].mTablePtr;
         aState.mBands[aBand].mIsACopy  = true;
      }
      else if (defaultState.mBands[WsfEM_Types::cIRBAND_DEFAULT].mTablePtr != nullptr)
      {
         // Use the default band from the default state.
         aState.mBands[aBand].mTablePtr = defaultState.mBands[WsfEM_Types::cIRBAND_DEFAULT].mTablePtr;
         aState.mBands[aBand].mIsACopy  = true;
      }
      else
      {
         auto out = ut::log::error() << "Undefined 'infrared_signature'.";
         out.AddNote() << "Type: " << aBase.GetType();
         out.AddNote() << "State: " << WsfStringId(aState.mStateId).GetString();
         out.AddNote() << "Band: " << WsfEM_Util::EnumToString(aBand);
         ok = false;
      }
   }
   return ok;
}

// =================================================================================================
//! Constructor to create a table reference.
//! @param aTablePtr Pointer to the table data that defines the signature.
//! @param aIsACopy If 'false' then the table data is owned by the object.
//! If 'true' then it is owned by some other instance of this object.
WsfStandardInfraredSignature::Table::Table(UtAzElTable* aTablePtr, bool aIsACopy)
   : mTablePtr(aTablePtr)
   , mIsACopy(aIsACopy)
{
}

// =================================================================================================
WsfStandardInfraredSignature::State::State(WsfStringId aStateId)
   : mStateId(aStateId)
   , mBands(WsfEM_Types::cIRBAND_COUNT)
{
}
