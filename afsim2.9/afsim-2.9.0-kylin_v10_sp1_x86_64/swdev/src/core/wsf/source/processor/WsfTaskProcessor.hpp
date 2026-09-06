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

#ifndef WSFTASKPROCESSOR_HPP
#define WSFTASKPROCESSOR_HPP

#include "wsf_export.h"

#include <memory>
#include <vector>

#include "WsfTaskManager.hpp"
class WsfTrackStateController;

//! A processor for assigning and responding to tasks associated with tracks.
//!
//! The 'task processor' is a combination of the task manager and track
//! state controller (along with all the attendant script stuff). It basically
//! cycles through the track list, evaluates the tracks and makes assignments.
class WSF_EXPORT WsfTaskProcessor : public WsfTaskManager
{
public:
   WsfTaskProcessor(WsfScenario& aScenario, const std::string& aClassName = "WsfTaskProcessor");
   WsfTaskProcessor(const WsfTaskProcessor& aSrc);
   ~WsfTaskProcessor() override;

   const char* GetScriptClassName() const override;

   //! @name Framework methods
   //@{
   WsfProcessor* Clone() const override;
   bool          Initialize(double aSimTime) override;
   bool          Initialize2(double aSimTime) override;
   void          PlatformDeleted(WsfPlatform* aPlatformPtr) override;
   bool          ProcessInput(UtInput& aInput) override;
   bool          ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;
   void          TurnOff(double aSimTime) override;
   void          TurnOn(double aSimTime) override;
   //@}

   //! Get the reference to the 'track state controller' object.
   WsfTrackStateController& GetTrackStateController() { return *mControllerPtr; }

   void SetExternallyControlled(double aSimTime, bool aIsExternallyControlled) override;

private:
   WsfTaskProcessor& operator=(const WsfTaskProcessor&);

protected:
   void TaskCompleted(double aSimTime, WsfTask& aTask) override;

   std::unique_ptr<WsfTrackStateController> mControllerPtr;
};

#endif
