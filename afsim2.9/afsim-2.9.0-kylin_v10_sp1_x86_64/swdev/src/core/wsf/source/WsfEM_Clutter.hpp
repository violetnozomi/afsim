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

#ifndef WSFEM_CLUTTER_HPP
#define WSFEM_CLUTTER_HPP

#include "wsf_export.h"

class UtInput;
class WsfEM_Interaction;
class WsfEM_Rcvr;
class WsfEnvironment;
#include "WsfObject.hpp"

//! An abstract class for computing clutter power.
class WSF_EXPORT WsfEM_Clutter : public WsfObject
{
public:
   WsfEM_Clutter();
   ~WsfEM_Clutter() override = default;

   WsfEM_Clutter* Clone() const override = 0;

   virtual bool Initialize(WsfEM_Rcvr* aRcvrPtr);

   bool ProcessInput(UtInput& aInput) override;

   //! Is 'debug' enabled?
   bool DebugEnabled() const { return mDebugEnabled; }

   //! Compute the clutter power for an interaction.
   //! @param aInteraction      The current interaction.
   //! @param aEnvironment      The environment (sea state, land formation, land coverage).
   //! @param aProcessingFactor The factor to be applied to the raw computed clutter power to produce
   //! the clutter power as seen by the radar AFTER is has performed clutter suppression. This value
   //! (in the range [0..1] represents the ability for the radar to attenuate (suppress) clutter returns.
   //! A value of zero indicates the clutter signal is completely attenuated while a value of one indicates
   //! that none of the clutter signal could be suppressed.
   //!
   //! For the non-table models, the computed clutter power will be multiplied by this value to
   //! produce the return value. For table models the factor has probably already been included
   //! in the table and the value will probably be ignored.
   //!
   //! @return The clutter power (watts).
   virtual double ComputeClutterPower(WsfEM_Interaction& aInteraction,
                                      WsfEnvironment&    aEnvironment,
                                      double             aProcessingFactor) = 0;

   //! Is the object a 'null' (no-effect) clutter model?
   //! No operational model needs to worry about this method.
   //! @note See the code in WsfEM_ClutterTypes as to why this is present.
   virtual bool IsNullModel() const { return false; }

protected:
   WsfEM_Clutter(const WsfEM_Clutter& aSrc);
   WsfEM_Clutter& operator=(const WsfEM_Clutter& aRhs);

private:
   // static WsfEM_Clutter* CreateInstance(const std::string& aTypeName);

   bool mDebugEnabled;
};

#endif
