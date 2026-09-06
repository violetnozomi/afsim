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

#include "WsfCommMediumUnguided.hpp"

#include "WsfComm.hpp"
#include "WsfCommEvent.hpp"
#include "WsfCommLayer.hpp"
#include "WsfCommLayerEvent.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommPhysicalLayer.hpp"
#include "WsfCommResult.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"

namespace
{
//! Medium factory to process input for this type.
class UnguidedFactory : public wsf::comm::medium::Factory<wsf::comm::medium::Unguided>
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
bool ModeUnguided::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "snr_transfer_rate_table")
   {
      std::string snrUnits, rateUnits;

      // Read the first item of input
      aInput.ReadValue(snrUnits);
      UtStringUtil::ToLower(snrUnits);
      // If the first input is the SNR units (older table style), also get the data rate units
      // and use those units when reading in the table vales
      // Otherwise, push the first item back and then read the UtTable in normally
      if (UtInput::ValidateUnits(snrUnits, UtInput::cRATIO))
      {
         // read the next item as it is the other units
         aInput.ReadValue(rateUnits);
         mSNR_XferRateTable.ProcessInput(aInput,
                                         UtInput::cRATIO,
                                         snrUnits,
                                         UtTable::NoCheck(),
                                         UtInput::cDATA_RATE,
                                         rateUnits,
                                         UtTable::ValueGE(0.0));
      }
      else
      {
         aInput.PushBack(snrUnits);
         mSNR_XferRateTable.ProcessInput(aInput,
                                         UtInput::cRATIO,
                                         "dB",
                                         UtTable::NoCheck(),
                                         UtInput::cDATA_RATE,
                                         "bit/s",
                                         UtTable::ValueGE(0.0));
      }

      mUseSNR_XferRateTable = mSNR_XferRateTable.IsDefined();
      mUseBER_EbNoTable     = false;
   }
   else if (command == "bit_error_probability")
   {
      aInput.ReadValue(mBitErrorProbability);
      aInput.ValueGreaterOrEqual(mBitErrorProbability, 0.0);
   }
   else if (command == "error_correction")
   {
      double rawRatio = 0.0;
      aInput.ReadValue(rawRatio);
      aInput.ValueInClosedRange(rawRatio, 0.0, 1.0);
      mErrorCorrection = aInput.ConvertValue(rawRatio, "db", UtInput::cRATIO);
   }
   else if (command == "bit_error_rate_ebno_table")
   {
      mBER_EbNoTable.ProcessInput(aInput,
                                  UtInput::cNON_DIMENSIONAL,
                                  "",
                                  UtTable::NoCheck(),
                                  UtInput::cRATIO,
                                  "dB",
                                  UtTable::ValueGE(0.0));

      mUseBER_EbNoTable     = mBER_EbNoTable.IsDefined();
      mUseSNR_XferRateTable = false;
   }
   else if (ModeGuided::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
double ModeUnguided::GetTransmissionTime(double aSimTime, Comm& aXmtr, Layer* aLayerPtr, Message& aMessage)
{
   auto transmissionTime = 0.0; // default instantaneous transfer
   auto messageSizeBits  = aMessage.SourceMessage()->GetSizeBits();

   // Instantaneous transfer for message of size 0
   if (messageSizeBits <= 0)
   {
      return transmissionTime;
   }

   // Get the transfer rate (this updates the local data member, mTransferRate)
   // This defaults to the user input if table based data is not provided, but is
   // within the physical constraints of the being able to receive a message.
   auto transferRate = CalculateTransferRate(aMessage.GetResult(), aXmtr.CanSend());

   // Check if the optional layer is provided, and it provides a limitation on transmission rate
   // per its hardware definition. This is not performed if one of the table methods is specified,
   // in order to conform to legacy input. This should be modified in AFSIM 3.0.
   if (aLayerPtr && !mUseSNR_XferRateTable && !mUseBER_EbNoTable)
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

   if (transferRate > 0.0)
   {
      transmissionTime = static_cast<double>(messageSizeBits) / transferRate;
   }

   return transmissionTime;
}

// =================================================================================================
double ModeUnguided::CalculateTransferRate(Result& aResult, bool aCanSend)
{
   double xferRate = GetTransferRate().LastDraw(); // default to the transfer rate set by the parent input or the network
   // If a SNR Transfer Rate table is set up, calculate the transfer rate based off of the current SNR.
   if (mUseSNR_XferRateTable && mSNR_XferRateTable.IsDefined())
   {
      xferRate = mSNR_XferRateTable.Lookup(aResult.mSignalToNoise);
   }
   // Or if the Eb/No table is set up and we have a transmitter, calculate the transfer rate
   // based on SNR and Eb/No
   else if (mUseBER_EbNoTable && mBER_EbNoTable.IsDefined() && aCanSend)
   {
      // Get the value of Eb/No for the radio's specified BER
      double ebno = mBER_EbNoTable.Lookup(mBitErrorProbability);

      xferRate = (aResult.mSignalToNoise * mErrorCorrection * (aResult.GetReceiver()->GetBandwidth() / ebno));

      // Update the result with the Eb/No related status
      aResult.mEnergyBitToNoiseDensity = ebno;
      aResult.mBitErrorRate            = mBitErrorProbability;
      aResult.mDataRate                = xferRate;
   }
   return xferRate;
}

// =================================================================================================
ScriptMediumModeUnguidedClass::ScriptMediumModeUnguidedClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : ScriptMediumModeGuidedClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfCommMediumModeUnguided");

   AddMethod(ut::make_unique<GetUseXferRateTable>());
   AddMethod(ut::make_unique<GetBitErrorProbability>());
   AddMethod(ut::make_unique<GetErrorCorrection>());
   AddMethod(ut::make_unique<GetUseBER_EbNoTable>());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumModeUnguidedClass, ModeUnguided, GetUseXferRateTable, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetUseXferRateTable());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumModeUnguidedClass, ModeUnguided, GetBitErrorProbability, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBitErrorProbability());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumModeUnguidedClass, ModeUnguided, GetErrorCorrection, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetErrorCorrection());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(ScriptMediumModeUnguidedClass, ModeUnguided, GetUseBER_EbNoTable, 0, "bool", "")
{
   aReturnVal.SetBool(aObjectPtr->GetUseBER_EbNoTable());
}

