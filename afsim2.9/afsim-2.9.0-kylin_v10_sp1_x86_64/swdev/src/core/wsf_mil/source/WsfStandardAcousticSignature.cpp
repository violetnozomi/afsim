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

#include "WsfStandardAcousticSignature.hpp"

#include <cmath>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringIdLiteral.hpp"

// =================================================================================================
//! Factory method called by WsfAcousticSignatureTypes.
// static
WsfAcousticSignature* WsfStandardAcousticSignature::ObjectFactory(const std::string& aTypeName)
{
   WsfAcousticSignature* instancePtr = nullptr;
   if (aTypeName == "WSF_ACOUSTIC_SIGNATURE")
   {
      instancePtr = new WsfStandardAcousticSignature();
   }
   return instancePtr;
}

// =================================================================================================
//! Called by WsfAcousticSignatureTypes to create the default type to be used on platform if none provided.
// static
WsfAcousticSignature* WsfStandardAcousticSignature::CreateDefaultType()
{
   // Create a default acoustic signature of 100 dB-20uPa at 1 kHz
   WsfStandardAcousticSignature* defaultPtr = new WsfStandardAcousticSignature;
   std::vector<double>           freqVector;
   freqVector.push_back(1000.0);
   std::vector<double> sigVector;
   sigVector.push_back(100.0);
   defaultPtr->mSharedDataPtr->AddStateEntry(WsfStringId("default"), freqVector, sigVector);
   defaultPtr->InitializeType();
   return defaultPtr;
}

// =================================================================================================
WsfStandardAcousticSignature::WsfStandardAcousticSignature()
   : WsfAcousticSignature()
   , mSharedDataPtr(new SharedData())
{
}

// =================================================================================================
// virtual
WsfAcousticSignature* WsfStandardAcousticSignature::Clone() const
{
   return new WsfStandardAcousticSignature(*this);
}

// =================================================================================================
// virtual
bool WsfStandardAcousticSignature::InitializeType()
{
   bool ok = WsfAcousticSignature::InitializeType();
   ok &= mSharedDataPtr->InitializeType(*this);
   return ok;
}

// =================================================================================================
// virtual
std::vector<WsfStringId> WsfStandardAcousticSignature::GetStateNames() const
{
   const auto& states = GetStates();
   return GetStateNamesP<State, &State::mStateId>(states.begin(), states.end());
}

// =================================================================================================
// virtual
bool WsfStandardAcousticSignature::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (mSharedDataPtr->ProcessInput(aInput, *this))
   {
   }
   else
   {
      myCommand = WsfAcousticSignature::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
double WsfStandardAcousticSignature::GetNoisePressure(WsfStringId aStateId, double aFrequency)
{
   // Locate the state among the defined states.

   StateIndex stateIndex;
   for (stateIndex = 0; stateIndex < mSharedDataPtr->mStates.size(); ++stateIndex)
   {
      if (aStateId == mSharedDataPtr->mStates[stateIndex].mStateId)
      {
         break;
      }
   }

   // If the requested state was not found then use the default state.
   if (stateIndex >= mSharedDataPtr->mStates.size())
   {
      stateIndex = mSharedDataPtr->mDefaultStateIndex;
   }

   Table* tablePtr = &(mSharedDataPtr->mStates[stateIndex].mSigTable);
   tablePtr->mFreqLookup.Lookup(tablePtr->mFreqs, aFrequency);
   return TblEvaluate(tablePtr->mSigs, tablePtr->mFreqLookup);
}

std::pair<double, double> WsfStandardAcousticSignature::GetNoisePressureLimits(WsfStringId aStateId) const
{
   StateIndex stateIndex;
   for (stateIndex = 0; stateIndex < mSharedDataPtr->mStates.size(); ++stateIndex)
   {
      if (aStateId == mSharedDataPtr->mStates[stateIndex].mStateId)
      {
         break;
      }
   }

   // If the requested state was not found then use the default state.
   if (stateIndex >= mSharedDataPtr->mStates.size())
   {
      stateIndex = mSharedDataPtr->mDefaultStateIndex;
   }

   Table& table  = mSharedDataPtr->mStates[stateIndex].mSigTable;
   auto&  values = table.mSigs.GetValues();
   auto   minmax = std::minmax_element(values.begin(), values.end());
   return {*minmax.first, *minmax.second};
}

// =================================================================================================
//! @returns The signatures for all of defined states.
//! @note This method was created to allow applications access to
//! signature data. It should not be used to look up signatures.
const std::vector<WsfStandardAcousticSignature::State>& WsfStandardAcousticSignature::GetStates() const
{
   return mSharedDataPtr->mStates;
}

// =================================================================================================
WsfStandardAcousticSignature::SharedData::SharedData()
   : mCurrentStateId(UtStringIdLiteral("default"))
   , mCorrectionFactor(1.0)
   , mDefaultStateIndex(0)
   , mStates()
{
}

// =================================================================================================
WsfStandardAcousticSignature::SharedData::~SharedData()
{
   // TODO need to fill this in!!!
}

// =================================================================================================
bool WsfStandardAcousticSignature::SharedData::InitializeType(WsfObject& aBase)
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

   return ok;
}

