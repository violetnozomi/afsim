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

#include "WsfStandardOpticalReflectivity.hpp"

#include <algorithm>

#include "UtAzElLookup.hpp"
#include "UtAzElTable.hpp"
#include "UtAzElTableLoader.hpp"
#include "UtAzElUtil.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringIdLiteral.hpp"

// =================================================================================================
//! Factory method called by WsfOpticalReflectivityTypes.
// static
WsfOpticalReflectivity* WsfStandardOpticalReflectivity::ObjectFactory(const std::string& aTypeName)
{
   WsfOpticalReflectivity* instancePtr = nullptr;
   if (aTypeName == "WSF_OPTICAL_REFLECTIVITY")
   {
      instancePtr = new WsfStandardOpticalReflectivity();
   }
   return instancePtr;
}

// =================================================================================================
// virtual
WsfStandardOpticalReflectivity* WsfStandardOpticalReflectivity::Clone() const
{
   return new WsfStandardOpticalReflectivity(*this);
}

// =================================================================================================
// virtual
bool WsfStandardOpticalReflectivity::InitializeType()
{
   bool ok = WsfOpticalReflectivity::InitializeType();
   ok &= mSharedDataPtr->InitializeType(*this);
   return ok;
}

// =================================================================================================
// virtual
std::vector<WsfStringId> WsfStandardOpticalReflectivity::GetStateNames() const
{
   const auto& states = GetStates();
   return GetStateNamesP<State, &State::mStateId>(states.begin(), states.end());
}

// =================================================================================================
// virtual
bool WsfStandardOpticalReflectivity::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mSharedDataPtr->ProcessInput(aInput, *this))
   {
   }
   else
   {
      myCommand = WsfOpticalReflectivity::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
float WsfStandardOpticalReflectivity::GetOpticalReflectivity(double /*aSimTime*/,
                                                             WsfStringId aStateId,
                                                             double      aWavelength,
                                                             double      aTgtToXmtrAz,
                                                             double      aTgtToXmtrEl,
                                                             double      aTgtToRcvrAz,
                                                             double      aTgtToRcvrEl)
{
   UtAzElLookup context;
   mSharedDataPtr->SelectTable(context, aStateId, aWavelength);
   float value = context.Lookup(aTgtToXmtrAz, aTgtToXmtrEl);
   // Convert scalar reflectance to a BRDF (1/sr).
   value /= static_cast<float>(UtMath::cPI);
   return std::max(value, 1.0E-30F);
}

// =================================================================================================
//! @returns The signatures for all of defined states.
//! @note This method was created to allow applications access to
//! signature data. It should not be used to look up signatures.
const std::vector<WsfStandardOpticalReflectivity::State>& WsfStandardOpticalReflectivity::GetStates() const
{
   return mSharedDataPtr->mStates;
}

// =================================================================================================
WsfStandardOpticalReflectivity::SharedData::~SharedData()
{
   for (auto& mState : mStates)
   {
      TableSet& tables = mState.mTables;
      for (auto& table : tables)
      {
         delete table.mTablePtr;
      }
   }
}

// =================================================================================================
//! Initialize the optical reflectivity.
//! @returns 'true' if successful or 'false' if the required 'default' data is not present.
bool WsfStandardOpticalReflectivity::SharedData::InitializeType(WsfStandardOpticalReflectivity& aObject)
{
   bool ok = true;

   // Make sure the configuration state 'default' exists.
   size_t stateIndex;
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
      for (auto& table : state.mTables)
      {
         table.mTablePtr->SetInterpolationFlag(mInterpolateTables);
         table.mTablePtr->SetInterpolationType(mInterpolationType);
      }
   }

   return ok;
}

// =================================================================================================
bool WsfStandardOpticalReflectivity::SharedData::IsA_ValidState(WsfStringId aId) const
{
   if (aId == UtStringIdLiteral("default"))
   {
      return true;
   } // default is always true
   bool found = false;
   for (const auto& mState : mStates)
   {
      if (mState.mStateId == aId)
      {
         found = true;
         break;
      }
   }
   return found;
}

