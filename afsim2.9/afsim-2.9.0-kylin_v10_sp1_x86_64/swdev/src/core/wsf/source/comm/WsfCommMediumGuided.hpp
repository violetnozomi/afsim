// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMMMEDIUMGUIDED_HPP
#define WSFCOMMMEDIUMGUIDED_HPP

#include "wsf_export.h"

#include "UtMath.hpp"
#include "WsfCommMedium.hpp"
#include "WsfCommMediumTypeIdentifier.hpp"
#include "WsfCommMediumTypes.hpp"
#include "WsfRandomVariable.hpp"

namespace wsf
{
namespace comm
{
namespace medium
{

//! A mode implementation for common guided legacy usage.
class WSF_EXPORT ModeGuided : public Mode
{
public:
   ModeGuided() = default;
   ModeGuided(const std::string& aName)
      : Mode(aName)
   {
   }
   ~ModeGuided() override             = default;
   ModeGuided(const ModeGuided& aSrc) = default;
   ModeGuided* Clone() const override { return new ModeGuided(*this); }

   const char* GetScriptClassName() const override { return "WsfCommMediumModeGuided"; }

   bool   ProcessInput(UtInput& aInput) override;
   double GetTransmissionTime(double aSimTime, Comm& aXmtr, Layer* aLayerPtr, Message& aMessage) override;
   double GetPropagationTime(double aSimTime, Comm& aXmtr, Comm& aRcvr, Message& aMessage) override;
   double GetPacketLossTime(double aSimTime, Message& aMessage) override;

   WsfRandomVariable& GetPropagationSpeed() { return mPropagationSpeed; }
   WsfRandomVariable& GetTransferRate() { return mTransferRate; }
   WsfRandomVariable& GetDelayTime() { return mDelayTime; }

private:
   WsfRandomVariable mPropagationSpeed{UtMath::cLIGHT_SPEED, WsfRandomVariable::cNON_NEGATIVE};
   WsfRandomVariable mTransferRate{0.0, WsfRandomVariable::cPOSITIVE};
   WsfRandomVariable mDelayTime{0.0, WsfRandomVariable::cNON_NEGATIVE};
};

//! The guided mode script class.
class WSF_EXPORT ScriptMediumModeGuidedClass : public ScriptMediumModeClass
{
public:
   ScriptMediumModeGuidedClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   ~ScriptMediumModeGuidedClass() override = default;

   UT_DECLARE_SCRIPT_METHOD(GetPropagationSpeed);
   UT_DECLARE_SCRIPT_METHOD(GetTransferRate);
   UT_DECLARE_SCRIPT_METHOD(GetPacketLossTime);
};

//! This medium provides legacy capabilities for usage in AFSIM. It is intended
//! to provide the functionality available in previous iterations without the need
//! to modify existing user input. As the legacy guided medium model, it provides only
//! basic functionality that is applicable to all objects that utilize mediums
//! within the AFSIM core.
class WSF_EXPORT Guided : public Medium
{
public:
   static void RegisterFactory(Types& aTypes);

   Guided(WsfScenario& aScenario);
   ~Guided() override = default;

   const char*    GetScriptClassName() const override { return "WsfCommMediumGuided"; }
   TypeIdentifier GetMediumIdentifier() const override { return TypeIdentifier::cGUIDED; }
   Guided*        Clone() const override;
   bool           ProcessInput(UtInput& aInput) override;

   std::unique_ptr<Mode> CreateDefaultMode() const override { return ut::make_unique<ModeGuided>(); }

   TransmissionResult TransmitMessage(double aSimTime, Layer* aLayerPtr, Message& aMessage, Comm& aXmtr) override;

   void EndTransmission(double aSimTime, const Message::Identifier& aIdentifier) final;

   virtual void EndTransmission(double aSimTime, MessageStatus& aMessageStatus, const Message::Identifier& aIdentifier);

   void PropagateMessage(double aSimTime, const Message::Identifier& aIdentifier) final;

   void PropagateMessage(double aSimTime, MessageStatus& aMessageStatus, const Message::Identifier& aIdentifier);

   void EndPropagation(double aSimTime, const Message::Identifier& aIdentifier) final;

   virtual void EndPropagation(double aSimTime, MessageStatus& aMessageStatus, const Message::Identifier& aIdentifier);
};

WSF_COMM_MEDIUM_DECLARE_ROLE_TYPE(Guided, cGUIDED);

} // namespace medium
} // namespace comm
} // namespace wsf

#endif
