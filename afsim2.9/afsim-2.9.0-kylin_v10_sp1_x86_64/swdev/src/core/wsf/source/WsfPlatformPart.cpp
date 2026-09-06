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

#include "WsfPlatformPart.hpp"

#include <cmath>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMemory.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfEvent.hpp"
#include "WsfException.hpp"
#include "WsfGroup.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSimulation.hpp"
#include "WsfStatusMessage.hpp"
#include "WsfStringId.hpp"

// ========================== start sub-class PartRestorationEvent ==========================
class PartRestorationEvent : public WsfEvent
{
public:
   EventDisposition Execute() override;
   PartRestorationEvent(double aSimTime, WsfPlatformPart* aPlatformPartPtr);

protected:
   size_t           mPlatformIndex;
   WsfPlatformPart* mPlatformPartPtr;
};

WsfEvent::EventDisposition PartRestorationEvent::Execute()
{
   WsfEvent::EventDisposition result = cDELETE;
   if (GetSimulation()->PlatformExists(mPlatformIndex))
   {
      mPlatformPartPtr->Restore(GetTime());
   }
   return result;
}

PartRestorationEvent::PartRestorationEvent(double aSimTime, WsfPlatformPart* aPlatformPartPtr)
   : WsfEvent(aSimTime)
   , mPlatformIndex(aPlatformPartPtr->GetPlatform()->GetIndex())
   , mPlatformPartPtr(aPlatformPartPtr)
{
}

// ========================== end sub-class PartRestorationEvent ==========================

// =================================================================================================
WsfPlatformPart::WsfPlatformPart(const WsfScenario& aScenario, int aPartType)
   : WsfObject()
   , mPartType(aPartType)
   , mScenario(aScenario)
   , mPlatformPtr(nullptr)
   , mInternalLinks()
   , mCategories()
   , mGroups()
   , mLastStatusChangeTime(0.0)
   , mAutomaticRecoveryTime(0.0)
   , mDamageFactor(0.0)
   , mIsCritical(false)
   , mIsTurnedOn(false)
   , mInitiallyTurnedOn(true)
   , mIsOperational(true)
   , mIsRestorable(true)
   , mDebugEnabled(false)
   , mIsExternallyControlled(false)
   , mUpdateEventActive(false)
   , mDebugMask(3)
   , mUpdateEventEpoch(1)
   , mInternalMessageQueue()
   , mRecipientMessageQueue()
   , mMutex()
{
}

// =================================================================================================
//! Copy constructor (for Clone()).
// protected
WsfPlatformPart::WsfPlatformPart(const WsfPlatformPart& aSrc)
   : WsfObject(aSrc)
   , WsfAuxDataEnabled(aSrc)
   , mPartType(aSrc.mPartType)
   , mScenario(aSrc.mScenario)
   , mPlatformPtr(nullptr)
   , // Don't copy. Must be reset when attached to platform.
   mInternalLinks(aSrc.mInternalLinks)
   , mCategories(aSrc.mCategories)
   , mGroups(aSrc.mGroups)
   , mLastStatusChangeTime(aSrc.mLastStatusChangeTime)
   , mAutomaticRecoveryTime(aSrc.mAutomaticRecoveryTime)
   , mDamageFactor(aSrc.mDamageFactor)
   , mIsCritical(aSrc.mIsCritical)
   , mIsTurnedOn(aSrc.mIsTurnedOn)
   , mInitiallyTurnedOn(aSrc.mInitiallyTurnedOn)
   , mIsOperational(aSrc.mIsOperational)
   , mIsRestorable(aSrc.mIsRestorable)
   , mDebugEnabled(aSrc.mDebugEnabled)
   , mIsExternallyControlled(aSrc.mIsExternallyControlled)
   , mUpdateEventActive(false)
   , mDebugMask(aSrc.mDebugMask)
   , mUpdateEventEpoch(1)
   , mInternalMessageQueue()
   , mRecipientMessageQueue()
   , mMutex()
{
   // Active objects cannot be cloned (i.e.: those that are part of a running simulation).
   // Cloning was designed to work only for 'type objects' or instances that are part of
   // a type object (such as a sensor that is part of a platform type).

   if ((aSrc.GetPlatform() != nullptr) && (aSrc.GetPlatform()->GetIndex() != 0))
   {
      throw WsfUncloneableException("WsfUncloneableException: object: " + aSrc.GetPlatform()->GetName() + '.' +
                                    aSrc.GetName());
   }
}

