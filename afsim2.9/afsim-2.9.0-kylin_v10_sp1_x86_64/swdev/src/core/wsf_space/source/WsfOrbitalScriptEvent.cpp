// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalScriptEvent.hpp"

#include <string>

#include "UtCalendar.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfDateTime.hpp"
#include "WsfOrbitalEventVisitor.hpp"
#include "WsfOrbitalManeuvering.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfSpaceOrbitalMissionContext.hpp"
#include "script/WsfScriptContext.hpp"

WsfOrbitalScriptEvent::WsfOrbitalScriptEvent(const WsfScenario& aScenario)
   : WsfOrbitalEvent(aScenario)
   , mScenario(aScenario)
   , mContextPtr(new WsfScriptContext(*aScenario.GetScriptContext(), "WsfOrbitalScriptEvent"))
   , mOnInitializeInput()
   , mOnUpdateInput()
   , mOnCompleteInput()
   , mIsCompleteInput()
   , mOnUpdateScriptPtr(nullptr)
   , mOnCompleteScriptPtr(nullptr)
   , mIsCompleteScriptPtr(nullptr)
{
   mCanVerify = false;
   SetType(GetTypeId());
}

WsfOrbitalScriptEvent::WsfOrbitalScriptEvent(const WsfOrbitalScriptEvent& aSrc)
   : WsfOrbitalEvent(aSrc)
   , mScenario(aSrc.mScenario)
   , mContextPtr(new WsfScriptContext(*(aSrc.mContextPtr)))
   , mOnInitializeInput(aSrc.mOnInitializeInput)
   , mOnUpdateInput(aSrc.mOnUpdateInput)
   , mOnCompleteInput(aSrc.mOnCompleteInput)
   , mIsCompleteInput(aSrc.mIsCompleteInput)
   , mOnUpdateScriptPtr(nullptr)
   , mOnCompleteScriptPtr(nullptr)
   , mIsCompleteScriptPtr(nullptr)
{
}

WsfOrbitalScriptEvent::~WsfOrbitalScriptEvent()
{
   delete mContextPtr;
}

bool WsfOrbitalScriptEvent::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();
   if (command == "on_initialize")
   {
      InputBlockToString(aInput, mOnInitializeInput);
   }
   else if (command == "on_update")
   {
      InputBlockToString(aInput, mOnUpdateInput);
   }
   else if (command == "is_complete")
   {
      InputBlockToString(aInput, mIsCompleteInput, false);
   }
   else if (command == "on_complete")
   {
      InputBlockToString(aInput, mOnCompleteInput, false);
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfOrbitalEvent::ProcessInput(aInput);
   }
   return myCommand;
}

bool WsfOrbitalScriptEvent::Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext)
{
   WsfSpaceMoverBase* spaceMoverPtr = aContext.GetSpaceMover();
   if (spaceMoverPtr)
   {
      // Important note:  We must set the parent script context before compiling scripts,
      // or the context cannot look at space mover and platform instances to find scripts not defined in
      // the blocks themselves.
      mContextPtr->SetParent(&spaceMoverPtr->GetScriptContext());

      try
      {
         if (mOnInitializeInput != "")
         {
            UtInput input;
            input.PushInputString(mOnInitializeInput);
            mContextPtr->ProcessInput(input); // on_initialize is handled directly by the context object.
         }
         if (mOnUpdateInput != "")
         {
            UtInput input;
            input.PushInputString(mOnUpdateInput);
            mContextPtr->ProcessInput(input); // on_update is handled directly by the context object.
            mOnUpdateScriptPtr = mContextPtr->FindScript("on_update");
         }
         if (mIsCompleteInput != "")
         {
            UtInput input;
            input.PushInputString(mIsCompleteInput);
            mIsCompleteScriptPtr = mContextPtr->CompileImplicitScript(input, "is_complete", "bool");
         }
         if (mOnCompleteInput != "")
         {
            UtInput input;
            input.PushInputString(mOnCompleteInput);
            mOnCompleteScriptPtr = mContextPtr->CompileImplicitScript(input, "on_complete", "void");
         }
      }
      catch (std::exception&)
      {
         return false;
      }
   }

   bool ok = WsfOrbitalEvent::Initialize(aEpoch, aContext);
   if (ok && aContext.GetSpaceMover() && aContext.GetSimulation())
   {
      ok = mContextPtr->Initialize(aEpoch.GetTimeSince(aContext.GetSimulation()->GetDateTime().GetStartDateAndTime()),
                                   aContext.GetSpaceMover()->GetScriptContext(), // parent context
                                   this);
   }
   return ok;
}