// =================================================================================================
bool WsfStandardAcousticSignature::SharedData::IsA_ValidState(WsfStringId aId) const
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
bool WsfStandardAcousticSignature::SharedData::ProcessInput(UtInput& aInput, WsfObject& aBase)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "data_reference_range")
   {
      double aSlantRange;
      aInput.ReadValueOfType(aSlantRange, UtInput::cLENGTH);
      aInput.ValueGreater(aSlantRange, 0.0);

      // compute propagation correction factor
      mCorrectionFactor = pow(4.0 * UtMath::cPI * aSlantRange, 2.0);
   }
   else if (command == "state")
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
   }
   else if (command == "spectrum_data")
   {
      std::vector<double> freqs;
      std::vector<double> sigs;

      UtInputBlock block(aInput, "end_spectrum_data");
      while (block.ReadCommand())
      {
         command = block.GetCommand();

         if (command == "freq")
         {
            // read frequency
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cFREQUENCY);
            aInput.ValueGreater(value, 0.0);
            freqs.push_back(value);
         }
         else if (command == "noise_pressure")
         {
            // read noise value
            double value = 0.0;
            aInput.ReadValueOfType(value, UtInput::cNOISE_PRESSURE);
            aInput.ValueGreater(value, 0.0);
            value *= mCorrectionFactor;
            sigs.push_back(value);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Error reading acoustic signature.");
         }
      }

      // check data
      if (!freqs.empty())
      {
         if (freqs.size() != sigs.size())
         {
            throw UtInput::BadValue(aInput, "Number of freqs and noise pressure entries must match.");
         }

         if (!AddStateEntry(mCurrentStateId, freqs, sigs))
         {
            throw UtInput::BadValue(aInput, "Cannot add table to acoustic signature.");
         }
         freqs.clear();
         sigs.clear();
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Add a new table to the acoustic signature.
//!
//! @param aStateId    String ID of the state name for which the table applies.
//! @param aFreqVector Vector of freqs for sig
//! @param aSigVector  Vector of noise pressures
//! @returns true if table added or false if a table for the requested state has already been defined.
bool WsfStandardAcousticSignature::SharedData::AddStateEntry(WsfStringId          aStateId,
                                                             std::vector<double>& aFreqVector,
                                                             std::vector<double>& aSigVector)
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

   if ((stateIndex >= mStates.size()) || (mStates[stateIndex].mSigTable.mFreqs.GetSize() < 2))
   {
      // Creating a new state or overriding default state
      Table aTable;
      aTable.mFreqs.SetValues(aFreqVector);
      aTable.mSigs.Resize(static_cast<int>(aFreqVector.size()));
      for (unsigned int i = 0; i < aFreqVector.size(); ++i)
      {
         aTable.mSigs.Set(aSigVector[i], i);
      }

      mStates.emplace_back(aStateId, aTable);
   }
   else
   {
      ok = false;
   }

   return ok;
}

// =================================================================================================
WsfStandardAcousticSignature::State::State(WsfStringId aStateId, Table& aTable)
   : mStateId(aStateId)
   , mSigTable(aTable)
{
}
