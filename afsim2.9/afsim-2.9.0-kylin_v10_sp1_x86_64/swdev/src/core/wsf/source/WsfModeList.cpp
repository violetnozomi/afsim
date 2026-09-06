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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company
// ****************************************************************************

#include "WsfModeList.hpp"

#include <cassert>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfMode.hpp"

// NOTE: The mode pointed to by mTemplatePtr serves two purposes. It is the 'template' from
// which modes are created when using the 'mode' command (this is called 'explicit' mode).
// In this case, the template may be configured using the 'mode_template/end_mode_template'
// block. In the second case the template represents the definition of the one and only
// 'implicit' mode.  Valid 'mode'-related commands that aren't inside a 'mode/end_mode' or
// 'mode_template/end_mode_template' block are assumed to define the one and only mode. This
// makes it a little easier on the user in that they don't have to explicitly create a mode
// for a single mode system.

// =================================================================================================
//! Constructor.
//! @param aTemplatePtr A pointer to the 'template' from which new modes will be created
//! by ProcessInput(). The mode list assumes ownership of the 'template' and will delete
//! it when the mode list is destroyed.
WsfModeList::WsfModeList(WsfMode* aTemplatePtr)
   : mModeList()
   , mTemplatePtr(aTemplatePtr)
   , mExplicitModeUsed(false)
   , mImplicitModeUsed(false)
   , mCanMultiSelect(false)
   , mInitialModeNameId(nullptr)
   , mIsInitialized(false)
   , mIsTurnedOn(false)
   , mCurrentModeIndex(0)
   , mModeSelectDelay(0.0)
   , mModeChangeScheduled(false)
{
   assert(mTemplatePtr != nullptr);
   mTemplatePtr->SetName(UtStringIdLiteral("default"));
}

// =================================================================================================
//! Copy constructor.
WsfModeList::WsfModeList(const WsfModeList& aSrc)
   : mModeList()
   , mTemplatePtr(nullptr)
   , mExplicitModeUsed(aSrc.mExplicitModeUsed)
   , mImplicitModeUsed(aSrc.mImplicitModeUsed)
   , mCanMultiSelect(aSrc.mCanMultiSelect)
   , mInitialModeNameId(aSrc.mInitialModeNameId)
   , mIsInitialized(false)
   , mIsTurnedOn(false)
   , mCurrentModeIndex(0)
   , mModeSelectDelay(aSrc.mModeSelectDelay)
   , mModeChangeScheduled(aSrc.mModeChangeScheduled)
{
   // Clone the template/implicit mode.
   if (aSrc.mTemplatePtr != nullptr)
   {
      mTemplatePtr = (aSrc.mTemplatePtr)->Clone();
   }

   // Clone the explicit modes.
   if (mExplicitModeUsed)
   {
      for (auto srcMode : aSrc.mModeList)
      {
         WsfMode* modePtr    = srcMode->Clone();
         modePtr->mModeIndex = mModeList.size();
         mModeList.push_back(modePtr);
      }
   }
}

// =================================================================================================
WsfModeList::~WsfModeList()
{
   // Delete the template/implicit mode.
   delete mTemplatePtr;
   // Delete any explicit modes.
   if (mExplicitModeUsed)
   {
      for (auto& ml : mModeList)
      {
         delete ml;
      }
   }
}

// =================================================================================================
//! Return the string id of the 'default' mode name.
// static
WsfStringId WsfModeList::GetDefaultModeNameId()
{
   return UtStringIdLiteral("default");
}

// =================================================================================================
//! Add a mode to the mode list.
//! @param aModePtr Pointer to the mode object to be added. The mode list will assume
//! ownership of the object and will delete it when the mode list is deleted.
void WsfModeList::AddMode(WsfMode* aModePtr)
{
   aModePtr->mModeIndex = mModeList.size();
   mModeList.push_back(aModePtr);
}

// =================================================================================================
//! Return the mode index of the mode with the specified name (ID).
//! @returns If the requested mode is found the return value will be the mode index
//! in the range [0, GetModeCount() - 1]. If the mode was not found then the value of
//! GetModeCount() will be returned.
//! @note This method is not valid until Initialize() has been called.
size_t WsfModeList::GetModeByName(WsfStringId aModeNameId) const
{
   for (size_t i = 0; i < mModeList.size(); ++i)
   {
      if (aModeNameId == mModeList[i]->GetNameId())
      {
         return i;
      }
   }
   return GetModeCount();
}

// =================================================================================================
//! Return the string ID of the name associated with the mode with the specified index.
//! @param aModeIndex The index of the mode whose name is being requested. This must be
//! in the range [0, GetModeCount() - 1].
//! @returns The string ID of the name of the mode or 0 if the mode index was invalid.
//! @note This method is not valid until Initialize() has been called.
WsfStringId WsfModeList::GetModeNameId(size_t aModeIndex) const
{
   if (aModeIndex < mModeList.size())
   {
      return mModeList[aModeIndex]->GetNameId();
   }
   return nullptr; // Unknown mode
}

