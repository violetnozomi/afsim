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
#ifndef WSFMASKINGPATTERN_HPP
#define WSFMASKINGPATTERN_HPP

#include "wsf_export.h"

#include <vector>

class UtAzElLookup;
class UtAzElTable;
class WsfArticulatedPart;
#include "WsfObject.hpp"

//! Model blockage that might occur when looking at a specified direction from an articulated part.
//!
//! This object can be optionally associated with an articulated part, and provides a 'masking
//! factor' that is a function of viewing angle. The factor is a number in the range [0..1]
//! that represents fraction of the original 'signal' that is passed.
//!
//! This is typically used to represent the blockage of a signal by the structure of the platform
//! or articulated part. A value of 1 indicates no blockage while 0 indicates complete blockage.
//! Intermediate values might be used to represent the viewing-angle dependent factor when looking
//! through a window.

class WSF_EXPORT WsfMaskingPattern : public WsfUncloneableObject
{
public:
   WsfMaskingPattern();
   ~WsfMaskingPattern() override;

   virtual bool Initialize();

   bool ProcessInput(UtInput& aInput) override;

   double GetFactor(WsfStringId         aStateId,
                    WsfArticulatedPart* aPartPtr,
                    double              aViewAzPCS,
                    double              aViewElPCS,
                    const double        aViewVecWCS[3]);

   class State
   {
   public:
      State(WsfStringId aStateId)
         : mECS_TablePtr(nullptr)
         , mPCS_TablePtr(nullptr)
         , mStateId(aStateId)
      {
      }

      //! The ECS (platform-relative) effects table.
      UtAzElTable* mECS_TablePtr;
      //! The PCS (platform part-relative) effects table.
      UtAzElTable* mPCS_TablePtr;
      //! The state to which this table corresponds.
      WsfStringId mStateId;
   };

private:
   using StateIndex = std::vector<State>::size_type;

   StateIndex         mDefaultStateIndex;
   std::vector<State> mStates;
};

#endif
