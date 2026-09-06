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

#ifndef WSFMODE_HPP
#define WSFMODE_HPP

#include "wsf_export.h"

class UtInput;
class WsfModeList;
#include "WsfNamed.hpp"

//! An abstract base class that represents the operating mode of a platform subsystem.
//!
//! Many platform subsystems (sensors, interferers, radios) have a concept of 'modes'.
//! A mode would be specific set of subsystem parameters (frequency, PRF, power, etc.)
//! that would typically be selected by the 'operator' using some sort of switch.
//!
//! A derived class implements the actions that are associated with selecting and
//! de-selecting the mode.
//!
//! WsfModeList is used to maintain a collection of modes.
class WSF_EXPORT WsfMode : public WsfNamed
{
   friend class WsfModeList;

public:
   WsfMode();
   ~WsfMode() override = default;

   virtual WsfMode* Clone() const = 0;

   virtual bool ProcessInput(UtInput& aInput);

   //! Perform any actions necessary to select this mode.
   //! @param aSimTime The current simulation time.
   //! @note This called ONLY by WsfModeList. Any other use is undefined.
   virtual void Select(double aSimTime) {}

   //! Perform any actions necessary to de-select this mode.
   //! @param aSimTime The current simulation time.
   //! @note This called ONLY by WsfModeList. Any other use is undefined.
   virtual void Deselect(double aSimTime) {}

   //! Return the index of this mode within its associated mode list.
   size_t GetModeIndex() const { return mModeIndex; }

   //! Returns 'true' if the mode is selected.
   bool IsSelected() const { return mIsSelected; }

private:
   //! The index of the mode within its associated mode list.
   size_t mModeIndex;

   //! 'true' if selected.
   bool mIsSelected;
};

#endif