// =================================================================================================
// virtual
WsfPlatformPart::~WsfPlatformPart()
{
   mGroups.LeaveAllGroups();
}

const int* WsfPlatformPart::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_PLATFORM_PART, cWSF_COMPONENT_NULL};
   return roles;
}

void* WsfPlatformPart::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_PLATFORM_PART)
   {
      return this;
   }
   return nullptr;
}

// =================================================================================================
//! Set the damage factor of the part.
//! @param aSimTime Current simulation time.
//! @param aDamageFactor The new value for part damage.  Zero is undamaged, one is totally damaged.
//! @returns true if the damage factor changed during the request.
//! @note If the part is critical, setting damage to one will queue a destruction of the platform.
bool WsfPlatformPart::SetDamageFactor(double aSimTime, double aDamageFactor)
{
   // The damage state of this platform part might be set externally.
   // If not mIsRestorable and currently dead, do not allow transition back to life.
   // IsBroken() = (! mIsRestorable) && (mDamageFactor == 1.0));
   // (If identified as not restorable, damage state latches at 1.0 (broken).)

   if (mIsExternallyControlled || IsBroken())
   {
      return false;
   }

   double newDamage = std::min(1.0, std::max(0.0, aDamageFactor));
   if (newDamage == mDamageFactor)
   {
      return false;
   }

   mDamageFactor = newDamage;

   if (mDamageFactor == 1.0)
   {
      // Mortal damage occurred
      // Setting damage to 1.0 AT LEAST temporarily disables the part:
      if (mIsTurnedOn)
      {
         // If Turned On, it is Forced Off.
         TurnOff(aSimTime);
      }

      if (mIsOperational)
      {
         // If Operational, it is Forced Non-Operational.
         SetNonOperational(aSimTime);
      }

      // If not restorable, this part is broken.
      PartBroken(aSimTime);

      if (mIsCritical)
      {
         // If identified as critical, the containing platform is destroyed.
         GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
      }
      else if (mIsRestorable && mAutomaticRecoveryTime > 0.0)
      {
         // Queue an event to restore the part.
         GetSimulation()->AddEvent(ut::make_unique<PartRestorationEvent>(aSimTime + mAutomaticRecoveryTime, this));
      }
   }
   return true;
}

// =================================================================================================
//! Inform the internal systems that this platform is broken.
//! @param aSimTime The current simulation time.
// virtual
void WsfPlatformPart::OnPlatformBrokenEvent(double aSimTime)
{
   if (mIsOperational && (mDamageFactor < 1.0))
   {
      SetOperational(aSimTime);
      if (mInitiallyTurnedOn && mIsTurnedOn)
      {
         TurnOff(aSimTime);
      }
   }
   SetNonOperational(aSimTime);
}

// =================================================================================================
//! Inform the internal systems that this part is broken.
//! @param aSimTime The current simulation time.
// virtual
void WsfPlatformPart::PartBroken(double aSimTime)
{
   // Should be dead _before_ this call is made.
   assert(mDamageFactor == 1.0);

   mLastStatusChangeTime = aSimTime;
   // Send an status message to interested parties.
   // (Their response to this message is likely to take me out of their internal link list.)
   WsfStatusMessage message(UtStringIdLiteral("SYSTEM_BROKEN"), mPlatformPtr);
   message.SetSystemNameId(GetNameId());
   WsfPlatformPart::SendMessage(aSimTime, message);
}

// =================================================================================================
//! Perform process of Restoring a part (return to Operational from non-Operational).  This
//! virtual function will usually be a response to a time-queued event.  If the Damage Factor
//! of the part is totally damaged, it will be set to "slightly" less than totally damaged, to
//! permit the part to become Operational again.
//! @param aSimTime The current simulation time.
// virtual
void WsfPlatformPart::Restore(double aSimTime)
{
   if ((!mIsOperational) && (!mIsExternallyControlled) && (mIsRestorable))
   {
      if (mDamageFactor == 1.0)
      {
         // This is necessary because a part cannot Restore
         // unless damage factor is less than total.
         const double cBARELY_ALIVE = 0.999;
         mDamageFactor              = cBARELY_ALIVE;
      }
      SetOperational(aSimTime);
   }
}

