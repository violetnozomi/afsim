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

#include "WsfStandardInherentContrast.hpp"

#include "UtAzElLookup.hpp"
#include "UtAzElTable.hpp"
#include "UtAzElTableLoader.hpp"
#include "UtAzElUtil.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtStringIdLiteral.hpp"

// =================================================================================================
//! Factory method called by WsfInherentContrastTypes.
// static
WsfInherentContrast* WsfStandardInherentContrast::ObjectFactory(const std::string& aTypeName)
{
   WsfInherentContrast* instancePtr = nullptr;
   if (aTypeName == "WSF_INHERENT_CONTRAST")
   {
      instancePtr = new WsfStandardInherentContrast();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfStandardInherentContrast* WsfStandardInherentContrast::Clone() const
{
   return new WsfStandardInherentContrast(*this);
}

// =================================================================================================
// virtual
bool WsfStandardInherentContrast::InitializeType()
{
   bool ok = WsfInherentContrast::InitializeType();
   ok &= mSharedDataPtr->InitializeType(*this);
   return ok;
}

// =================================================================================================
// virtual
std::vector<WsfStringId> WsfStandardInherentContrast::GetStateNames() const
{
   const auto& states = mSharedDataPtr->mStates;
   return GetStateNamesP<State, &State::mStateId>(states.begin(), states.end());
}

// =================================================================================================
// virtual
bool WsfStandardInherentContrast::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mSharedDataPtr->ProcessInput(aInput, *this))
   {
   }
   else
   {
      myCommand = WsfInherentContrast::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
float WsfStandardInherentContrast::GetInherentContrast(WsfStringId aStateId, double aAzimuth, double aElevation)
{
   UtAzElLookup context;
   mSharedDataPtr->SelectTable(context, aStateId);
   return context.Lookup(aAzimuth, aElevation);
}

// =================================================================================================
WsfStandardInherentContrast::SharedData::~SharedData()
{
   for (auto& state : mStates)
   {
      delete state.mTablePtr;
   }
}

// =================================================================================================
bool WsfStandardInherentContrast::SharedData::InitializeType(WsfObject& aBase)
{
   bool ok = true;

   // Make sure the contrast state 'default' exists.

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
      ut::log::error() << "The contrast state 'default' does not exist.";
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
bool WsfStandardInherentContrast::SharedData::IsA_ValidState(WsfStringId aId) const
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
bool WsfStandardInherentContrast::SharedData::ProcessInput(UtInput& aInput, WsfObject& aBase)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "state")
   {
      std::string stateName;
      aInput.ReadValue(stateName);
      mCurrentStateId = stateName;
      // Make sure the new state has not been used
      for (auto& state : mStates)
      {
         if (mCurrentStateId == state.mStateId)
         {
            throw UtInput::BadValue(aInput, "Duplicate contrast state: " + stateName);
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
      std::string  tableUnits("dummy");
      myCommand = UtAzElTableLoader::ProcessTable(aInput, tablePtr, tableUnits);
      if (myCommand && tablePtr != nullptr)
      {
         if (!AddTable(tablePtr, mCurrentStateId))
         {
            delete tablePtr;
            throw UtInput::BadValue(aInput, "Failed to add table to inherent contrast.");
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Add a new table to the inherent contrast.
//! @param aTablePtr Pointer to the table to be added.
//! @param aStateId  String ID of the state name for which the table applies.
//! @returns true if table added or false if a table for the requested state has already been defined.
bool WsfStandardInherentContrast::SharedData::AddTable(UtAzElTable* aTablePtr, WsfStringId aStateId)
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
//! Select a inherent contrast based on the supplied contrast state.
//! @param aContext [output] The lookup context for the requested contrast state.
//! @param aStateId [input] The string ID of the inherent contrast state.
void WsfStandardInherentContrast::SharedData::SelectTable(UtAzElLookup& aContext, WsfStringId aStateId)
{
   for (auto& state : mStates)
   {
      if (aStateId == state.mStateId)
      {
         state.mTablePtr->GetContext(aContext);
         return;
      }
   }

   // Requested contrast state not found.  Use the contrast for the default state.

   mStates[mDefaultStateIndex].mTablePtr->GetContext(aContext);
}

// =================================================================================================
WsfStandardInherentContrast::State::State(WsfStringId aStateId, UtAzElTable* aTablePtr)
   : mStateId(aStateId)
   , mTablePtr(aTablePtr)
{
}
