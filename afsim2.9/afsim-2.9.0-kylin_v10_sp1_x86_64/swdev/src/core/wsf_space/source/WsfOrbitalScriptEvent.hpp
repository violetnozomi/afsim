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

#ifndef WSFORBITALSCRIPTEVENT_HPP
#define WSFORBITALSCRIPTEVENT_HPP

#include "wsf_space_export.h"

#include <string>

class UtCalendar;
class UtInput;
class UtScript;
class WsfScriptContext;
#include "UtScriptData.hpp"
#include "WsfOrbitalEvent.hpp"
class WsfScenario;

//! An orbital event that allows scripted behavior.
class WSF_SPACE_EXPORT WsfOrbitalScriptEvent : public WsfOrbitalEvent
{
public:
   explicit WsfOrbitalScriptEvent(const WsfScenario& aScenario);
   WsfOrbitalScriptEvent(const WsfOrbitalScriptEvent& aSrc);
   WsfOrbitalScriptEvent& operator=(const WsfOrbitalScriptEvent&) = delete;
   ~WsfOrbitalScriptEvent() override;

   bool ProcessInput(UtInput& aInput) override;
   bool Initialize(const UtCalendar& aEpoch, const wsf::space::OrbitalMissionContext& aContext) override;

   //! Make a copy of this object.
   WsfOrbitalEvent* Clone() const override;

   const char* GetScriptClassName() const override { return "WsfOrbitalScriptEvent"; }

   static WsfStringId GetTypeId();

   bool GetNextExecutionTime(const UtCalendar&              aCurrentTime,
                             const UtOrbitalPropagatorBase& aPropagator,
                             UtCalendar&                    aNextTime) override;

   const std::string& GetOnInitializeInput() const { return mOnInitializeInput; }
   const std::string& GetOnUpdateInput() const { return mOnUpdateInput; }
   const std::string& GetOnCompleteInput() const { return mOnCompleteInput; }
   const std::string& GetIsCompleteInput() const { return mIsCompleteInput; }

   //! Set the initial input string representing the name of a context-accessible script.
   //! @note This method is primarily used in the corresponding script class.
   void SetOnInitializeInput(const std::string& aInput) { mOnInitializeInput = aInput; }


   //! Set the update input string representing the name of a context-accessible script.
   //! @note This method is primarily used in the corresponding script class.
   void SetOnUpdateInput(const std::string& aInput) { mOnUpdateInput = aInput; }

   //! Set the complete input string representing the name of a context-accessible script.
   //! @note This method is primarily used in the corresponding script class.
   void SetOnCompleteInput(const std::string& aInput) { mOnCompleteInput = aInput; }

   //! Set the complete input string representing the name of a context-accessible script.
   //! @note This method is primarily used in the corresponding script class.
   void SetIsCompleteInput(const std::string& aInput) { mIsCompleteInput = aInput; }

   void Accept(WsfOrbitalEventVisitor* aVisitorPtr) override;

protected:
   bool ExecuteEvent(const UtCalendar& aEpoch, wsf::space::OrbitalMissionContext& aContext) override;

   bool EvaluateCompletion(const UtCalendar& aEpoch, const UtOrbitalPropagatorBase& aPropagator) override;

private:
   static void InputBlockToString(UtInput& aInput, std::string& aString, bool aIncludeBlock = true);

   const WsfScenario& mScenario; // Needed to create the script context before ProcessInput is called.
   WsfScriptContext*  mContextPtr;

   std::string mOnInitializeInput;
   std::string mOnUpdateInput;
   std::string mOnCompleteInput;
   std::string mIsCompleteInput;

   UtScript* mOnUpdateScriptPtr;
   UtScript* mOnCompleteScriptPtr;
   UtScript* mIsCompleteScriptPtr;
};

#endif