// =================================================================================================
//! Perform phase 1 initialization of the part.
//! This is invoked by WsfPlatform::Initialize() during phase 1 initialization of the platform.
//! During phase 1 initialization, platform components must not assume the existence of ANY
//! data that is the responsibility of another platform component. In particular, the platform
//! location and initial tracks cannot be assumed to be valid.
//! @param aSimTime The current simulation time.
//! @return 'true' if successful or 'false' if some sort of error occurred.
// virtual
bool WsfPlatformPart::Initialize(double aSimTime)
{
   AssignUniqueId(GetSimulation()); // In WsfUniqueId
   mGroups.Initialize(this);
   return mInternalLinks.Initialize(aSimTime, this);
}

// =================================================================================================
//! Perform phase 2 initialization of the part.
//! This is invoked by WsfPlatform::Initialize() during phase 2 initialization of the platform.
//! During phase 2, platform components may assume that the platform location has been defined,
//! initial tracks have been loaded and that all components have completed phase 1 initialization.
//! @param aSimTime The current simulation time.
//! @return 'true' if successful or 'false' if some sort of error occurred.
// virtual
bool WsfPlatformPart::Initialize2(double aSimTime)
{
   return true;
}

// =================================================================================================
//! Join the given group
void WsfPlatformPart::JoinGroup(WsfGroup* aGroupPtr)
{
   mGroups.JoinGroup(aGroupPtr, GetPlatform()->GetIndex(), GetUniqueId());
}

// =================================================================================================
//! Leave the given group
void WsfPlatformPart::LeaveGroup(WsfGroup* aGroupPtr)
{
   mGroups.LeaveGroup(aGroupPtr, GetPlatform()->GetIndex(), GetUniqueId());
}

// =================================================================================================
// returns true if the platform part is a member of the given group
bool WsfPlatformPart::IsGroupMember(WsfStringId aGroupId)
{
   return mGroups.IsGroupMember(aGroupId);
}

// =================================================================================================
//! Update the state of an platform part.
//! Updates the platform part to reflect the state at the specified simulation time.
//! @param aSimTime The current simulation time.
// virtual
void WsfPlatformPart::Update(double /* aSimTime */) {}

// =================================================================================================
//! Some other platform has been added to the simulation.
//!
//! This is invoked (via WsfPlatform::PlatformAdded) whenever another platform
//! has been added to the simulation.
//!
//! @param aSimTime     The current simulation time.
//! @param aPlatformPtr Pointer to the platform being added.
// virtual
void WsfPlatformPart::PlatformAdded(double /* aSimTime */, WsfPlatform* /* aPlatformPtr */) {}

// =================================================================================================
//! Some other platform has been deleted from the simulation.
//!
//! This is invoked (via WsfPlatform::PlatformDeleted) whenever another platform has
//! been deleted from the simulation.
//!
//! @param aPlatformPtr Pointer to the platform being deleted.
// virtual
void WsfPlatformPart::PlatformDeleted(WsfPlatform* /* aPlatformPtr */) {}

// =================================================================================================
// virtual
bool WsfPlatformPart::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "off")
   {
      mInitiallyTurnedOn = false;
   }
   else if (command == "on")
   {
      mInitiallyTurnedOn = true;
   }
   else if (command == "operational")
   {
      mIsOperational = true;
   }
   else if (command == "non_operational")
   {
      mIsOperational = false;
   }
   else if (command == "restorable")
   {
      mIsRestorable = true;
   }
   else if (command == "non_restorable")
   {
      mIsRestorable = false;
   }
   else if (command == "automatic_recovery_time")
   {
      aInput.ReadValueOfType(mAutomaticRecoveryTime, UtInput::cTIME);
      aInput.ValueGreater(mAutomaticRecoveryTime, 0.0);
   }
   else if ((command == "broken") || (command == "dead"))
   {
      mIsOperational = false;
      mIsRestorable  = false;
      mDamageFactor  = 1.0;
   }
   else if (command == "damage_factor")
   {
      aInput.ReadValue(mDamageFactor);
      aInput.ValueInClosedRange(mDamageFactor, 0.0, 1.0);
   }
   else if (command == "critical")
   {
      mIsCritical = true;
   }
   else if (command == "non_critical")
   {
      mIsCritical = false;
   }
   else if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else if (command == "no_debug")
   {
      mDebugEnabled = false;
   }
   else if (command == "debug_level")
   {
      unsigned int debugLevel(0);
      aInput.ReadValue(debugLevel);
      aInput.ValueGreaterOrEqual(debugLevel, 0U);
      mDebugMask = debugLevel;
   }
   else if (mInternalLinks.ProcessInput(aInput))
   {
   }
   else if (mCategories.ProcessInput(aInput))
   {
   }
   else if (mGroups.ProcessInput(aInput, this))
   {
   }
   else if (WsfAuxDataEnabled::ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = WsfObject::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
//! Process a received message.
//! @param aSimTime The current simulation time.
//! @param aMessage The message that is being sent.
//! @returns 'true' if the message was processed or 'false' if not.
//! @note A message is first received by ReceiveMessage which then calls ProcessMessage to process
//! the message. A derived class ReceiveMessage method may choose to delay the message before
//! calling ProcessMessage or may choose to ignore the message altogether.
// virtual
bool WsfPlatformPart::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   if (aMessage.GetType() == WsfStatusMessage::GetTypeId())
   {
      const WsfStatusMessage& message = dynamic_cast<const WsfStatusMessage&>(aMessage);
      if (message.GetStatusId() == UtStringIdLiteral("SYSTEM_BROKEN"))
      {
         return mInternalLinks.RemoveLink(message.GetSystemNameId());
      }
   }
   return false;
}

