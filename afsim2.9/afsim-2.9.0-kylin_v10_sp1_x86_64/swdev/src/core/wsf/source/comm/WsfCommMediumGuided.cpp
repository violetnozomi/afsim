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

#include "WsfCommMediumGuided.hpp"

#include "WsfComm.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommLayerEvent.hpp"
#include "WsfCommLayerMessage.hpp"
#include "WsfCommMediumContainer.hpp"
#include "WsfCommMediumFactory.hpp"
#include "WsfCommMediumTypes.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommPhysicalLayer.hpp"

namespace
{
//! Medium factory to process input for this type.
class GuidedFactory : public wsf::comm::medium::Factory<wsf::comm::medium::Guided>
{
public:
   bool ProcessInput(UtInput& aInput, wsf::comm::medium::Container& aParent) override
   {
      auto& types = wsf::comm::medium::Types::Get(GetScenario());
      return types.ProcessInput(aInput);
      return false;
   }

   bool ProcessAddOrEditCommand(UtInput& aInput, wsf::comm::medium::Container& aParent, bool aIsAdding) override
   {
      auto& types = wsf::comm::medium::Types::Get(GetScenario());
      return types.LoadMedium(aInput, aParent, aIsAdding);
   }

   bool ProcessDeleteCommand(UtInput& aInput, wsf::comm::medium::Container& aParent) override
   {
      auto& types = wsf::comm::medium::Types::Get(GetScenario());
      return types.DeleteMedium(aInput, aParent);
   }
};
} // namespace