// =================================================================================================
Unguided::Unguided(WsfScenario& aScenario)
   : Guided(aScenario)
{
   // Remove any modes added by base class constructors.
   mModes.clear();

   // Add the "default" mode used by this medium type.
   mModes.emplace_back(ut::make_unique<ModeUnguided>("default"));
}

// =================================================================================================
void Unguided::RegisterFactory(Types& aTypes)
{
   aTypes.GetManager().AddFactory<Unguided>(ut::make_unique<UnguidedFactory>());
}

// =================================================================================================
Unguided* Unguided::Clone() const
{
   return new Unguided(*this);
}

// =================================================================================================
bool Unguided::ProcessInput(UtInput& aInput)
{
   return Guided::ProcessInput(aInput);
}

// =================================================================================================
TransmissionResult Unguided::TransmitMessage(double aSimTime, Layer* aLayerPtr, Message& aMessage, Comm& aXmtr)
{
   auto        networkManagerPtr = GetSimulation()->GetCommNetworkManager();
   const auto& nextHop           = aMessage.SourceMessage()->GetNextHopAddr();
   auto        rcvrComm          = networkManagerPtr->GetComm(nextHop);
   bool        canSendTo         = true;
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
      auto out = ut::log::debug() << "Started transmission of message.";
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
      throw UtException("Invalid mode designation for WsfCommMediumUnguided.");
   }

   // The medium type does not assume perfect connectivity. Check comm model constraints.
   canSendTo &= aXmtr.CanSendTo(aSimTime, rcvrComm, &aMessage);

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
      throw UtException("Invalid duplicate of message to transmit encountered for WsfCommMediumUnguided.");
   }
   MessageStatus&             status     = msgIter.first->second;
   const Message::Identifier& identifier = msgIter.first->first;

   status.SetTimeTransmissionEnd(transmissionEndTime);
   status.SetTimeDelivery(deliveryTime);
   status.SetLayer(aLayerPtr);
   status.SetTimeLastXmtrStatusChange(aXmtr.GetLastStatusChangeTime());

   // Determine if the recipient is actually able to receive a message based on simulation
   // truth state, by checking with the network manager. If so, schedule the message delivery.
   // Also take into account the previous comm model constraints check.
   if (!canSendTo || (!networkManagerPtr->PathExists(aXmtr.GetAddress(), rcvrComm->GetAddress())))
   {
      // Indicate a message abort. The message will still continue processing but will not be delivered.
      status.SetAbortDelivery(true);
   }

   // Inform observers and listeners of the transmission attempt.
   if (aMessage.GetResult().mCheckedStatus != 0)
   {
      WsfObserver::MessageDeliveryAttempt(
         GetSimulation())(aSimTime, &aXmtr, rcvrComm, *aMessage.SourceMessage(), aMessage.GetResult());
      WsfEM_Xmtr* xmtrPtr = aMessage.GetResult().GetTransmitter();
      if (xmtrPtr)
      {
         xmtrPtr->SetTransmissionEndTime(deliveryTime);
         xmtrPtr->NotifyListeners(aSimTime, aMessage.GetResult());
      }
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
      // Instantaneous transmission time.
      // Immediately handle end of transmission.
      EndTransmission(aSimTime, status, identifier);
   }

   return result;
}

