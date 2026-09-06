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

#include "WsfMaskingPattern.hpp"

#include "UtAzElLookup.hpp"
#include "UtAzElTable.hpp"
#include "UtAzElTableLoader.hpp"
#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfArticulatedPart.hpp"
#include "WsfPlatform.hpp"
#include "WsfStringId.hpp"

// =================================================================================================
WsfMaskingPattern::WsfMaskingPattern()
   : WsfUncloneableObject()
   , mDefaultStateIndex(0)
{
}

// =================================================================================================
// virtual
WsfMaskingPattern::~WsfMaskingPattern()
{
   for (StateIndex stateIndex = 0; stateIndex < mStates.size(); ++stateIndex)
   {
      delete mStates[stateIndex].mECS_TablePtr;
      delete mStates[stateIndex].mPCS_TablePtr;
   }
}

// =================================================================================================
//! Initialize the masking pattern.
//!
//! @returns 'true' if successful or 'false' if the required 'default' data is not present.
// virtual
bool WsfMaskingPattern::Initialize()
{
   bool ok = true;

   // Make sure the 'default' state exists.

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
      ut::log::warning() << "The state 'default' does not exist.";
   }
   return ok;
}

// =================================================================================================
// virtual
bool WsfMaskingPattern::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "state")
   {
      std::string stateName;
      aInput.ReadValue(stateName);
      WsfStringId stateId = stateName;

      // Make sure the new state has not been used
      for (StateIndex i = 0; i < mStates.size(); ++i)
      {
         if (stateId == mStates[i].mStateId)
         {
            throw UtInput::BadValue(aInput, "Duplicate state: " + stateName);
         }
      }
      mStates.emplace_back(stateId);
   }
   else if ((command == "platform_factor") || (command == "part_factor"))
   {
      bool         isPlatformFactor = (command == "platform_factor");
      UtAzElTable* tablePtr         = nullptr;
      std::string  tableUnits("dummy"); // no units
      // The table loader looks at the current command to decide the type of table to load...
      // ... so read the command (otherwise we are just pointing at '[platform|part]_factor')
      aInput.ReadCommand(command);
      myCommand = UtAzElTableLoader::ProcessTable(aInput, tablePtr, tableUnits);
      if ((!myCommand) || (tablePtr == nullptr))
      {
         throw UtInput::BadValue(aInput);
      }

      // If this is an entry with no preceding 'state' then it is for the 'default' state.
      if (mStates.empty())
      {
         mStates.emplace_back("default");
      }

      // Stash the pointer to the table...
      if (isPlatformFactor)
      {
         delete mStates.back().mECS_TablePtr;
         mStates.back().mECS_TablePtr = tablePtr;
      }
      else
      {
         delete mStates.back().mPCS_TablePtr;
         mStates.back().mPCS_TablePtr = tablePtr;
      }
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Compute the masking factor.
//! @param aStateId The current state.
//! @param aPartPtr The host articulated part.
//! @param aViewAzPCS The azimuth   viewing angle in the part coordinate system.
//! @param aViewElPCS The elevation viewing angle in the part coordinate system.
//! @param aViewVecWCS The unit viewing vector in the world coordinate system. This is used if a
//! @returns The masking factor in the range [0..1]. A value of 1 indicates the view is unobstructed
//! in the indicated direction while a value of 0 indicates total obstruction.
double WsfMaskingPattern::GetFactor(WsfStringId         aStateId,
                                    WsfArticulatedPart* aPartPtr,
                                    double              aViewAzPCS,
                                    double              aViewElPCS,
                                    const double        aViewVecWCS[3])
{
   StateIndex stateIndex = mDefaultStateIndex;
   for (StateIndex i = 0; i < mStates.size(); ++i)
   {
      if (aStateId == mStates[i].mStateId)
      {
         stateIndex = i;
         break;
      }
   }

   double       factor = 1.0;
   UtAzElLookup context;
   UtAzElTable* tablePtr = mStates[stateIndex].mECS_TablePtr;
   if (tablePtr != nullptr)
   {
      double az = 0.0;
      double el = 0.0;
      aPartPtr->GetPlatform()->ComputeAspect(aViewVecWCS, az, el);
      tablePtr->GetContext(context);
      factor *= context.Lookup(az, el);
   }

   if (factor > 0.0)
   {
      tablePtr = mStates[stateIndex].mPCS_TablePtr;
      if (tablePtr != nullptr)
      {
         tablePtr->GetContext(context);
         factor *= context.Lookup(aViewAzPCS, aViewElPCS);
      }
   }

   return factor;
}