// =================================================================================================
//! Initialize the mode list.
//! This method ensures that the mode list is properly formed and prepares it for use.
//! @returns 'true' if successful or 'false' if not.
//! @note This does NOT initialize each of the modes. It is the responsibility of the
//! caller to initialize the individual modes. This tends to be very specific to the
//! application and could not be done conveniently through a common interface (it could
//! have been done, but it was simpler just to let the caller write their own loop).
bool WsfModeList::Initialize()
{
   if (mIsInitialized)
   {
      return false;
   }

   bool ok = true;
   if (mExplicitModeUsed)
   {
      if (mModeList.empty())
      {
         ut::log::error() << "No modes have been defined.";
         ok = false;
      }
      else if (!mInitialModeNameId.IsNull())
      {
         mCurrentModeIndex = GetModeByName(mInitialModeNameId);
         if (mCurrentModeIndex >= mModeList.size())
         {
            auto out = ut::log::error() << "Undefined initial_mode.";
            out.AddNote() << "Mode: " << mInitialModeNameId;
            ok = false;
         }
      }
   }
   else
   {
      // Implicit mode (use the template as the one and only mode).
      mImplicitModeUsed = true;
      mModeList.clear();
      AddMode(mTemplatePtr);
   }
   mIsInitialized = true;
   return ok;
}

