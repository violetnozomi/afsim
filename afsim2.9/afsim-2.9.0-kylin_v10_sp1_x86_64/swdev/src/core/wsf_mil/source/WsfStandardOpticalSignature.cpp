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

#include "WsfStandardOpticalSignature.hpp"

#include "UtAzElLookup.hpp"
#include "UtAzElTable.hpp"
#include "UtAzElTableLoader.hpp"
#include "UtAzElUtil.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtStringIdLiteral.hpp"
#include "UtStringUtil.hpp"

// =================================================================================================
//! Factory method called by WsfOpticalSignatureTypes.
// static
WsfOpticalSignature* WsfStandardOpticalSignature::ObjectFactory(const std::string& aTypeName)
{
   WsfOpticalSignature* instancePtr = nullptr;
   if (aTypeName == "WSF_OPTICAL_SIGNATURE")
   {
      instancePtr = new WsfStandardOpticalSignature();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfOpticalSignature* WsfStandardOpticalSignature::Clone() const
{
   return new WsfStandardOpticalSignature(*this);
}

// =================================================================================================
// virtual
bool WsfStandardOpticalSignature::InitializeType()
{
   bool ok = WsfOpticalSignature::InitializeType();
   ok &= mSharedDataPtr->InitializeType(*this);
   return ok;
}

// =================================================================================================
// virtual
std::vector<WsfStringId> WsfStandardOpticalSignature::GetStateNames() const
{
   const auto& states = GetStates();
   return GetStateNamesP<State, &State::mStateId>(states.begin(), states.end());
}

// =================================================================================================
// virtual
bool WsfStandardOpticalSignature::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mSharedDataPtr->ProcessInput(aInput, *this))
   {
   }
   else
   {
      myCommand = WsfOpticalSignature::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
float WsfStandardOpticalSignature::GetSignature(double aSimTime, WsfStringId aStateId, double aAzimuth, double aElevation)
{
   UtAzElLookup context;
   mSharedDataPtr->SelectTable(context, aStateId);
   return context.Lookup(aAzimuth, aElevation);
}

// =================================================================================================
//! @returns The signatures for all of defined states.
//! @note This method was created to allow applications access to
//! signature data. It should not be used to look up signatures.
const std::vector<WsfStandardOpticalSignature::State>& WsfStandardOpticalSignature::GetStates() const
{
   return mSharedDataPtr->mStates;
}

// =================================================================================================
WsfStandardOpticalSignature::SharedData::~SharedData()
{
   for (auto& state : mStates)
   {
      delete state.mTablePtr;
   }
}

// =================================================================================================
bool WsfStandardOpticalSignature::SharedData::InitializeType(WsfObject& aBase)
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
      ut::log::error() << "The signature state 'default' does not exist.";
   }

   // Ensure that the interpolation flag is set in each tables context
   for (auto& state : mStates)
   {
      if (state.mTablePtr != nullptr)
      {
         state.mTablePtr->SetInterpolationFlag(mInterpolateTables);
         state.mTablePtr->SetInterpolationType(mInterpolationType);
      }
   }

   return ok;
}

// =================================================================================================
bool WsfStandardOpticalSignature::SharedData::IsA_ValidState(WsfStringId aId) const
{
   if (aId == UtStringIdLiteral("default"))
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
bool WsfStandardOpticalSignature::SharedData::ProcessInput(UtInput& aInput, WsfObject& aBase)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "state")
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
      UtAzElTable* tablePtr = nullptr;
      std::string  tableUnits("m^2");
      myCommand = UtAzElTableLoader::ProcessTable(aInput, tablePtr, tableUnits);
      if (myCommand && tablePtr != nullptr)
      {
         UtStringUtil::ToLower(tableUnits);
         if (tableUnits == "dbsm")
         {
            // Input values are in dBsm
            tablePtr->ConvertValuesFromDB(1.0F);
         }
         else
         {
            // Input values were dimensional
            try
            {
               double multiplier = aInput.ConvertValue(1.0, tableUnits, UtInput::cAREA);
               tablePtr->MultiplyValues(static_cast<float>(multiplier));
            }
            catch (...)
            {
               delete tablePtr;
               throw;
            }
         }
         if (!AddTable(tablePtr, mCurrentStateId))
         {
            delete tablePtr;
            throw UtInput::BadValue(aInput, "Failed to add table to optical signature.");
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Add a new table to the optical signature.
//! @param aTablePtr Pointer to the table to be added.
//! @param aStateId  String ID of the state name for which the table applies.
//! @returns true if table added or false if a table for the requested state has already been defined.
bool WsfStandardOpticalSignature::SharedData::AddTable(UtAzElTable* aTablePtr, WsfStringId aStateId)
{
   // Make sure the table doesn't duplicate another table.
   for (auto& state : mStates)
   {
      if (aStateId == state.mStateId)
      {
         return false;
      }
   }

   mStates.emplace_back(aStateId, aTablePtr);
   return true;
}

// =================================================================================================
//! Select a optical signature based on the supplied signature state.
//!
//! @param aContext [output] The lookup context for the requested signature state.
//! @param aStateId [input] The string ID of the optical signature state.
void WsfStandardOpticalSignature::SharedData::SelectTable(UtAzElLookup& aContext, WsfStringId aStateId)
{
   for (auto& state : mStates)
   {
      if (aStateId == state.mStateId)
      {
         state.mTablePtr->GetContext(aContext);
         return;
      }
   }

   // Requested signature state not found.  Use the signature for the default state.

   mStates[mDefaultStateIndex].mTablePtr->GetContext(aContext);
}

// =================================================================================================
WsfStandardOpticalSignature::State::State(WsfStringId aStateId, UtAzElTable* aTablePtr)
   : mStateId(aStateId)
   , mTablePtr(aTablePtr)
{
}