WsfOrbitalEvent* WsfOrbitalScriptEvent::Clone() const
{
   return new WsfOrbitalScriptEvent(*this);
}

bool WsfOrbitalScriptEvent::ExecuteEvent(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext)
{
   // Update the platform to the given time.
   double simTimeOfEpoch = aEpoch.GetTimeSince(aContext.GetSimulation()->GetDateTime().GetStartDateAndTime());
   aContext.GetSpaceMover()->GetPlatform()->Update(simTimeOfEpoch);

   if (mOnUpdateScriptPtr != nullptr)
   {
      double simTime = aEpoch.GetTimeSince(aContext.GetSimulation()->GetDateTime().GetStartDateAndTime());
      mContextPtr->Update(simTime);
      mContextPtr->ExecuteScript(simTime, mOnUpdateScriptPtr);
   }
   return true;
}

bool WsfOrbitalScriptEvent::EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator)
{
   bool complete = false;
   if (mIsCompleteScriptPtr != nullptr)
   {
      UtScriptData     returnVal;
      UtScriptDataList arguments;
      mContextPtr->ExecuteScript(mIsCompleteScriptPtr, returnVal, arguments);
      if (returnVal.GetType() == ut::script::Data::Type::cBOOL)
      {
         complete = returnVal.GetBool();
      }
      else
      {
         auto logger = ut::log::error() << "WsfOrbitalScriptEvent: Expected bool return type from is_complete script.";
         logger.AddNote() << "Platform Name: " << WsfScriptContext::GetPLATFORM(mContextPtr->GetContext())->GetName();
         complete = WsfOrbitalEvent::EvaluateCompletion(aEpoch, aPropagator);
      }
   }
   else
   {
      complete = WsfOrbitalEvent::EvaluateCompletion(aEpoch, aPropagator);
   }
   if (complete && (mOnCompleteScriptPtr != nullptr))
   {
      mContextPtr->ExecuteScript(aEpoch.GetTimeSince(mContextPtr->GetSimulation()->GetDateTime().GetStartDateAndTime()),
                                 mOnCompleteScriptPtr);
   }
   return complete;
}

// private
void WsfOrbitalScriptEvent::InputBlockToString(UtInput& aInput, std::string& aString, bool aIncludeBlock)
{
   UtInputBlock block(aInput);
   std::string  output = "";
   if (aIncludeBlock)
   {
      output = block.GetCommand() + ' ';
   }
   std::string command;
   bool        blockEmpty = true;
   while (block.ReadCommand(command))
   {
      blockEmpty = false;
      output += command + ' ';
   }
   if (!blockEmpty)
   {
      output += command + ' ';
      aString = output;
   }
   else
   {
      aString = "";
   }
}

WsfStringId WsfOrbitalScriptEvent::GetTypeId()
{
   return "SCRIPTED";
}

//! If the orbital constraint was at a point in the orbit for an impulsive event (duration == 0.0),
//! return a time at the same point in the next orbit.  Otherwise perform default scheduling.
bool WsfOrbitalScriptEvent::GetNextExecutionTime(const UtCalendar&              aCurrentTime,
                                                 const UtOrbitalPropagatorBase& aPropagator,
                                                 UtCalendar&                    aNextTime)
{
   if ((mOnUpdateScriptPtr == nullptr) || IsFinite() || (GetCondition().GetType() == wsf::space::NoneCondition::cTYPE) ||
       (GetCondition().GetType() == wsf::space::RelativeTimeCondition::cTYPE))
   {
      return WsfOrbitalEvent::GetNextExecutionTime(aCurrentTime, aPropagator, aNextTime);
   }
   else
   {
      aNextTime = aCurrentTime;
      double dt = 0.0;
      // Check for the constraint time on the next orbit
      // This can be a bit ambiguous for some constraints because we have just hit the it and are looking
      // for the next time (nominally by setting the orbit to one).  However, roundoff error may cause
      // the algorithm to return the next constraint time with orbit 0, so check that first.
      GetCondition().SetOrbitNumber(0);
      bool constraintMet = GetCondition().GetTimeToCondition(aPropagator, dt);

      // Determine that we hit the same constraint if the time to constraint is less that 1 second.
      if (!constraintMet || (dt < 1.0))
      {
         GetCondition().SetOrbitNumber(1);
         constraintMet = GetCondition().GetTimeToCondition(aPropagator, dt);
      }
      if (constraintMet)
      {
         aNextTime.AdvanceTimeBy(dt);
      }
      return constraintMet;
   }
}

void WsfOrbitalScriptEvent::Accept(WsfOrbitalEventVisitor* aVisitorPtr)
{
   aVisitorPtr->VisitOrbitalScriptEvent(this);
}
