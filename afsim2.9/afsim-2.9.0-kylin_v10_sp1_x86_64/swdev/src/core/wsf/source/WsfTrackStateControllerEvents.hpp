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
#ifndef WSFTRACKSTATECONTROLLEREVENTS_HPP
#define WSFTRACKSTATECONTROLLEREVENTS_HPP

#include "WsfTrackStateController.hpp"

//! A nested class for the event that performs the periodic review of a track.
class WsfTrackStateController_RequestEvaluationEvent : public WsfEvent
{
public:
   WsfTrackStateController_RequestEvaluationEvent(double                   aSimTime,
                                                  WsfTrackStateController* aControllerPtr,
                                                  WsfLocalTrackStatus*     aStatusPtr);
   EventDisposition Execute() override;

private:
   size_t                   mPlatformIndex;
   WsfTrackStateController* mControllerPtr;
   WsfTrackId               mLocalTrackId;
   unsigned int             mEvaluationEpoch;
};

//! A nested class to represent the request to evaluate a track.
class WsfTrackStateController_EvaluateRequest : public WsfTimeDelayQueue::Request
{
public:
   WsfTrackStateController_EvaluateRequest(double                   aTimeRequired,
                                           WsfTrackStateController* aControllerPtr,
                                           WsfLocalTrackStatus*     aStatusPtr);

   ~WsfTrackStateController_EvaluateRequest() override;
   bool CompleteRequest(double aSimTime) override;

   WsfTrackStateController* mControllerPtr;
   WsfTrackId               mLocalTrackId;
   unsigned int             mEvaluationEpoch;
};

#endif
