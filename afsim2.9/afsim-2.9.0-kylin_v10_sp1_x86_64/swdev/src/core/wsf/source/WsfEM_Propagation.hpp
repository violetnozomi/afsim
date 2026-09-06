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

#ifndef WSFEM_PROPAGATION_HPP
#define WSFEM_PROPAGATION_HPP

#include "wsf_export.h"

class UtInput;
class WsfEM_Interaction;
class WsfEM_Xmtr;
class WsfEM_XmtrRcvr;
class WsfEnvironment;
#include "WsfObject.hpp"

//! An abstract base class for computing pattern propagation factors.
class WSF_EXPORT WsfEM_Propagation : public WsfObject
{
public:
   WsfEM_Propagation();
   ~WsfEM_Propagation() override = default;

   WsfEM_Propagation* Clone() const override = 0;

   const char* GetScriptClassName() const override { return "WsfEM_Propagation"; }

   //! Is 'debug' enabled?
   bool DebugEnabled() const { return mDebugEnabled; }

   //! Compute the propagation factor for the current interaction.
   //! @param aInteraction The current interaction.
   //! @param aEnvironment The environment (sea state, land formation, land coverage).
   //! @return The pattern propagation factor (F40) as a linear value (not dB).
   virtual double ComputePropagationFactor(WsfEM_Interaction& aInteraction, WsfEnvironment& aEnvironment) = 0;

   virtual bool Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr);

   virtual bool Initialize(WsfEM_Xmtr* aXmtrPtr);

   bool ProcessInput(UtInput& aInput) override;

   //! Is the object a 'null' (no-effect) propagation model?
   //! No operational model needs to worry about this method.
   //! @note See the code in WsfEM_PropagationTypes as to why this is present.
   virtual bool IsNullModel() const { return false; }


protected:
   WsfEM_Propagation(const WsfEM_Propagation& aSrc);

   bool mDebugEnabled;

private:
   //! Assignment operator declared but not defined to prevent use.
   WsfEM_Propagation& operator=(const WsfEM_Propagation& aRhs) = delete;
};

#endif