// =================================================================================================
//! The standard ProcessInput method.
bool WsfModeList::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "mode")
   {
      if (mImplicitModeUsed)
      {
         throw UtInput::BadValue(aInput, "'mode' cannot be used in this context");
      }
      mExplicitModeUsed = true;

      UtInputBlock inputBlock(aInput);
      std::string  modeName;
      aInput.ReadValue(modeName);
      WsfMode* modePtr   = nullptr;
      size_t   modeIndex = GetModeByName(WsfStringId(modeName));
      if (modeIndex < GetModeCount())
      {
         // Updating an existing mode.
         modePtr = mModeList[modeIndex];
      }
      else if (mIsInitialized)
      {
         // Cannot add a mode after the mode list has been initialized. (Most sensors make a type-specific
         // copy of the mode list after initialization, and it cannot change size!)
         throw UtInput::BadValue(aInput,
                                 "Mode '" + modeName + "' does not exist and cannot be created after initialization");
      }
      else
      {
         // Creating a new mode.
         modePtr = mTemplatePtr->Clone();
         modePtr->SetName(modeName);
         AddMode(modePtr);
      }
      while (inputBlock.ReadCommand())
      {
         if (!modePtr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "mode_template")
   {
      if (mImplicitModeUsed)
      {
         throw UtInput::BadValue(aInput, "'mode_template' cannot be used with an implicitly defined 'mode'");
      }
      else if (mExplicitModeUsed && (!mModeList.empty()))
      {
         throw UtInput::BadValue(aInput, "'mode_template' cannot be used after a 'mode' has been defined.");
      }
      mExplicitModeUsed = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         if (!mTemplatePtr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   else if (command == "initial_mode")
   {
      std::string initialMode;
      aInput.ReadValue(initialMode);
      mInitialModeNameId.SetString(initialMode);
   }
   else if (command == "selection_mode")
   {
      std::string selectionMode;
      aInput.ReadValue(selectionMode);
      if (selectionMode == "single")
      {
         mCanMultiSelect = false;
      }
      else if (selectionMode == "multiple")
      {
         mCanMultiSelect = true;
      }
      else
      {
         throw UtInput::BadValue(aInput, "'selection_mode' must be 'single' or 'multiple'");
      }
   }
   else if (command == "mode_select_delay")
   {
      aInput.ReadValueOfType(mModeSelectDelay, UtInput::cTIME);
   }
   else if (mTemplatePtr->ProcessInput(aInput))
   {
      if (mExplicitModeUsed)
      {
         throw UtInput::BadValue(aInput, "'" + command + "' cannot be used in this context");
      }
      mImplicitModeUsed = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Deselect the mode with the specified mode name.
//! @param aModeNameId The string ID of the mode name that is to be deselected.
//! @param aSimTime The current simulation time.
//!
//! @note This method should not be used for single-select mode lists as they require one
//! mode to be selected at all times. Select is the only method that should be used on
//! Single-select lists.
//!
//! @note This does not alter the index that is returned by GetCurrentMode().
// virtual
void WsfModeList::Deselect(double aSimTime, WsfStringId aModeNameId)
{
   size_t modeIndex = GetModeByName(aModeNameId);
   if (modeIndex < mModeList.size())
   {
      if (mIsTurnedOn)
      {
         DeselectP(aSimTime, modeIndex);
      }
      else
      {
         mModeList[modeIndex]->mIsSelected = false;
      }
   }
   else
   {
      auto out = ut::log::error() << "Attempting to deselect non-existent mode.";
      out.AddNote() << "Mode: " << aModeNameId;
   }
}

// =================================================================================================
//! Select the mode with the specified mode name (ID).
//! @param aModeNameId The string ID of the mode name that is to be selected.
//! @param aSimTime The current simulation time.
//!
//! @note The index selected mode becomes the value that would be returned by GetCurrentMode().
//!
//! @note The current implementation only allows the selection of EXACTLY one mode at
//! any given point in time (while the system is on). The currently selected mode is
//! deselected before selecting the new mode.
void WsfModeList::Select(double aSimTime, WsfStringId aModeNameId)
{
   size_t modeIndex = GetModeByName(aModeNameId);
   if (modeIndex < mModeList.size()) // The requested mode name is valid
   {
      if (mIsTurnedOn)
      {
         if (mCanMultiSelect)
         {
            SelectP(aSimTime, modeIndex); // Select the mode and don't affect any other.
         }
         else if (modeIndex != mCurrentModeIndex) // Single select and new mode is not the current mode
         {
            DeselectP(aSimTime, mCurrentModeIndex); // Deselect the currently selected mode ...
            SelectP(aSimTime, modeIndex);           // ... and select the requested mode
         }
      }
      else if (mCanMultiSelect)
      {
         mModeList[modeIndex]->mIsSelected = true;
      }
      mCurrentModeIndex = modeIndex;
   }
   else
   {
      auto out = ut::log::error() << "Attempting to select non-existent mode.";
      out.AddNote() << "Mode: " << aModeNameId;
   }
}

// =================================================================================================
//! The system is being turned off.
//! This method invokes the 'Deselect' method for every selected mode.
//! @param aSimTime The current simulation time.
// virtual
void WsfModeList::TurnOff(double aSimTime)
{
   if (mIsTurnedOn)
   {
      // Deselect the selected mode(s).
      for (size_t modeIndex = 0; modeIndex < mModeList.size(); ++modeIndex)
      {
         if (mModeList[modeIndex]->IsSelected())
         {
            DeselectP(aSimTime, modeIndex);
         }
      }
      if (mCanMultiSelect)
      {
         // If 'initial_mode' is specified, it becomes the 'current mode' while the system is off
         // and will be selected when the system is turned back on.  If a different mode is selected
         // while the system is off (e.g. via script), the mCurrentModeIndex will be updated accordingly.
         size_t initialModeIndex = GetModeByName(mInitialModeNameId);
         if (initialModeIndex < GetModeCount())
         {
            mCurrentModeIndex = initialModeIndex;
         }
      }
      mIsTurnedOn = false;
   }
}

// =================================================================================================
//! The system is being turned on.
//! This method invokes the 'Select' method for every selected mode.
//! @param aSimTime The current simulation time.
// virtual
void WsfModeList::TurnOn(double aSimTime)
{
   if (!mIsTurnedOn)
   {
      if (mCanMultiSelect)
      {
         // Select any modes that have been explicitly selected while turned off.
         for (size_t modeIndex = 0; modeIndex < mModeList.size(); ++modeIndex)
         {
            if (mModeList[modeIndex]->mIsSelected)
            {
               mModeList[modeIndex]->mIsSelected = false; // Must be unselected so SelectP will select it.
               SelectP(aSimTime, modeIndex);
            }
         }
         // Also select any explicit 'initial_mode' if one was specified.
         size_t initialModeIndex = GetModeByName(mInitialModeNameId);
         if (initialModeIndex < GetModeCount())
         {
            if (!mModeList[initialModeIndex]->mIsSelected)
            {
               SelectP(aSimTime, initialModeIndex);
            }
            // At this point, all selected modes have been identified.  If the 'current mode'
            // is not selected (e.g. a different mode was selected then deselected via script
            // while the system was off), then use initial_mode as current.
            if (!mModeList[mCurrentModeIndex]->mIsSelected)
            {
               mCurrentModeIndex = initialModeIndex;
            }
         }
      }
      else
      {
         // If single-select mode then select the current mode.
         SelectP(aSimTime, mCurrentModeIndex);
      }
      mIsTurnedOn = true;
   }
}

// =================================================================================================
//! Deselect a mode.
//! @param aSimTime   The current simulation time.
//! @param aModeIndex The index of the mode to be deselected.
// protected
void WsfModeList::DeselectP(double aSimTime, size_t aModeIndex)
{
   if (aModeIndex < mModeList.size())
   {
      WsfMode* modePtr = mModeList[aModeIndex];
      if (modePtr->IsSelected())
      {
         // The selected/deselected state is changed prior to calling the mode to actually perform the
         // action. Some mode implementations call other routines which look at the entire mode list and
         // they don't recognize the new state of the mode.
         modePtr->mIsSelected = false;
         modePtr->Deselect(aSimTime);
      }
   }
}

// =================================================================================================
//! Select a mode.
//! @param aSimTime   The current simulation time.
//! @param aModeIndex The index of the mode to be selected.
// protected
void WsfModeList::SelectP(double aSimTime, size_t aModeIndex)
{
   if (aModeIndex < mModeList.size())
   {
      WsfMode* modePtr = mModeList[aModeIndex];
      if (!modePtr->IsSelected())
      {
         // The selected/deselected state is changed prior to calling the mode to actually perform the
         // action. Some mode implementations call other routines which look at the entire mode list and
         // they don't recognize the new state of the mode.
         modePtr->mIsSelected = true;
         modePtr->Select(aSimTime);
      }
   }
}