// =================================================================================================
void Unguided::EndTransmission(double aSimTime, MessageStatus& aMessageStatus, const Message::Identifier& aIdentifier)
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

   // If a layer was indicated, inform that the transmission is complete.
   // NOTE: Candidate for change in AFSIM 3.0
   // Legacy behavior notifies transmission completed at the end of transmission time, but failure
   // is indicated at delivery time (after propagation). This is impossible. Even if not explicitly
   // modelling an ACK for knowledge of a properly sent transmission, and assuming no latency due
   // to receiver processing, this would be twice the message delivery time at best. While its okay
   // to provide such messages since we have full knowledge in what is actually occurring in the simulation,
   // we should not be defining the comm behavior off of such events. This change cannot be made without
   // severely disrupting current event output expectations.
   else if (aMessageStatus.GetLayer() && xmtrPtr && !aMessageStatus.AbortDelivery() && !aMessageStatus.IsStatusIssued())
   {
      LayerEvent::ScheduleCommLayerEvent(aSimTime,
                                         aMessageStatus.GetLayer()->GetCommLayerImp()->GetIndex(),
                                         xmtrPtr,
                                         layer::cUP_ACK_SEND,
                                         &aMessageStatus.GetMessage());
   }

   // Indicate end of transmission.
   aMessageStatus.SetIsTransmitting(false);
   PropagateMessage(aSimTime, aMessageStatus, aIdentifier);
}

// =================================================================================================
void Unguided::EndPropagation(double aSimTime, MessageStatus& aMessageStatus, const Message::Identifier& aIdentifier)
{
   auto& msg = aMessageStatus.GetMessage();
   assert(aIdentifier == Message::Identifier(msg));
   // Get the transmitter.
   const auto& xmtrAddress = msg.GetTraceRoute().back();
   auto        xmtrPtr     = GetSimulation()->GetCommNetworkManager()->GetComm(xmtrAddress);

   // Get the recipient.
   const auto& rcvrAddress = msg.SourceMessage()->GetNextHopAddr();
   auto        rcvrPtr     = GetSimulation()->GetCommNetworkManager()->GetComm(rcvrAddress);

   if (aMessageStatus.AbortDelivery())
   {
      // Notify of failure if layer indicated, transmitter still exists, and status not issued.
      if (aMessageStatus.GetLayer() && xmtrPtr && !aMessageStatus.IsStatusIssued())
      {
         LayerEvent::ScheduleCommLayerEvent(aSimTime,
                                            aMessageStatus.GetLayer()->GetCommLayerImp()->GetIndex(),
                                            xmtrPtr,
                                            layer::cUP_NACK_SEND,
                                            &msg);
      }

      if (mDebug)
      {
         auto out = ut::log::debug() << "Message delivery failure due to abort condition.";
         out.AddNote() << "Source: " << aIdentifier.GetSource();
         out.AddNote() << "Receiver: " << rcvrAddress;
         out.AddNote() << "Message serial: " << aIdentifier.GetSerialNumber();
      }
   }
   else if (rcvrPtr && rcvrPtr->IsTurnedOn())
   {
      rcvrPtr->Receive(aSimTime, xmtrPtr, msg);
   }

   RemoveMessageStatus(aIdentifier);
}

} // namespace medium
} // namespace comm
} // namespace wsf
