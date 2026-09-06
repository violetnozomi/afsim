// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommComponentHW.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfComm.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommResult.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"


namespace wsf
{
namespace comm
{

// =================================================================================================
//! Find the instance of this component attached to the specified processor.
ComponentHW* ComponentHW::Find(const Comm& aParent)
{
   ComponentHW* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<ComponentHW>(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the instance of this component attached to the specified processor,
//! and create it if it doesn't exist.
ComponentHW* ComponentHW::FindOrCreate(Comm& aParent)
{
   ComponentHW* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      componentPtr = new ComponentHW;
      aParent.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// =================================================================================================
ComponentHW::ComponentHW()
   : Component()
   , mAntennaPtr(ut::make_unique<WsfEM_Antenna>())
   , mXmtrPtr(ut::make_unique<WsfEM_Xmtr>(WsfEM_Xmtr::cXF_COMM, mAntennaPtr.get()))
   , mRcvrPtr(ut::make_unique<WsfEM_Rcvr>(WsfEM_Rcvr::cRF_COMM, mAntennaPtr.get()))
   , mAltFreqChangeScheduled(false)
   , mAltFreqSelectDelay(0.0)
   , mLastAltFreqSelectTime(0.0)
   , mTransmitsContinuously(false)
   , mZoneAttenuationModifier(nullptr)
{
   // Indicate the receiver is 'linked' with the transmitter
   mXmtrPtr->SetLinkedReceiver(mRcvrPtr.get());

   // If the transmitter power is less than a 1.0e-30 watt, ReceiveMessage will only consider
   // geometry in determining if it can receive the message.  This is for compatibility
   // with old files that did not specify power.
   mXmtrPtr->SetPower(1.0E-30);
}

// =================================================================================================
ComponentHW::ComponentHW(const ComponentHW& aSrc)
   : Component(aSrc)
   , mAntennaPtr(ut::make_unique<WsfEM_Antenna>(*aSrc.mAntennaPtr))
   , mXmtrPtr(ut::make_unique<WsfEM_Xmtr>(*aSrc.mXmtrPtr, mAntennaPtr.get()))
   , mRcvrPtr(ut::make_unique<WsfEM_Rcvr>(*aSrc.mRcvrPtr, mAntennaPtr.get()))
   , mAltFreqChangeScheduled(aSrc.mAltFreqChangeScheduled)
   , mAltFreqSelectDelay(aSrc.mAltFreqSelectDelay)
   , mLastAltFreqSelectTime(aSrc.mLastAltFreqSelectTime)
   , mTransmitsContinuously(aSrc.mTransmitsContinuously)
   , mZoneAttenuationModifier(aSrc.mZoneAttenuationModifier)
{
   // Indicate the receiver is 'linked' with the transmitter
   mXmtrPtr->SetLinkedReceiver(mRcvrPtr.get());
}

// =================================================================================================
WsfComponent* ComponentHW::CloneComponent() const
{
   return new ComponentHW(*this);
}

// =================================================================================================
WsfStringId ComponentHW::GetComponentName() const
{
   return UtStringIdLiteral("comm_component_hw");
}

// =================================================================================================
const int* ComponentHW::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_HW, cWSF_COMPONENT_COMM, cWSF_COMPONENT_NULL};
   return roles;
}

// =================================================================================================
void* ComponentHW::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_HW)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_COMM)
   {
      return dynamic_cast<Component*>(this);
   }
   return nullptr;
}

// =================================================================================================
// virtual
bool ComponentHW::Initialize(double aSimTime)
{
   bool ok = true;

   // If this is a transmit/receive device then we allow certain attributes
   // (frequency in particular) to be specified in one or
   // the other and we'll automatically propagate it to the other if it wasn't
   // specified.  This simplifies user input...

   if (GetComm()->GetCommType() == Comm::cCT_XMT_RCV)
   {
      if (mRcvrPtr->GetFrequency() == 0.0)
      {
         mRcvrPtr->SetFrequency(mXmtrPtr->GetFrequency());
      }
      else if (mXmtrPtr->GetFrequency() == 0.0)
      {
         mXmtrPtr->SetFrequency(mRcvrPtr->GetFrequency());
      }
   }

   // Initialize the antenna, transmitter and receiver.
   ok &= mAntennaPtr->Initialize(GetComm());
   if (GetComm()->CanReceive())
   {
      ok &= mRcvrPtr->Initialize(*GetSimulation());
   }

   if (GetComm()->CanSend())
   {
      ok &= mXmtrPtr->Initialize(*GetSimulation());
   }

   return ok;
}