// =================================================================================================
//! Receive a message from another platform part (system).
//! The default implementation simply calls ProcessMessage to process the received message.
//! @param aSimTime The current simulation time.
//! @param aMessage The message that is being sent.
//! @returns 'true' if the message was received or 'false' if not.
//! @note A message is first received by ReceiveMessage which then calls ProcessMessage to process
//! the message. A derived class ReceiveMessage method may choose to delay the message before
//! calling ProcessMessage or may choose to ignore the message altogether.
// virtual
bool WsfPlatformPart::ReceiveMessage(double aSimTime, const WsfMessage& aMessage)
{
   return ProcessMessage(aSimTime, aMessage);
}

// =================================================================================================
//! Send a message to platform parts (systems) with whom we are linked.
//! @param aSimTime The current simulation time.
//! @param aMessage The message that is being sent.
// virtual
void WsfPlatformPart::SendMessage(double aSimTime, const WsfMessage& aMessage)
{
   if (mInternalLinks.HasLinks())
   {
      if (GetSimulation()->MultiThreadingActive())
      {
         std::lock_guard<std::recursive_mutex> lock(mMutex);
         mInternalMessageQueue.push(aMessage.Clone());
      }
      else
      {
         mInternalLinks.SendMessage(aSimTime, aMessage);
      }
   }
}

// =================================================================================================
//! Send a message to a specific platform part on the same platform.
//! @param aSimTime The current simulation time.
//! @param aMessage The message that is being sent.
//! @param aRecipientId The string ID of the name of the recipient.
// virtual
void WsfPlatformPart::SendMessageTo(double aSimTime, const WsfMessage& aMessage, WsfStringId aRecipientId)
{
   WsfPlatform* platformPtr = GetPlatform();

   // Find the FIRST part with the requested name.
   //
   // This is potentially ambiguous. Names are guaranteed to be unique ONLY within the part type.
   // It is possible to have a sensor named the same as a processor or a sensor to have the name
   // 'mover' or 'fuel' (which designate the mover and fuel parts).

   WsfPlatformPart* recipientPtr = platformPtr->GetComponent<WsfPlatformPart>(aRecipientId);
   if (recipientPtr != nullptr)
   {
      if (recipientPtr->IsTurnedOn())
      {
         if (GetSimulation()->MultiThreadingActive())
         {
            mRecipientMessageQueue.push(std::make_pair(recipientPtr, aMessage.Clone()));
         }
         else
         {
            recipientPtr->ReceiveMessage(aSimTime, aMessage);
         }
      }
   }
}

// =================================================================================================
//! Send a message to a specific processor on the same platform.
//! @param aSimTime The current simulation time.
//! @param aMessage The message that is being sent.
//! @param aRecipientId The string ID of the name of the recipient.
void WsfPlatformPart::SendMessageToProcessor(double aSimTime, const WsfMessage& aMessage, WsfStringId aRecipientId)
{
   WsfProcessor* recipientPtr = GetPlatform()->GetComponent<WsfProcessor>(aRecipientId);
   if (recipientPtr != nullptr)
   {
      if (recipientPtr->IsTurnedOn())
      {
         if (GetSimulation()->MultiThreadingActive())
         {
            mRecipientMessageQueue.push(std::make_pair(recipientPtr, aMessage.Clone()));
         }
         else
         {
            recipientPtr->ReceiveMessage(aSimTime, aMessage);
         }
      }
   }
}

