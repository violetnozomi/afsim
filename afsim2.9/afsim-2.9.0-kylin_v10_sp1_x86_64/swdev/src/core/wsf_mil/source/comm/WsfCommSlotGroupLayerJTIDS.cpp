// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommSlotGroupLayerJTIDS.hpp"

#include "UtAttributeContainer.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "WsfComm.hpp"
#include "WsfCommMessage.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommObserver.hpp"
#include "WsfCommandChain.hpp"
#include "WsfJTIDS_SlotGroup.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{
namespace comm
{

// ==========================================================================
SlotGroupLayerJTIDS::SlotGroupLayerJTIDS()
   : mSecondsPerSlot(7.8125E-3)
   , mBitsPerSlot(210)
   , mSetsPerFrame(3)
   , mSlotsPerSet(512)
   , mLastReceiveTime(-1.0)
   , mSlotGroups()
   , mCmdChainToGroup()
   , mCallbacks()
{
}

SlotGroupLayerJTIDS::~SlotGroupLayerJTIDS()
{
   mCallbacks.Clear();
   for (auto& slotGroup : mSlotGroups)
   {
      delete slotGroup;
   }
}

// ==========================================================================
wsf::comm::LayerImp* SlotGroupLayerJTIDS::Clone() const
{
   return new SlotGroupLayerJTIDS(*this);
}

// ==========================================================================
bool SlotGroupLayerJTIDS::Initialize(double aSimTime)
{
   bool ok = true;

   // Initialize each slot group and add callbacks
   for (auto& slotGroup : mSlotGroups)
   {
      ok &= slotGroup->Initialize(aSimTime);
      if (ok)
      {
         mCallbacks.Add(slotGroup->MessageSent.Connect(&SlotGroupLayerJTIDS::HandleMessageSent, this));
      }
   }

   // Validate each command chain to slot group mapping
   for (const auto& ccsg : mCmdChainToGroup)
   {
      if (Parent()->GetPlatform()->GetComponent<WsfCommandChain>(ccsg.first) == nullptr)
      {
         auto out = ut::log::error() << "Command Chain is not a valid command chain on this platform.";
         out.AddNote() << "Command Chain: " << ccsg.first;
         ok = false;
      }
   }
   return ok;
}

// ==========================================================================
bool SlotGroupLayerJTIDS::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "slot_group")
   {
      std::string groupName;
      aInput.ReadValue(groupName);
      WsfStringId groupId(groupName);
      if (FindGroup(groupId) != nullptr)
      {
         throw UtInput::BadValue(aInput, "Duplicate slot_group: " + groupName);
      }
      UtInputBlock                        inputBlock(aInput);
      std::unique_ptr<WsfJTIDS_SlotGroup> groupPtr(new WsfJTIDS_SlotGroup(this, groupName, ""));
      while (inputBlock.ReadCommand())
      {
         if (!groupPtr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mSlotGroups.push_back(groupPtr.release());
   }
   else if (command == "relay_slot_group")
   {
      std::string groupName;
      std::string sourceGroupName;
      aInput.ReadValue(groupName);
      aInput.ReadValue(sourceGroupName);
      WsfStringId groupId(groupName);
      if (FindGroup(groupId) != nullptr)
      {
         throw UtInput::BadValue(aInput, "Duplicate relay_slot_group: " + groupName);
      }
      UtInputBlock                        inputBlock(aInput);
      std::unique_ptr<WsfJTIDS_SlotGroup> groupPtr(new WsfJTIDS_SlotGroup(this, groupName, sourceGroupName));
      while (inputBlock.ReadCommand())
      {
         if (!groupPtr->ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
      mSlotGroups.push_back(groupPtr.release());
   }
   else if (command == "command_chain")
   {
      std::string chainName;
      std::string groupName;
      aInput.ReadValue(chainName);
      aInput.ReadValue(groupName);
      WsfStringId chainId       = chainName;
      WsfStringId groupId       = groupName;
      mCmdChainToGroup[chainId] = groupId;
   }
   else if (mSlotGroups.empty())
   {
      // These commands are invalid once a group has been defined as group
      // definitions use these for validation.

      if (command == "time_per_slot")
      {
         aInput.ReadValueOfType(mSecondsPerSlot, UtInput::cTIME);
         aInput.ValueGreater(mSecondsPerSlot, 0.0);
      }
      else if (command == "sets_per_frame")
      {
         aInput.ReadValue(mSetsPerFrame);
         aInput.ValueGreater(mSetsPerFrame, 0);
      }
      else if (command == "slots_per_set")
      {
         aInput.ReadValue(mSlotsPerSet);
         aInput.ValueGreater(mSlotsPerSet, 1);
      }
      else if (command == "slot_size")
      {
         aInput.ReadValueOfType(mBitsPerSlot, UtInput::cDATA_SIZE);
         aInput.ValueGreater(mBitsPerSlot, 0);
      }
      else
      {
         myCommand = false;
      }
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// ============================================================================
//! Receive the message in this layer.
//! @param aSimTime         [input] The current simulation time.
//! @param aXmtrPtr         [input] The comm device that sent the message.
//! @param aMessage         [input] The communication message being received.
//! @param aNotifyNextLayer [output] True if the next layer above this one should immediately process this message.
//! @return 'true' if layer successfully processed the message.
// virtual
bool SlotGroupLayerJTIDS::Receive(double aSimTime, Comm* aXmtrPtr, Message& aMessage, bool& aNotifyNextLayer)
{
   // Check to see if the receiver has already received a message in this time slot.
   //
   // The following check assumes that a message will never be received in the
   // first half of the time slot.  This is a valid assumption because the event
   // scheduling logic will always put receipt near the end of the time slot.

   if ((aSimTime - mLastReceiveTime) < (0.5 * mSecondsPerSlot))
   {
      if (aXmtrPtr->DebugEnabled() || Parent()->DebugEnabled())
      {
         ut::log::debug() << "Message already received in this slot.";
      }
      return false;
   }

   mLastReceiveTime = aSimTime;
   aNotifyNextLayer = true;
   return true;
}

// ============================================================================
//! Send the message to this layer.
//! @param aSimTime         [input] The current simulation time.
//! @param aLayerIndex      [input] The index in the protocol stack for this layer.
//! @param aMessage         [input] The communication message being received.
//! @param aNotifyNextLayer [output] True if the next layer below this one should immediately process this message.
//! @return 'true' if layer successfully processed the message.
// virtual
bool SlotGroupLayerJTIDS::Send(double aSimTime, size_t aLayerIndex, Message& aMessage, bool& aNotifyNextLayer)
{
   // Get the command chain name
   WsfStringId commandChainId(aMessage.GetAuxDataConst().GetString("command_chain"));

   auto recipientComm =
      Parent()->GetSimulation()->GetCommNetworkManager()->GetComm(aMessage.SourceMessage()->GetDstAddr());
   auto recipientPtr = Parent()->GetSimulation()->GetPlatformByName(recipientComm->GetPlatform()->GetNameId());

   // Find the slot group and send the message
   WsfJTIDS_SlotGroup* groupPtr = nullptr;
   if (InitializeSendRequest(aSimTime, commandChainId, groupPtr) &&
       groupPtr->SendMessage(aSimTime, *aMessage.SourceMessage(), recipientPtr))
   {
   }
   else
   {
      WsfObserver::MessageDiscarded(
         Parent()->GetSimulation())(aSimTime, Parent(), *aMessage.SourceMessage(), "unicast_send_failure");
      if (DebugEnabled())
      {
         auto out = ut::log::debug() << "JTIDS Terminal: Message discarded (unicast_send_failure).";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << Parent()->GetPlatform()->GetName();
         out.AddNote() << "WsfCommLayer: " << Parent()->GetName();
      }
   }

   aNotifyNextLayer = true;
   return true;
}

// ============================================================================
//! Process a layer event.
// virtual
bool SlotGroupLayerJTIDS::ProcessLayerMessage(double aSimTime, layer::Message aMessage, Message* aCommMessagePtr)
{
   return false;
}

// ============================================================================
//! Turn off the layer
//! @param aSimTime        [input] The current simulation time.
void SlotGroupLayerJTIDS::TurnOff(double aSimTime)
{
   for (auto& slotGroup : mSlotGroups)
   {
      slotGroup->PurgeQueuedMessages();
   }
}

// ============================================================================
//! Find group with the specified ID.
//! @param aGroupId   The ID of the group to be located.
//! @return The pointer to the group or 0 if it could not be found.
WsfJTIDS_SlotGroup* SlotGroupLayerJTIDS::FindGroup(WsfStringId aGroupId)
{
   for (auto& slotGroup : mSlotGroups)
   {
      if (slotGroup->GetNameId() == aGroupId)
      {
         return slotGroup;
      }
   }
   return nullptr;
}

// ============================================================================
SlotGroupLayerJTIDS::SlotGroupLayerJTIDS(const SlotGroupLayerJTIDS& aSrc)
   : mSecondsPerSlot(aSrc.mSecondsPerSlot)
   , mBitsPerSlot(aSrc.mBitsPerSlot)
   , mSetsPerFrame(aSrc.mSetsPerFrame)
   , mSlotsPerSet(aSrc.mSlotsPerSet)
   , mLastReceiveTime(-1.0)
   , mSlotGroups()
   , mCmdChainToGroup(aSrc.mCmdChainToGroup)
   , mCallbacks()
{
   // Copy the slot groups
   for (auto slotGroup : aSrc.mSlotGroups)
   {
      mSlotGroups.push_back(new WsfJTIDS_SlotGroup(*slotGroup, this));
   }
}

// ============================================================================
//! Initialize a request to send a message.
//! @return 'true'  if OK to continue with the request
//! @return 'false' if the request cannot be satisfied.
bool SlotGroupLayerJTIDS::InitializeSendRequest(double aSimTime, const WsfStringId& aCmdChainId, WsfJTIDS_SlotGroup*& aGroupPtr)
{
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "JTIDS Terminal: Attempting to send message.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << Parent()->GetPlatform()->GetName();
      out.AddNote() << "WsfCommLayer: " << Parent()->GetName();
   }

   // Select the group (NPG) associated with this command chain.
   //
   // If there is an explicit mapping from this command chain to a specific
   // group name then use it.  Otherwise simply use the command chain name
   // as the desired group name.

   WsfStringId chainId = aCmdChainId;
   if (chainId == nullptr)
   {
      chainId = WsfStringId(WsfCommandChain::GetDefaultName());
   }

   WsfStringId wantedGroupId;
   if (chainId != nullptr)
   {
      auto iter = mCmdChainToGroup.find(chainId);
      if (iter != mCmdChainToGroup.end())
      {
         wantedGroupId = iter->second;
      }
   }

   // Find the desired group.  If it doesn't exist then we simply return.

   WsfJTIDS_SlotGroup* groupPtr(FindGroup(wantedGroupId));
   if (groupPtr == nullptr)
   {
      return false;
   }
   aGroupPtr = groupPtr;
   return true;
}

// ============================================================================
void SlotGroupLayerJTIDS::HandleMessageSent(double              aSimTime,
                                            const WsfMessage*   aMessagePtr,
                                            int                 aSlotNumber,
                                            WsfJTIDS_SlotGroup* aSlotGroup)
{
   // Just forward the message on from the slot group
   MessageSent(aSimTime, aMessagePtr, aSlotNumber, aSlotGroup);
}

} // namespace comm
} // namespace wsf
