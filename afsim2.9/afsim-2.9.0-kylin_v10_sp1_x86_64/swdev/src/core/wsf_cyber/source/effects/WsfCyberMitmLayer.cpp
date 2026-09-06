// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCyberMitmLayer.hpp"

#include "UtMemory.hpp"
#include "WsfSimulation.hpp"
#include "comm/WsfCommNetworkManager.hpp"
#include "effects/WsfCyberMitmEvent.hpp"

namespace wsf
{
namespace cyber
{

namespace
{
// =================================================================================================
std::unique_ptr<MitmMessage> ExecuteScript(double aSimTime, UtScript* aScriptPtr, WsfMessage* aMessage, WsfScriptContext& aContext)
{
   UtScriptData       scriptRetVal;
   UtScriptDataPacker scriptArgs{aContext.GetTypes()};
   scriptArgs << aMessage;
   aContext.ExecuteScript(aSimTime, aScriptPtr, scriptRetVal, scriptArgs);
   auto returnVal = scriptRetVal.GetPointer()->GetAppObject<MitmMessage>();
   return ut::clone(returnVal);
}
} // namespace

// =================================================================================================
MITM_Layer* MITM_Layer::Clone() const
{
   return new MITM_Layer(*this);
}

// =================================================================================================
bool MITM_Layer::ProcessLayerMessage(double aSimTime, comm::layer::Message aLayerMessage, comm::Message* aCommMessagePtr)
{
   // Do not interrupt normal layer messaging. This layer will automatically forward any message received.
   if (aLayerMessage.FromLower() && GetUpperLayer())
   {
      return Parent()->ProcessLayerMessage(aSimTime,
                                           GetUpperLayer()->GetCommLayerImp()->GetIndex(),
                                           aLayerMessage,
                                           aCommMessagePtr);
   }
   else if (!aLayerMessage.FromLower() && GetLowerLayer())
   {
      return Parent()->ProcessLayerMessage(aSimTime,
                                           GetLowerLayer()->GetCommLayerImp()->GetIndex(),
                                           aLayerMessage,
                                           aCommMessagePtr);
   }

   return false;
}

// =================================================================================================
bool MITM_Layer::Send(double aSimTime, size_t, comm::Message& aMessage, bool& aNotifyNextLayer)
{
   Exfiltrate(aSimTime, aMessage);

   aNotifyNextLayer = true;
   if (mIsSend && mOnSendReceivePtr && Parent() &&
       !aMessage.SourceMessage()->GetAuxData().AttributeExists("EXFILTRATION_MESSAGE"))
   {
      auto mitmMessage = ExecuteScript(aSimTime, mOnSendReceivePtr, aMessage.SourceMessage(), *mContextPtr);
      if (mitmMessage->GetDrop())
      {
         aNotifyNextLayer = false;
         return false;
      }
      if (mitmMessage->GetDelay() > 0.0)
      {
         auto                         simPtr = Parent()->GetSimulation();
         MitmMessage::DelayParameters delayParams;
         delayParams.mPlatformName = Parent()->GetPlatform()->GetName();
         delayParams.mCommName     = Parent()->GetName();
         delayParams.mIsSending    = true;
         delayParams.mMITM_LayerID = Parent()->GetProtocolStack().GetLayer(GetIndex())->GetUniqueId();
         if (GetLowerLayer())
         {
            auto curLayer = GetLowerLayer();
            while (curLayer)
            {
               delayParams.mNextLayerIDs.push_back(curLayer->GetUniqueId());
               curLayer = curLayer->GetLowerLayer();
            }
         }

         mitmMessage->SetDelayParameters(delayParams);
         simPtr->AddEvent(ut::make_unique<MITM_DelayEvent>(aSimTime + mitmMessage->GetDelay(), std::move(mitmMessage)));

         aNotifyNextLayer = false;
         return true;
      }

#undef GetMessage
      // Update message based on returned message from script method usage.
      aMessage = *mitmMessage->GetMessage();
   }

   return true;
}

// =================================================================================================
bool MITM_Layer::Receive(double aSimTime, comm::Comm* aXmtrPtr, comm::Message& aMessage, bool& aNotifyNextLayer)
{
   Exfiltrate(aSimTime, aMessage);

   aNotifyNextLayer = true;
   if (!mIsSend && mOnSendReceivePtr && Parent() &&
       !aMessage.SourceMessage()->GetAuxData().AttributeExists("EXFILTRATION_MESSAGE"))
   {
      auto mitmMessage = ExecuteScript(aSimTime, mOnSendReceivePtr, aMessage.SourceMessage(), *mContextPtr);
      if (mitmMessage->GetDrop())
      {
         aNotifyNextLayer = false;
         return false;
      }
      if (mitmMessage->GetDelay() > 0.0)
      {
         auto                         simPtr = Parent()->GetSimulation();
         MitmMessage::DelayParameters delayParams;
         delayParams.mPlatformName = Parent()->GetPlatform()->GetName();
         delayParams.mCommName     = Parent()->GetName();
         delayParams.mIsSending    = false;
         delayParams.mMITM_LayerID = Parent()->GetProtocolStack().GetLayer(GetIndex())->GetUniqueId();
         if (GetUpperLayer())
         {
            auto curLayer = GetUpperLayer();
            while (curLayer)
            {
               delayParams.mNextLayerIDs.push_back(curLayer->GetUniqueId());
               curLayer = curLayer->GetUpperLayer();
            }
         }

         mitmMessage->SetDelayParameters(delayParams);
         simPtr->AddEvent(
            ut::make_unique<MITM_DelayEvent>(aSimTime + mitmMessage->GetDelay(), std::move(mitmMessage), aXmtrPtr));

         aNotifyNextLayer = false;
         return true;
      }

#undef GetMessage
      // Update message based on returned message from script method usage.
      aMessage = *mitmMessage->GetMessage();
   }

   return true;
}

// =================================================================================================
// static
void MITM_Layer::ResolveDelay(double                       aSimTime,
                              comm::LayerImp&              aLayer,
                              std::unique_ptr<MitmMessage> aMessage,
                              comm::Comm*                  aXmtrPtr) // = nullptr
{
   if (aMessage->GetDelayParameters().mIsSending)
   {
      if (aLayer.GetLowerLayer())
      {
#undef GetMessage
         // Layers exist below this one. Continue stack processing.
         // This implementation assumes a following layer will handle
         // the propagation/transmission of the message.
         aLayer.GetLowerLayer()->Send(aSimTime, *aMessage->GetMessage());
      }
      else
      {
#undef GetMessage
         // Since there is no other layers below this one, there is no
         // implementation handling transmission/propagation. Let the
         // comm implementation handle a notionally successful transmission.
         aLayer.Parent()->SendActions(aSimTime, true, *aMessage->GetMessage());
      }
   }
   else
   {
      if (aLayer.GetUpperLayer())
      {
#undef GetMessage
         // Layers exist above this one. Continue stack processing.
         // Because reception is handled at the comm level, if the rest of the stack
         // processing is successful, let the comm implementation handle via its
         // defined behavior.
         if (aLayer.GetUpperLayer()->Receive(aSimTime, aXmtrPtr, *aMessage->GetMessage()))
         {
            aLayer.Parent()->ReceiveActions(aSimTime, true, aXmtrPtr, *aMessage->GetMessage());
         }
      }
      else
      {
#undef GetMessage
         // No other layers. Use the comm implementation defined method
         // for what a comm does when a message is successfully received.
         aLayer.Parent()->ReceiveActions(aSimTime, true, aXmtrPtr, *aMessage->GetMessage());
      }
   }
}

void MITM_Layer::Exfiltrate(double aSimTime, const comm::Message& aMessage)
{
   if (mExfiltrateToPlatform && !aMessage.SourceMessage()->GetAuxData().AttributeExists("EXFILTRATION_MESSAGE") &&
       !aMessage.SourceMessage()->GetAuxData().AttributeExists("EXFILTRATED_MESSAGE"))
   {
      auto comm           = Parent();
      auto sim            = comm->GetSimulation();
      auto networkManager = wsf::comm::NetworkManager::Find(*sim);
      auto cloneMessage   = [&]()
      {
         auto msg = aMessage.SourceMessage();
         msg->GetAuxData().Add("EXFILTRATION_MESSAGE", true);
         return msg;
      };

      auto platform = sim->GetPlatformByIndex(*mExfiltrateToPlatform);
      if (platform)
      {
         auto findComm = [&]()
         {
            mExfiltrateToAddress = ut::nullopt;
            for (auto targetComm : platform->GetComponentRange<wsf::comm::Comm>())
            {
               if (networkManager->PathExists(comm->GetAddress(), targetComm->GetAddress()))
               {
                  auto sent = comm->Send(aSimTime, cloneMessage(), targetComm->GetAddress());

                  if (sent)
                  {
                     mExfiltrateToAddress = targetComm->GetAddress();
                     break;
                  }
               }
            }
         };

         if (!mExfiltrateToAddress)
         {
            findComm();
         }
         else
         {
            auto sent = comm->Send(aSimTime, cloneMessage(), *mExfiltrateToAddress);
            if (!sent)
            {
               findComm();
            }
         }

         if (!mExfiltrateToAddress && comm->DebugEnabled())
         {
            ut::log::debug() << "Unable to exfiltrate message from comm " << comm->GetPlatform()->GetName() << "."
                             << comm->GetName() << " to platform " << platform->GetName() << ".";
         }
         else
         {
            aMessage.SourceMessage()->GetAuxData().Add("EXFILTRATED_MESSAGE", true);
         }
      }
   }
}

} // namespace cyber
} // namespace wsf
