// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "NetworkEnabledWeaponPart.hpp"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <sstream>

#include "ComputerProcessor.hpp"
#include "FieldTypes.hpp"
#include "Message11_1.hpp"
#include "Messages.hpp"
#include "MessagesFactory.hpp"
#include "ScriptMessage.hpp"
#include "ScriptedPartClass.hpp"
#include "UtScript.hpp"
#include "UtScriptContext.hpp"
#include "UtScriptExecutor.hpp"
#include "UtScriptFunction.hpp"
#include "UtStringUtil.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfTrack.hpp"
#include "WsfWeapon.hpp"
#include "script/WsfScriptContext.hpp"


namespace WsfL16
{
NetworkEnabledWeaponPart::NetworkEnabledWeaponPart(WsfScenario& aScenario)
   : ComputerPart(aScenario)
   , mNetworkEnabled(true)
{
   mContextPtr->GetContext().RegisterVariable("JPROCESSOR", "WsfTadilJProcessor");
}

NetworkEnabledWeaponPart::NetworkEnabledWeaponPart(const NetworkEnabledWeaponPart& aSrc)
   : ComputerPart(aSrc)
   , mNetworkEnabled(aSrc.mNetworkEnabled)
{
}

// virtual
NetworkEnabledWeaponPart::~NetworkEnabledWeaponPart() {}

ComputerPart* NetworkEnabledWeaponPart::Clone() const
{
   return new NetworkEnabledWeaponPart(*this);
}

bool NetworkEnabledWeaponPart::Initialize(double aSimTime, ComputerProcessor& aComputer)
{
   mOnSendScript = nullptr;
   bool ok       = ComputerPart::Initialize(aSimTime, aComputer);
   if (ok)
   {
      UtScriptContext& context = mContextPtr->GetContext();
      context.Var("JPROCESSOR").GetPointer()->SetAppObject(this);

      const UtScriptScope::ScriptMap& scripts = context.GetScope()->GetScripts();
      for (UtScriptScope::ScriptMap::const_iterator i = scripts.begin(); i != scripts.end(); ++i)
      {
         if (!i->second.mIsLocal)
         {
            continue;
         }
         UtScript* script = i->second.mScriptPtr;
         if (script->GetName() == "on_send")
         {
            mOnSendScript = script;
         }
         else if (script->GetName().substr(0, 11) == "on_message_")
         {
            std::string labelStr = script->GetName().substr(11);
            int         lbl, sublbl;
            if (DecodeLabel(labelStr, lbl, sublbl))
            {
               if (script->mPrototype->mArgs.size() == 1)
               {
                  UtScriptClass*    argType = context.GetTypes()->GetClass(script->mPrototype->mArgs[0]);
                  std::stringstream ss;
                  ss << "WsfTadilJ" << lbl << '_' << sublbl << "I";
                  std::string goodType = ss.str();
                  if (argType && argType->GetClassName() == goodType)
                  {
                     mReceiveScripts[MessageLabel(lbl, sublbl)] = script;
                  }
               }
            }
         }
      }
   }
   return ok;
}

bool NetworkEnabledWeaponPart::DecodeLabel(const std::string& aLabelStr, int& aLabel, int& aSubLabel)
{
   std::string tmp = aLabelStr;
   std::replace(tmp.begin(), tmp.end(), '_', ' ');
   return 2 == sscanf(tmp.c_str(), "%i %i", &aLabel, &aSubLabel);
}

bool NetworkEnabledWeaponPart::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;
   if (ComputerPart::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void NetworkEnabledWeaponPart::HandleReceive(double aSimTime, const Messages::InitialBase& aMessage)
{
   int lbl, sublbl;
   aMessage.GetLabel(lbl, sublbl);
   std::map<MessageLabel, UtScript*>::iterator i = mReceiveScripts.find(MessageLabel(lbl, sublbl));
   if (i != mReceiveScripts.end())
   {
      UtScriptContext&                      context = mContextPtr->GetContext();
      UtScript*                             script  = i->second;
      UtScriptData                          retVal;
      WsfL16::Messages::MessageScriptClass* scriptClass = Messages::Factory::GetMessageScriptClass(lbl, sublbl);
      UtScriptData                          msgArg(new UtScriptRef((void*)&aMessage, scriptClass));
      UtScriptDataList                      args;
      args.push_back(msgArg);
      context.Execute(&GetComputer()->GetSimulation()->GetScriptExecutor(), script, retVal, args);
   }
}

void NetworkEnabledWeaponPart::HandleSend(double aSimTime)
{
   using namespace Messages::J11_1;


   if (mOnSendScript != nullptr)
   {
      UtScriptContext& context = mContextPtr->GetContext();
      UtScriptData     retVal;
      UtScriptDataList args;
      context.Execute(&GetComputer()->GetSimulation()->GetScriptExecutor(), mOnSendScript, retVal, args);
   }
}
} // namespace WsfL16