// =================================================================================================
bool WsfStandardOpticalReflectivity::SharedData::ProcessInput(UtInput& aInput, WsfStandardOpticalReflectivity& aObject)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "state")
   {
      ProcessInputState(aInput, aObject);
   }
   else if (command == "wavelength_limit")
   {
      ProcessInputWavelengthLimit(aInput, aObject);
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
   else if (command == "query")
   {
      ProcessInputQuery(aInput, aObject);
   }
   else
   {
      myCommand = ProcessInputTableData(aInput, aObject);
   }
   return myCommand;
}

// =================================================================================================
void WsfStandardOpticalReflectivity::SharedData::ProcessInputState(UtInput& aInput, WsfStandardOpticalReflectivity& aObject)
{
   std::string stateName;
   aInput.ReadValue(stateName);
   mCurrentStateId = WsfStringId(stateName);
   // Make sure the new state has not been used
   for (auto& mState : mStates)
   {
      if (mCurrentStateId == mState.mStateId)
      {
         throw UtInput::BadValue(aInput, "Duplicate signature state: " + stateName);
      }
   }

   // Reset wavelength limit
   mCurrentWavelengthLimit = 1.0E+30;
}

// =================================================================================================
void WsfStandardOpticalReflectivity::SharedData::ProcessInputWavelengthLimit(UtInput&                        aInput,
                                                                             WsfStandardOpticalReflectivity& aObject)
{
   aInput.ReadValueOfType(mCurrentWavelengthLimit, UtInput::cLENGTH);
   aInput.ValueGreater(mCurrentWavelengthLimit, 0.0);

   // Make sure the wavelength limits are monotonically increasing.

   if ((!mStates.empty()) && (mCurrentStateId == mStates.back().mStateId))
   {
      State&    state  = mStates.back();
      TableSet& tables = state.mTables;
      if ((!tables.empty()) && (mCurrentWavelengthLimit <= tables.back().mWavelengthLimit))
      {
         throw UtInput::BadValue(aInput, "wavelength_limits must increase monotonically");
      }
   }
}

// =================================================================================================
void WsfStandardOpticalReflectivity::SharedData::ProcessInputQuery(UtInput& aInput, WsfStandardOpticalReflectivity& aObject)
{
   std::string state;
   double      wavelength;
   double      tgtToXmtrAz; // float tgtToXmtrAz;
   double      tgtToXmtrEl; // float tgtToXmtrEl;
   double      tgtToRcvrAz; // float tgtToRcvrAz;
   double      tgtToRcvrEl; // float tgtToRcvrEl;
   aInput.ReadValue(state);
   aInput.ReadValueOfType(wavelength, UtInput::cLENGTH);
   aInput.ValueGreater(wavelength, 0.0);
   aInput.ReadValueOfType(tgtToXmtrAz, UtInput::cANGLE);
   aInput.ValueInClosedRange(tgtToXmtrAz, -UtMath::cPI, UtMath::cPI);
   aInput.ReadValueOfType(tgtToXmtrEl, UtInput::cANGLE);
   aInput.ReadValueOfType(tgtToRcvrAz, UtInput::cANGLE);
   aInput.ValueInClosedRange(tgtToRcvrAz, -UtMath::cPI, UtMath::cPI);
   aInput.ReadValueOfType(tgtToRcvrEl, UtInput::cANGLE);
   aInput.ValueInClosedRange(tgtToRcvrEl, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   std::string expect;
   aInput.ReadValue(expect);
   float expectedValue = 1000.0F;
   if (expect == "expect")
   {
      aInput.ReadValue(expectedValue);
      aInput.ValueInClosedRange(expectedValue, 0.0F, 1.0F);
   }
   else
   {
      aInput.PushBack(expect);
   }
   if (aObject.InitializeType())
   {
      double simTime = 0.0;
      float  value =
         aObject.GetOpticalReflectivity(simTime, state, wavelength, tgtToXmtrAz, tgtToXmtrEl, tgtToRcvrAz, tgtToRcvrEl);
      auto out = ut::log::info() << "Query State:";
      out.AddNote() << "State: " << state;
      out.AddNote() << "Wavelength: " << wavelength * 1.0E9 << " nm";
      { // RAII block
         auto note = out.AddNote() << "Tgt->Xmtr:";
         note.AddNote() << "Az: " << tgtToXmtrAz * UtMath::cDEG_PER_RAD << " deg";
         note.AddNote() << "El: " << tgtToXmtrEl * UtMath::cDEG_PER_RAD << " deg";
      }
      { // RAII block
         auto note = out.AddNote() << "Tgt->Rcvr:";
         note.AddNote() << "Az: " << tgtToRcvrAz * UtMath::cDEG_PER_RAD << " deg";
         note.AddNote() << "El: " << tgtToRcvrEl * UtMath::cDEG_PER_RAD << " deg";
      }

      if (expectedValue < 1000.0)
      {
         auto note = out.AddNote() << "Value: " << value;
         note.AddNote() << "Expected: " << expectedValue;
         note.AddNote() << "Difference: " << value - expectedValue;

         if (fabs(value - expectedValue) > 1.0E-3)
         {
            out.Send();
            ut::log::info("test_fail") << "-FAIL- Optical reflectivity mismatch";
         }
      }
   }
}

// =================================================================================================
bool WsfStandardOpticalReflectivity::SharedData::ProcessInputTableData(UtInput&                        aInput,
                                                                       WsfStandardOpticalReflectivity& aObject)
{
   UtAzElTable* tablePtr = nullptr;
   std::string  tableUnits("dummy"); // Indicates that values are non-dimensional
   bool         myCommand = UtAzElTableLoader::ProcessTable(aInput, tablePtr, tableUnits);
   if (myCommand && tablePtr != nullptr)
   {
      if (!AddTable(tablePtr, mCurrentStateId, mCurrentWavelengthLimit))
      {
         delete tablePtr;
         throw UtInput::BadValue(aInput, "Failed to add table to optical_reflectivity.");
      }
   }
   return myCommand;
}

// =================================================================================================
//! Add a new table to the radar signature.
//! @param aTablePtr Pointer to the table to be added.
//! @param aStateId  String ID of the state name for which the table applies.
//! @param aWavelengthLimit Upper bound of the wavelength to which the table applies.
//! @returns true if table added or false if a table for the requested state/polarization
//! has already been defined.
bool WsfStandardOpticalReflectivity::SharedData::AddTable(UtAzElTable* aTablePtr, WsfStringId aStateId, double aWavelengthLimit)
{
   bool ok = true;

   size_t stateIndex;
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
      Table table(aWavelengthLimit, aTablePtr);
      mStates.back().mTables.push_back(table);
   }
   else
   {
      // Creating a new wavelength limit within the current state.
      // Wavelengths must be monotonically ascending.
      if (aWavelengthLimit > mStates[stateIndex].mTables.back().mWavelengthLimit)
      {
         Table table(aWavelengthLimit, aTablePtr);
         mStates[stateIndex].mTables.push_back(table);
      }
      else
      {
         ok = false;
      }
   }
   return ok;
}

