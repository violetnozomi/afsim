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
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFMOVERMODE_HPP
#define WSFMOVERMODE_HPP

#include "wsf_export.h"

#include "WsfMode.hpp"
#include "WsfMover.hpp"
#include "WsfStringId.hpp"

//! A specialization of WsfMode for movers.
//! A mover mode represents a particular set of motion characteristics for
//! a mover. For instance, a mover may have climb, cruise, descent modes,
//! each with particular speed and rate-of-climb constraints.
class WSF_EXPORT WsfMoverMode : public WsfMode
{
public:
   WsfMoverMode();
   WsfMoverMode(const WsfMoverMode& aSrc);
   WsfMoverMode& operator   =(const WsfMoverMode& aRhs);
   ~WsfMoverMode() override = default;

   //! Return the pointer to the platform to which the associated sensor is attached.
   WsfPlatform* GetPlatform() const { return mPlatformPtr; }

   //! Return the pointer to the Mover with which this mode is associated.
   WsfMover* GetMover() const { return mMoverPtr; }

   //! Create an exact duplicate of this mode.
   WsfMode* Clone() const override = 0;

   //! Initialize the Mover Mode.
   virtual bool Initialize(double aSimTime, WsfMover* aMoverPtr);

   bool ProcessInput(UtInput& aInput) override;

   //! The mode is being deselected.
   //! The specific implementation should perform whatever actions are associated with de-selecting the mode
   //! (e.g.: deactivating transmitters or receivers).
   //! @param aSimTime   [input] The current simulation time.
   void Deselect(double aSimTime) override = 0;

   //! The mode is being activated.
   //! The specific implementation should perform whatever actions are associated with selecting the mode
   //! (e.g.: activating transmitters or receivers).
   //! @param aSimTime   [input] The current simulation time.
   void Select(double aSimTime) override = 0;

protected:
   //! Pointer to the Mover with which the mode is associated.
   WsfMover* mMoverPtr;
   //! Pointer to the platform with which the sensor is associated.
   WsfPlatform* mPlatformPtr;
};

#endif
