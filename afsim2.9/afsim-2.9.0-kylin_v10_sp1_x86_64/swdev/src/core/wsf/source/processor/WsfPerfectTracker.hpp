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

#ifndef WSFPERFECTTRACKER_HPP
#define WSFPERFECTTRACKER_HPP

#include "wsf_export.h"

class UtInput;
#include "WsfProcessor.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackId.hpp"

/**
   A utility class to facilitate the tracking of a particular platform,
   when the simulation can safely ignore sensor and emitter/emission
   details.  Provided that this processor is turned on, every Update()
   will produce a perfect "truth" track update on the specified target
   (provided it exists), and set it as the current target for the
   platform to which it belongs (see WsfPlatform::SetCurrentTarget()).
   The platform of interest can be changed at any time using
   SetTargetByNameId(), SetTargetByPlatformId(). or ClearTrackedTarget().
   A virtual method (DegradeTrack()) is provided to allow derived
   classes to degrade the track in some chosen way, to model
   uncertainties or errors.
*/

class WSF_EXPORT WsfPerfectTracker : public WsfProcessor
{
public:
   WsfPerfectTracker(WsfScenario& aScenario);
   WsfPerfectTracker& operator=(const WsfPerfectTracker&) = delete;
   ~WsfPerfectTracker() override;
   WsfProcessor* Clone() const override;

   bool Initialize2(double aSimTime) override;
   bool ProcessInput(UtInput& aInput) override;
   void Update(double aSimTime) override;

   void SetTargetByNameId(WsfStringId aNameId);
   void SetTargetByIndex(size_t aPlatformIndex);
   void ClearTrackedTarget();

   size_t GetTargetIndex() const { return mTargetIndex; }

protected:
   inline void NullTrack();
   inline void SetValidityFlags();

   void EstablishNewTrack(double aSimTime, WsfPlatform* aTgtPtr);
   void UpdateExistingTrack(double aSimTime, WsfPlatform* aTgtPtr);

   virtual void DegradeTrack(double /*aSimTime*/);

   //! Copy constructor (for Clone())
   WsfPerfectTracker(const WsfPerfectTracker& aSrc);

   // ----- Attributes ----- :
   WsfStringId mTgtNameId;
   size_t      mTargetIndex;
   bool        mChanged;
   WsfTrackId  mTrackId;
   WsfTrack    mTrack;
   double      mLastUpdate;
};

#endif