// =================================================================================================
//! Send queued messages from the internal and external message queues.
//! @param aSimTime The current simulation time.
void WsfPlatformPart::SendQueuedMessages(double aSimTime)
{
   while (!mInternalMessageQueue.empty())
   {
      auto messagePtr = mInternalMessageQueue.front();
      WsfPlatformPart::SendMessage(aSimTime, *messagePtr);
      delete messagePtr;
      mInternalMessageQueue.pop();
   }

   while (!mRecipientMessageQueue.empty())
   {
      WsfPlatformPart* recipientPtr = mRecipientMessageQueue.front().first;
      if (recipientPtr->IsTurnedOn())
      {
         auto messagePtr = mRecipientMessageQueue.front().second;
         recipientPtr->ReceiveMessage(aSimTime, *messagePtr);
         delete messagePtr;
      }
      mRecipientMessageQueue.pop();
   }
}

// =================================================================================================
//! Set the operational status of the part.
//! @param aSimTime Current simulation time.
//! @param aMakeOperational true if the part is to be considered operational or false if not.
//! @returns true if the request changed the operational status of the part.
bool WsfPlatformPart::SetOperational(double aSimTime, bool aMakeOperational)
{
   if ((CanBeMadeOperational() && aMakeOperational) || (CanBeMadeNonOperational() && !aMakeOperational))
   {
      if (aMakeOperational)
      {
         SetOperational(aSimTime);
      }
      else
      {
         // Must be turned off just before going non-operational
         if (mIsTurnedOn)
         {
            TurnOff(aSimTime);
         }

         SetNonOperational(aSimTime);
      }
      return true;
   }
   return false;
}

// =================================================================================================
//! Turn off the platform part (system).
//! @param aSimTime The current simulation time.
// virtual
void WsfPlatformPart::TurnOff(double aSimTime)
{
   mLastStatusChangeTime = aSimTime;
   mIsTurnedOn           = false;

   // Send an status message to interested parties.

   WsfStatusMessage message(UtStringIdLiteral("SYSTEM_TURNED_OFF"), mPlatformPtr);
   message.SetSystemNameId(GetNameId());
   WsfPlatformPart::SendMessage(aSimTime, message);
}

// =================================================================================================
//! Turn on the platform part (system).
//! @param aSimTime The current simulation time.
// virtual
void WsfPlatformPart::TurnOn(double aSimTime)
{
   // A part cannot turn on until it is operational.
   assert(mIsOperational);
   assert(mDamageFactor < 1.0);

   mLastStatusChangeTime = aSimTime;
   mIsTurnedOn           = true;

   // Send an status message to interested parties.

   WsfStatusMessage message(UtStringIdLiteral("SYSTEM_TURNED_ON"), mPlatformPtr);
   message.SetSystemNameId(GetNameId());
   WsfPlatformPart::SendMessage(aSimTime, message);
}

// =================================================================================================
//! Set the system status as Non-Operational.
//! @param aSimTime The current simulation time.
// virtual
void WsfPlatformPart::SetNonOperational(double aSimTime)
{
   // A part cannot go non-Op until it is switched off...
   assert(!IsTurnedOn());

   mLastStatusChangeTime = aSimTime;
   mIsOperational        = false;

   // Send an status message to interested parties.

   WsfStatusMessage message(UtStringIdLiteral("SYSTEM_NON_OPERATIONAL"), mPlatformPtr);
   message.SetSystemNameId(GetNameId());
   WsfPlatformPart::SendMessage(aSimTime, message);
}

// =================================================================================================
//! Set the system status as Operational.
//! @param aSimTime The current simulation time.
// virtual
void WsfPlatformPart::SetOperational(double aSimTime)
{
   // Something is wrong if we are broken...
   assert(mDamageFactor < 1.0);

   mLastStatusChangeTime = aSimTime;
   mIsOperational        = true;

   // Send an status message to interested parties.

   WsfStatusMessage message(UtStringIdLiteral("SYSTEM_OPERATIONAL"), mPlatformPtr);
   message.SetSystemNameId(GetNameId());
   WsfPlatformPart::SendMessage(aSimTime, message);
}

// =================================================================================================
void WsfPlatformPart::SetExternallyControlled(double /*aSimTime*/, bool aIsExternallyControlled)
{
   mIsExternallyControlled = aIsExternallyControlled;
}

// =================================================================================================
const char* WsfPlatformPart::GetScriptClassName() const
{
   return "WsfPlatformPart";
}

// =================================================================================================
WsfSimulation* WsfPlatformPart::GetSimulation() const
{
   return mPlatformPtr ? mPlatformPtr->GetSimulation() : nullptr;
}