// =================================================================================================
// virtual
bool ComponentHW::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "modifier_category")
   {
      std::string category;
      aInput.ReadValue(category);
      mZoneAttenuationModifier = category;
   }
   else if (command == "transmit_mode")
   {
      std::string transmitMode;
      aInput.ReadValue(transmitMode);
      if (transmitMode == "continuous")
      {
         mTransmitsContinuously = true;
      }
      else if (transmitMode == "intermittent")
      {
         mTransmitsContinuously = false;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Invalid transmit_mode: " + transmitMode);
      }
   }
   else if (command == "frequency_select_delay")
   {
      aInput.ReadValueOfType(mAltFreqSelectDelay, UtInput::cTIME);
   }
   else if (mAntennaPtr->ProcessInput(aInput))
   {
   }
   else if (GetComm()->CanSend() && mXmtrPtr->ProcessInputBlock(aInput))
   {
   }
   else if (GetComm()->CanReceive() && mRcvrPtr->ProcessInputBlock(aInput))
   {
   }
   // Allow un-delimited transmitter commands only for transmit-only devices.
   // Transmit/receive devices must use explicit transmitter/end_transmitter
   // and receiver/end_receiver blocks.
   else if (GetComm()->CanSend() && (!GetComm()->CanReceive()) && mXmtrPtr->ProcessInput(aInput))
   {
   }
   // Allow un-delimited receiver commands only for receive-only devices
   // Transmit/receive devices must use explicit transmitter/end_transmitter
   // and receiver/end_receiver blocks.
   else if (GetComm()->CanReceive() && (!GetComm()->CanSend()) && mRcvrPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = Component::ProcessInput(aInput);
   }

   return myCommand;
}

// =================================================================================================
// virtual
void ComponentHW::TurnOff(double aSimTime)
{
   // Indicate the receiver and transmitter are not active.
   if (GetComm()->CanReceive())
   {
      mRcvrPtr->Deactivate();
   }
   if (GetComm()->CanSend())
   {
      mXmtrPtr->Deactivate();
   }
}

// =================================================================================================
// virtual
void ComponentHW::TurnOn(double aSimTime)
{
   // Indicate the receiver and transmitter are active.
   if (GetComm()->CanReceive())
   {
      mRcvrPtr->Activate();
   }
   if (GetComm()->CanSend())
   {
      mXmtrPtr->Activate();
   }
}

// =================================================================================================
// virtual
bool ComponentHW::CanInteractWith(const Comm* aXmtrCommPtr)
{
   if (GetComm()->GetClassId() == aXmtrCommPtr->GetClassId())
   {
      ComponentHW* xmtrHW_Ptr = ComponentHW::Find(*aXmtrCommPtr);
      if (xmtrHW_Ptr != nullptr)
      {
         if (mRcvrPtr->CanInteractWith(xmtrHW_Ptr->mXmtrPtr.get()))
         {
            return true;
         }
      }
   }
   return false;
}