namespace wsf
{
namespace comm
{
namespace medium
{

// =================================================================================================
bool ModeGuided::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "propagation_speed")
   {
      mPropagationSpeed.ProcessInput(aInput, UtInput::cSPEED);
   }
   else if (command == "transfer_rate")
   {
      // Note: This transfer rate differs from the implied usage of the original.
      // This command is retained on the comm physical layer definitions to describe
      // the hardware limit in respect to bandwidth. This version defines the limitations
      // imposed by the medium itself, which is variable based on conditions determined by
      // mode selection and driven by the user or script. The lessor of these two rates
      // is used when data is actually transmitted.
      mTransferRate.ProcessInput(aInput, UtInput::cDATA_RATE);
   }
   // Candidate for AFSIM 3.0 API update. Name is too specific, should indicate a general delay time.
   else if (command == "packet_loss_time")
   {
      mDelayTime.ProcessInput(aInput, UtInput::cTIME);
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
double ModeGuided::GetTransmissionTime(double aSimTime, Comm& aXmtr, Layer* aLayerPtr, Message& aMessage)
{
   auto transmissionTime = 0.0; // default instantaneous transfer
   auto messageSizeBits  = aMessage.SourceMessage()->GetSizeBits();
   auto transferRate     = mTransferRate.Draw();

   // Check if the optional layer is provided, and it provides a limitation on transmission rate
   // per its hardware definition.
   if (aLayerPtr)
   {
      auto physicalLayerPtr = dynamic_cast<PhysicalLayer*>(aLayerPtr->GetCommLayerImp());
      if (physicalLayerPtr)
      {
         // The transmission rate is bound by the either the medium or the hardware. Use the lessor
         // of the two, ignoring rates of 0.0 (instantaneous).
         if (physicalLayerPtr->GetTransferRate() > 0.0)
         {
            if (transferRate > 0.0)
            {
               transferRate = std::min(transferRate, physicalLayerPtr->GetTransferRate());
            }
            else
            {
               transferRate = physicalLayerPtr->GetTransferRate();
            }
         }
      }
   }

   if ((messageSizeBits > 0) && (transferRate > 0.0))
   {
      transmissionTime = static_cast<double>(messageSizeBits) / transferRate;
   }

   return transmissionTime;
}

// =================================================================================================
double ModeGuided::GetPropagationTime(double aSimTime, Comm& aXmtr, Comm& aRcvr, Message& aMessage)
{
   auto propagationTime  = 0.0; // default instantaneous propagation
   auto propagationSpeed = mPropagationSpeed.Draw();

   if (propagationSpeed > 0.0)
   {
      // Update locations of sender and receiver.
      aXmtr.UpdatePosition(aSimTime);
      aRcvr.UpdatePosition(aSimTime);

      // Determine distance between sender and receiver.
      double offsetWCS[3];
      aXmtr.GetPlatform()->GetRelativeLocationWCS(aRcvr.GetPlatform(), offsetWCS);
      auto distance = UtVec3d::Magnitude(offsetWCS);

      propagationTime = distance / propagationSpeed;
   }

   return propagationTime;
}

// =================================================================================================
double ModeGuided::GetPacketLossTime(double aSimTime, Message& aMessage)
{
   return mDelayTime.Draw();
}

// =================================================================================================
ScriptMediumModeGuidedClass::ScriptMediumModeGuidedClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : ScriptMediumModeClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCommMediumModeGuided");

   AddMethod(ut::make_unique<GetPropagationSpeed>());
   AddMethod(ut::make_unique<GetTransferRate>());
   AddMethod(ut::make_unique<GetPacketLossTime>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumModeGuidedClass, ModeGuided, GetPropagationSpeed, 0, "WsfRandomVariable", "")
{
   // Returns a reference. The user may modify this value to change parameters, the distribution, and make draws.
   aReturnVal.SetPointer(UtScriptRef::Ref(&aObjectPtr->GetPropagationSpeed(), aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumModeGuidedClass, ModeGuided, GetTransferRate, 0, "WsfRandomVariable", "")
{
   // Returns a reference. The user may modify this value to change parameters, the distribution, and make draws.
   aReturnVal.SetPointer(UtScriptRef::Ref(&aObjectPtr->GetTransferRate(), aReturnClassPtr));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumModeGuidedClass, ModeGuided, GetPacketLossTime, 0, "WsfRandomVariable", "")
{
   // Returns a reference. The user may modify this value to change parameters, the distribution, and make draws.
   aReturnVal.SetPointer(UtScriptRef::Ref(&aObjectPtr->GetDelayTime(), aReturnClassPtr));
}

// =================================================================================================
Guided::Guided(WsfScenario& aScenario)
   : Medium(aScenario)
{
   // Add the "default" mode used by this medium type.
   mModes.emplace_back(ut::make_unique<ModeGuided>("default"));
}

// =================================================================================================
void Guided::RegisterFactory(Types& aTypes)
{
   aTypes.GetManager().AddFactory<Guided>(ut::make_unique<GuidedFactory>());
}

// =================================================================================================
Guided* Guided::Clone() const
{
   return new Guided(*this);
}

// =================================================================================================
bool Guided::ProcessInput(UtInput& aInput)
{
   return Medium::ProcessInput(aInput);
}

// =================================================================================================
TransmissionResult Guided::TransmitMessage(double aSimTime, Layer* aLayerPtr, Message& aMessage, Comm& aXmtr)
{
   auto        networkManagerPtr = GetSimulation()->GetCommNetworkManager();
   const auto& nextHop           = aMessage.SourceMessage()->GetNextHopAddr();
   auto        rcvrComm          = networkManagerPtr->GetComm(nextHop);
   auto        result            = TransmissionResult::cSUCCESS;

   if (!rcvrComm)
   {
      return TransmissionResult::cFAILURE_NONEXISTENT_RECEIVER;
   }

   if (GetCurrentTransmissions() >= GetNumChannels())
   {
      return TransmissionResult::cFAILURE_CHANNEL_LIMIT_REACHED;
   }

   if (mDebug)
   {
      auto out = ut::log::debug() << "Starting transmission of message.";
      out.AddNote() << "T=" << aSimTime;
      out.AddNote() << "Transmitter: " << aXmtr.GetFullName();
      out.AddNote() << "Message: " << *aMessage.SourceMessage();
   }

   // Another candidate for change in legacy behavior for AFSIM 3.0. This message should be delayed
   // until after other potential failures that are new to the comm framework since 2.3. It has to
   // be used here to avoid potentially modifying event output.
   WsfObserver::MessageTransmitted(GetSimulation())(aSimTime, &aXmtr, *(aMessage.SourceMessage()));

   // Get the current indicated mode of the medium for use with this message.
   auto modePtr = GetModeForTransmission(aSimTime, aXmtr, *rcvrComm, aMessage);
   if (!modePtr)
   {
      ut::log::error() << "A medium mode was designated for transmission that does not exist.";
      throw UtException("Invalid mode designation for WsfCommMediumGuided.");
   }

   // Get the transmission time and packet loss time.
   auto transmissionTime    = modePtr->GetTransmissionTime(aSimTime, aXmtr, aLayerPtr, aMessage);
   auto packetLossTime      = modePtr->GetPacketLossTime(aSimTime, aMessage);
   auto transmissionEndTime = (aSimTime + transmissionTime + packetLossTime);

   // Determine the delivery time (propagation time + transmission time) as of now. This is not guaranteed.
   auto propagationTime = modePtr->GetPropagationTime(aSimTime, aXmtr, *rcvrComm, aMessage);
   auto deliveryTime    = (aSimTime + transmissionTime + packetLossTime + propagationTime);

   // Add the message for bookkeeping.
   auto msgIter = mMessages.emplace(std::piecewise_construct,
                                    std::forward_as_tuple(aMessage),
                                    std::forward_as_tuple(aSimTime, aMessage, GetModeIndex(modePtr->GetName()).first));
   if (!msgIter.second)
   {
      assert(false);
      ut::log::error() << "A transmit message request was made for a message thats already being transmitted";
      throw UtException("Invalid duplicate of message to transmit encountered for WsfCommMediumGuided.");
   }
   MessageStatus&             status     = msgIter.first->second;
   const Message::Identifier& identifier = msgIter.first->first;

   status.SetTimeTransmissionEnd(transmissionEndTime);
   status.SetTimeDelivery(deliveryTime);
   status.SetLayer(aLayerPtr);
   status.SetTimeLastXmtrStatusChange(aXmtr.GetLastStatusChangeTime());

   // Determine if the recipient is actually able to receive a message based on simulation
   // truth state, by checking with the network manager. If so, schedule the message delivery.
   if (!networkManagerPtr->PathExists(aXmtr.GetAddress(), rcvrComm->GetAddress()))
   {
      // Indicate a message abort. The message will still continue processing but will not be delivered.
      status.SetAbortDelivery(true);
   }


   if (transmissionTime + packetLossTime > 0.0)
   {
      // Non-instantaneous transmission time.
      // Schedule the transmission end event.
      GetSimulation()->AddEvent(
         std::move(ut::make_unique<GenericEvent<int>>(transmissionEndTime,
                                                      mEventPtr,
                                                      [this, transmissionEndTime, identifier]()
                                                      { EndTransmission(transmissionEndTime, identifier); })));
   }
   else
   {
      EndTransmission(aSimTime, status, identifier);
   }

   return result;
}

// =================================================================================================
void Guided::EndTransmission(double aSimTime, const Message::Identifier& aIdentifier)
{
   // Get the corresponding message.
   auto messageStatusPtr = GetMessageStatus(aIdentifier);
   if (messageStatusPtr)
   {
      EndTransmission(aSimTime, *messageStatusPtr, aIdentifier);
   }
}

void Guided::EndTransmission(double aSimTime, MessageStatus& aMessageStatus, const Message::Identifier& aIdentifier)
{
   assert(aIdentifier == Message::Identifier(aMessageStatus.GetMessage()));

   // Get the transmitter.
   const auto& xmtrAddress = aMessageStatus.GetMessage().GetTraceRoute().back();
   auto        xmtrPtr     = GetSimulation()->GetCommNetworkManager()->GetComm(xmtrAddress);

   // If the transmitter has been updated, the transmission was a failure.
   if (xmtrPtr->GetLastStatusChangeTime() != aMessageStatus.GetTimeLastXmtrStatusChange())
   {
      aMessageStatus.SetAbortDelivery(true);
   }

   // Indicate status if a layer was provided and has not already done so.
   if (aMessageStatus.GetLayer() && xmtrPtr && !aMessageStatus.IsStatusIssued())
   {
      auto layerMessage = layer::cUP_ACK_SEND;
      if (aMessageStatus.AbortDelivery())
      {
         layerMessage = layer::cUP_NACK_SEND;
      }

      LayerEvent::ScheduleCommLayerEvent(aSimTime,
                                         aMessageStatus.GetLayer()->GetCommLayerImp()->GetIndex(),
                                         xmtrPtr,
                                         layerMessage,
                                         &aMessageStatus.GetMessage());

      aMessageStatus.SetStatusIssued(true);
   }

   // Indicate end of transmission.
   aMessageStatus.SetIsTransmitting(false);
   PropagateMessage(aSimTime, aMessageStatus, aIdentifier);
}

// =================================================================================================
void Guided::PropagateMessage(double aSimTime, const Message::Identifier& aIdentifier)
{
   auto messageStatusPtr = GetMessageStatus(aIdentifier);
   if (messageStatusPtr)
   {
      PropagateMessage(aSimTime, *messageStatusPtr, aIdentifier);
   }
}

void Guided::PropagateMessage(double aSimTime, MessageStatus& aMessageStatus, const Message::Identifier& aIdentifier)
{
   assert(aIdentifier == Message::Identifier(aMessageStatus.GetMessage()));

   // Schedule an event for propagation end (delivery).
   auto deliveryTime = aMessageStatus.GetTimeDelivery();
   if (deliveryTime <= aSimTime)
   {
      EndPropagation(aSimTime, aMessageStatus, aIdentifier);
   }
   else
   {
      Message::Identifier identifier{aIdentifier};
      GetSimulation()->AddEvent(std::move(ut::make_unique<GenericEvent<int>>(deliveryTime,
                                                                             mEventPtr,
                                                                             [this, deliveryTime, identifier]() {
                                                                                EndPropagation(deliveryTime, identifier);
                                                                             })));
   }
}
// =================================================================================================

void Guided::EndPropagation(double aSimTime, const Message::Identifier& aIdentifier)
{
   // Get the corresponding message.
   auto messageStatusPtr = GetMessageStatus(aIdentifier);
   if (messageStatusPtr)
   {
      EndPropagation(aSimTime, *messageStatusPtr, aIdentifier);
   }
}

void Guided::EndPropagation(double aSimTime, MessageStatus& aMessageStatus, const Message::Identifier& aIdentifier)
{
   auto& msg = aMessageStatus.GetMessage();
   assert(aIdentifier == Message::Identifier(msg));

   // Check abort status for delivery of message.
   if (!aMessageStatus.AbortDelivery())
   {
      // Get the recipient.
      const auto& rcvrAddress = msg.SourceMessage()->GetNextHopAddr();
      auto        rcvrPtr     = GetSimulation()->GetCommNetworkManager()->GetComm(rcvrAddress);

      // Get the transmitter.
      const auto& xmtrAddress = msg.GetTraceRoute().back();
      auto        xmtrPtr     = GetSimulation()->GetCommNetworkManager()->GetComm(xmtrAddress);

      if (rcvrPtr)
      {
         if (rcvrPtr->IsTurnedOn())
         {
            rcvrPtr->Receive(aSimTime, xmtrPtr, msg);
         }
      }
   }
   else if (mDebug)
   {
      const auto& xmtrAddress = msg.GetTraceRoute().back();
      const auto& rcvrAddress = msg.SourceMessage()->GetNextHopAddr();
      auto        out         = ut::log::warning() << "Message delivery failure due to abort condition.";
      out.AddNote() << "Transmitter: " << xmtrAddress;
      out.AddNote() << "Receiver: " << rcvrAddress;
      out.AddNote() << "Message serial: " << aIdentifier.GetSerialNumber();
   }

   RemoveMessageStatus(aIdentifier);
}

} // namespace medium
} // namespace comm
} // namespace wsf