// =================================================================================================
//! Select a table for a given configuration state and signal wavelength.
//! @param aContext      [output] The updated table lookup context representing the selected table.
//! @param aStateId      [input] The string ID representing the signature state to be used.
//! @param aWavelength    [input] The wavelength of the signal.
void WsfStandardOpticalReflectivity::SharedData::SelectTable(UtAzElLookup& aContext, WsfStringId aStateId, double aWavelength)
{
   // Locate the state among the defined states.
   size_t stateIndex;
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

   // Within the state, search to locate the proper table.
   TableSet& tables = mStates[stateIndex].mTables;
   size_t    tableIndex;
   for (tableIndex = 0; tableIndex < tables.size(); ++tableIndex)
   {
      if (aWavelength < tables[tableIndex].mWavelengthLimit)
      {
         tables[tableIndex].mTablePtr->GetContext(aContext);
         return;
      }
   }

   // A signature with the appropriate wavelength limit did not exist.
   // Simply use the last signature for the last wavelength.
   if (!tables.empty())
   {
      tables.back().mTablePtr->GetContext(aContext);
      return;
   }

   // This should NEVER happen because InitializeType() has guaranteed that every state at least one table.
   ut::log::error() << "Signature not correctly initialized.";
}

// =================================================================================================
//! Constructor to create a table reference.
//! @param aWavelengthLimit The maximum wavelength to which this table applies.
//! @param aTablePtr        Pointer to the table data that defines the signature.
WsfStandardOpticalReflectivity::Table::Table(double aWavelengthLimit, UtAzElTable* aTablePtr)
   : mWavelengthLimit(aWavelengthLimit)
   , mTablePtr(aTablePtr)
{
}

// =================================================================================================
WsfStandardOpticalReflectivity::State::State(WsfStringId aStateId)
   : mStateId(aStateId)
{
}