// =================================================================================================
// virtual
bool ComponentHW::CanSendTo(double aSimTime, Comm* aRcvrPtr, Result& aResult)
{
   bool canSendTo = false;
   aResult.SetCategory(mZoneAttenuationModifier);

   // Get the hardware component from the other comm device
   ComponentHW* rcvrHW_Ptr = ComponentHW::Find(*aRcvrPtr);
   if (rcvrHW_Ptr != nullptr)
   {
      WsfEM_Rcvr& rcvr = rcvrHW_Ptr->GetEM_Rcvr(0);
      rcvr.UpdatePosition(aSimTime);      // Ensure the receiver position is current
      mXmtrPtr->UpdatePosition(aSimTime); // Ensure the transmitter position is current

      if (aResult.BeginOneWayInteraction(mXmtrPtr.get(), &rcvr, true, true) == 0)
      {
         if (mXmtrPtr->GetPower() > 1.0E-30) // See comment in constructor
         {
            aResult.SetTransmitterBeamPosition();
            aResult.SetReceiverBeamPosition();
            aResult.ComputeRF_OneWayPower();

            // Let components update the results.
            Component::AttemptToTransmit(*GetComm(), aSimTime, aResult);
            Component::AttemptToReceive(*aRcvrPtr, aSimTime, aResult);

            aResult.mSignalToNoise =
               rcvr.ComputeSignalToNoise(aResult.mRcvdPower, aResult.mClutterPower, aResult.mInterferencePower);

            aResult.mCheckedStatus = Result::cSIGNAL_LEVEL;
            if ((aResult.mSignalToNoise >= rcvr.GetDetectionThreshold()) &&
                (aResult.mInterferenceFactor < 0.5)) // TODO_JAJ How should we really do this?
            {
               // Signal level success
               canSendTo = true;
            }
            else
            {
               aResult.mFailedStatus |= Result::cSIGNAL_LEVEL;
            }
         }
         else
         {
            canSendTo = true;
         }
      }

      // Check for terrain masking if we have made it this far...
      if (canSendTo && aResult.MaskedByTerrain())
      {
         canSendTo = false;
      }
   }

   if (GetComm()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Comm Component HW CanSendTo Result:";
      aResult.Print(out);
      if (aResult.mFailedStatus == 0)
      {
         // Revisit after addressing WsfEM_Interaction
         out.AddNote() << "S/N Req: " << UtMath::LinearToDB(mRcvrPtr->GetDetectionThreshold()) << " dB";
         out.AddNote() << "Sent: " << canSendTo;
      }
   }

   return canSendTo;
}

// ============================================================================
// virtual
void ComponentHW::ScheduleAltFreqChange(double aSimTime, int aAltFreqId /*= -1*/)
{
   double changeTime = std::max(aSimTime, mLastAltFreqSelectTime) + mAltFreqSelectDelay;
   GetSimulation()->AddEvent(ut::make_unique<AltFrequencyChangeEvent>(changeTime, GetComm(), aAltFreqId));
   mAltFreqChangeScheduled = true;
}

// ============================================================================
//! Select an alternate frequency for the comm.
//! @param aSimTime   The current simulation time.
//! @param aAltFreqId (optional) The ID of the frequency to change to.
// virtual
void ComponentHW::SelectAlternateFrequency(double aSimTime, int aAltFreqId /*= -1*/)
{
   if (GetEM_XmtrCount() > 0)
   {
      int id = aAltFreqId;
      if (id < 0)
      {
         id = GetEM_Xmtr(0).GetCurrentAlternateFrequencyId() + 1;
      }
      GetEM_Xmtr(0).SelectAlternateFrequency(id);
      GetEM_Xmtr(0).NotifyChangeListeners(aSimTime, GetPlatform()->GetIndex());
      WsfObserver::CommFrequencyChanged(GetSimulation())(aSimTime, GetComm());
      mLastAltFreqSelectTime  = aSimTime;
      mAltFreqChangeScheduled = false;
   }
}

// ============================================================================
// Nested class SelectAltFrequencyEvent
// ============================================================================

//! Alternate frequency selection event that changes a comm devices frequency.
ComponentHW::AltFrequencyChangeEvent::AltFrequencyChangeEvent(double aSimTime, Comm* aCommPtr, int aAltFrequencyId /*= -1*/)
   : WsfEvent(aSimTime)
   , mCommPtr(aCommPtr)
   , mAltFrequencyId(aAltFrequencyId)
{
   mPlatformIndex = mCommPtr->GetPlatform()->GetIndex();
}

// ============================================================================
WsfEvent::EventDisposition ComponentHW::AltFrequencyChangeEvent::Execute()
{
   WsfPlatform* platformPtr = GetSimulation()->GetPlatformByIndex(mPlatformIndex);
   if (platformPtr != nullptr)
   {
      if (mCommPtr != nullptr)
      {
         ComponentHW* commHW_Ptr = ComponentHW::Find(*mCommPtr);
         if (commHW_Ptr != nullptr)
         {
            commHW_Ptr->SelectAlternateFrequency(GetTime(), mAltFrequencyId);
         }
      }
   }
   return cDELETE;
}

} // namespace comm
} // namespace wsf
