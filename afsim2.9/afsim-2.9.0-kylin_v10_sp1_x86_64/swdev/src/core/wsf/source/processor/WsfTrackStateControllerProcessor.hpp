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

#ifndef WSFTRACKSTATECONTROLLERPROCESSOR_HPP
#define WSFTRACKSTATECONTROLLERPROCESSOR_HPP

#include "wsf_export.h"

#include <memory>

class WsfTimeDelayQueue;
#include "script/WsfScriptProcessor.hpp"
class WsfTrackStateController;

//! A processor for directing a set of local tracks through a state machine.
//!
//! This class makes WsfTrackStateController available as a standard 'processor'.
//! In addition to instantiating the track state controller, it also instantiates
//! the common script context and a message handler.
class WSF_EXPORT WsfTrackStateControllerProcessor : public WsfScriptProcessor
{
public:
   WsfTrackStateControllerProcessor(WsfScenario& aScenario, const std::string& aClassName = "WsfTrackStateController");
   WsfTrackStateControllerProcessor& operator=(const WsfTrackStateControllerProcessor&) = delete;
   ~WsfTrackStateControllerProcessor() override;

   //! @name Standard framework methods.
   //@{

   WsfProcessor* Clone() const override;
   bool          Initialize(double aSimTime) override;
   bool          ProcessInput(UtInput& aInput) override;
   bool          ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
   void          TurnOff(double aSimTime) override;
   void          TurnOn(double aSimTime) override;

   //@}

protected:
   WsfTrackStateControllerProcessor(const WsfTrackStateControllerProcessor& aSrc);

   std::unique_ptr<WsfTimeDelayQueue>       mThinkerPtr;
   std::unique_ptr<WsfTrackStateController> mControllerPtr;
};

#endif
