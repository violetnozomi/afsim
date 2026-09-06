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

#ifndef WSFMODELIST_HPP
#define WSFMODELIST_HPP

#include "wsf_export.h"

#include <cassert>
#include <vector>

class UtInput;
#include "WsfStringId.hpp"
class WsfMode;

//! A mode list is used to represent the various operating modes of platform
//! subsystems (sensors, interferers, etc). A mode (base class WsfMode) is used to
//! represent a single mode.
//!
//! This class provides the interface to manipulate multi-mode systems.

class WSF_EXPORT WsfModeList
{
public:
   using ModeList = std::vector<WsfMode*>;

   WsfModeList(WsfMode* aTemplatePtr);
   WsfModeList(const WsfModeList& aSrc);
   WsfModeList& operator=(const WsfModeList&) = delete;
   virtual ~WsfModeList();

   static WsfStringId GetDefaultModeNameId();

   virtual void AddMode(WsfMode* aModePtr);

   //! Can multiple modes can be selected simultaneously?
   //! @returns true if multiple modes can be selected simultaneously or 'false' if
   //! only one mode can be selected at a time.
   bool CanMultiSelect() const { return mCanMultiSelect; }

   //! Return the index of the current mode.
   //! @note This method returns the index of the last mode selected by Select().
   //! Calling Deselect() does not alter this value even though the mode is not selected.
   //! @note This method is not valid until Initialize() has been called.
   size_t GetCurrentMode() const { return mCurrentModeIndex; }

   //! Get the number of modes that are defined.
   //! @note This method is not valid until Initialize() has been called.
   size_t GetModeCount() const { return mModeList.size(); }

   //! Return the pointer to mode with the specified index.
   //! @param aModeIndex The index of the desired mode in the range [0, GetModeCount() - 1].
   //! The results are undefined if the index is outside the allowable range.
   //! @returns The pointer to the desired mode.
   //! @note This method is not valid until Initialize() has been called.
   WsfMode* GetModeEntry(size_t aModeIndex) const { return mModeList[aModeIndex]; }

   size_t GetModeByName(WsfStringId aModeNameId) const;

   WsfStringId GetModeNameId(size_t aModeIndex) const;

   //! Returns the mode that is the 'mode_template' or the implicit mode.
   WsfMode* GetModeTemplate() const { return mTemplatePtr; }

   virtual bool Initialize();

   virtual bool ProcessInput(UtInput& aInput);

   virtual void Deselect(double aSimTime, WsfStringId aModeNameId);

   virtual void Select(double aSimTime, WsfStringId aModeNameId);

   virtual void TurnOff(double aSimTime);

   virtual void TurnOn(double aSimTime);

   //! Returns the mode select delay time in seconds.
   //! @return The mode select delay time (sec).
   virtual double GetModeSelectDelay() { return mModeSelectDelay; }

   //! Check is a mode change is scheduled.
   //! @return Returns 'true' if a mode change is currently scheduled, returns 'false' otherwise.
   virtual bool IsModeChangeScheduled() const { return mModeChangeScheduled; }

   //! Check is a mode change is scheduled.
   //! @return Returns 'true' if a mode change is currently scheduled, returns 'false' otherwise.
   virtual void SetModeChangeScheduled(bool aModeChangeScheduled) { mModeChangeScheduled = aModeChangeScheduled; }

   //! A helper template class to get a vector of mode pointers of the proper type.
   //! Classes that implement modes typically need pointers to their specific type
   //! of mode in order to call methods that aren't defined in the WsfMode base class.
   //! This method creates a copy of the mode list but the pointers are of the
   //! requested type. This allows the to eliminate a lot of dynamic casting.
   //!
   //! \note This list that is produced is just a copy. The source list still owns
   //! the modes and is responsible for their deletion.
   template<class T>
   void GetDerivedModeList(std::vector<T>& aModeList) const
   {
      aModeList.resize(mModeList.size());
      for (typename std::vector<WsfMode*>::size_type i = 0; i < mModeList.size(); ++i)
      {
         T modePtr = dynamic_cast<T>(mModeList[i]);
         assert(modePtr != nullptr);
         aModeList[i] = modePtr;
      }
   }

protected:
   void DeselectP(double aSimTime, size_t aModeIndex);
   void SelectP(double aSimTime, size_t aModeIndex);

   //! This list of modes.
   std::vector<WsfMode*> mModeList;
   //! The mode that is the 'mode_template' or the implicit mode.
   WsfMode* mTemplatePtr;
   //! 'true' if 'mode' was or 'mode_template' has been used.
   bool mExplicitModeUsed;
   //! 'true' if a mode was implicitly defined without using 'mode' or 'mode_template'.
   bool mImplicitModeUsed;
   //! 'true' if multiple modes can be simultaneously selected. If 'false' then only one mode
   //! can be selected at a time and selecting one mode will deselect the currently selected mode.
   bool mCanMultiSelect;
   //! The string id of the mode to be selected when the system is FIRST turned on.
   //! In the current implementation a subsequent turn-on will use the last selected mode.
   WsfStringId mInitialModeNameId;
   //! 'true' if the system has been initialized
   bool mIsInitialized;
   //! 'true' if the system is currently turned on.
   bool mIsTurnedOn;
   //! The vector index of the currently selected mode.
   size_t mCurrentModeIndex;
   //! The time in which a mode change is delayed before it is actually change upon the request.
   //! @note Currently only used by extension class and is not integrated locally.
   double mModeSelectDelay;
   //! The last successful mode selection time.
   bool mModeChangeScheduled;
};

#endif
