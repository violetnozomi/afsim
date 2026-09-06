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

#ifndef WSFSCRIPTSTATEMACHINEPROCESSOR_HPP
#define WSFSCRIPTSTATEMACHINEPROCESSOR_HPP

#include "wsf_export.h"

#include "script/WsfScriptProcessor.hpp"
#include "script/WsfScriptStateMachine.hpp"

//! A specialization of WsfScriptProcessor that also provides a finite state machine.
//!
//! This class provides a scriptable finite state machine. The user can define
//! states and the rules to transition between states from within the input file.
class WSF_EXPORT WsfScriptStateMachineProcessor : public WsfScriptProcessor
{
public:
   WsfScriptStateMachineProcessor(WsfScenario& aScenario);
   ~WsfScriptStateMachineProcessor() override;

   WsfProcessor* Clone() const override;

   virtual int Evaluate(int aCurrentState);

   bool Initialize(double aSimTime) override;

   bool ProcessInput(UtInput& aInput) override;

   bool ProcessMessage(double aSimTime, const WsfMessage& aMessage) override;

   void Update(double aSimTime) override;

   const WsfScriptStateMachine* GetStateMachine() const override { return &mStateMachine; }

protected:
   WsfScriptStateMachineProcessor(const WsfScriptStateMachineProcessor& aSrc);

   //! The state machine
   WsfScriptStateMachine mStateMachine;
   UtScriptData*         mStateMachineVarPtr;
};

#endif
