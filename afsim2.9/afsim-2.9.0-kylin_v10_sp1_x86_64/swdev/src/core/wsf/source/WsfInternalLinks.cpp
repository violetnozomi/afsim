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

#include "WsfInternalLinks.hpp"

#include <cassert>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfFuel.hpp"
#include "WsfMessage.hpp"
#include "WsfMover.hpp"
#include "WsfPlatform.hpp"
#include "WsfPlatformPart.hpp"

// =================================================================================================
WsfInternalLinks::WsfInternalLinks()
   : mPlatformPartPtr(nullptr)
   , mRecipientPtrs()
   , mDebugLinks(false)
   , mIsInitialized(false)
   , mHasLinks(false)
{
}

// =================================================================================================
//! Copy constructor.
WsfInternalLinks::WsfInternalLinks(const WsfInternalLinks& aSrc)
   : mPlatformPartPtr(nullptr)
   , mRecipientPtrs()
   , mDebugLinks(aSrc.mDebugLinks)
   , mIsInitialized(false)
   , mHasLinks(aSrc.mHasLinks)
{
   for (size_t i = 0; i < aSrc.mRecipientPtrs.size(); ++i)
   {
      mRecipientPtrs.push_back(new Addressee(*aSrc.mRecipientPtrs[i]));
   }
}

// =================================================================================================
//! Assignment constructor.
WsfInternalLinks& WsfInternalLinks::operator=(const WsfInternalLinks& aRhs)
{
   if (this != &aRhs)
   {
      mPlatformPartPtr = nullptr;
      for (size_t i = 0; i < aRhs.mRecipientPtrs.size(); ++i)
      {
         mRecipientPtrs.push_back(new Addressee(*aRhs.mRecipientPtrs[i]));
      }
      mDebugLinks    = aRhs.mDebugLinks;
      mIsInitialized = false;
      mHasLinks      = aRhs.mHasLinks;
   }
   return *this;
}

WsfInternalLinks::~WsfInternalLinks()
{
   ClearRecipients();
}

// =================================================================================================
//! Initialize the links to the requested platform parts.
//! @note This function used to return false if any named links failed,
//!       but now merely prints a warning message, as derived classes sometimes delete certain parts.
//! @param aSimTime         The current simulation time.
//! @param aPlatformPartPtr The platform part pointer to which this is attached.
//! @return 'true'.
bool WsfInternalLinks::Initialize(double aSimTime, WsfPlatformPart* aPlatformPartPtr)
{
   mIsInitialized = true;

   mPlatformPartPtr = aPlatformPartPtr;

   // Locate the recipients to whom a link is to be established.
   //
   // Links are not established for externally controlled platforms their sensors, comms, etc. exist only
   // to stimulate local sensors.
   if (!mPlatformPartPtr->GetPlatform()->IsExternallyControlled())
   {
      for (size_t i = 0; i < mRecipientPtrs.size(); ++i)
      {
         /*mIsInitialized &=*/LinkRecipient(mRecipientPtrs[i]->mNameId);
      }
   }

   return mIsInitialized;
}

// =================================================================================================
//! Add a link to the platform part with the given name.
void WsfInternalLinks::AddRecipient(WsfStringId aRecipientId)
{
   mHasLinks = true;
   if (mIsInitialized)
   {
      LinkRecipient(aRecipientId);
   }
   else
   {
      // Avoid duplicate links
      bool found = false;
      for (size_t i = 0; i < mRecipientPtrs.size(); ++i)
      {
         if (aRecipientId == mRecipientPtrs[i]->mNameId)
         {
            found = true;
            break;
         }
      }

      // Check before adding
      if (!found)
      {
         mRecipientPtrs.push_back(new Addressee(aRecipientId));
      }
   }
}

// =================================================================================================
//! Add a link to the platform part
//! @return 'true' if the link was successfully created
bool WsfInternalLinks::AddRecipient(WsfPlatformPart* aRecipientPtr)
{
   bool ok = true;
   if (mIsInitialized)
   {
      ok = LinkRecipient(aRecipientPtr);
   }
   else
   {
      AddRecipient(aRecipientPtr->GetNameId());
   }
   return ok;
}

// =================================================================================================
//! Activate or deactivate sending to the specified platform part.
//! @note Method to be used only during run time.
//! @param aRecipientId The desired recipient to change send status for.
//! @param aActive True if the desire is to enable sending, false if disabling.
//! @return 'true' If the active status was changed to the desired status.
bool WsfInternalLinks::SetStatus(WsfStringId aRecipientId, bool aActive)
{
   bool changed = false;
   if (mIsInitialized)
   {
      for (size_t i = 0; i != mRecipientPtrs.size(); ++i)
      {
         if (mRecipientPtrs[i]->mNameId == aRecipientId)
         {
            changed                      = (aActive != mRecipientPtrs[i]->mIsActive);
            mRecipientPtrs[i]->mIsActive = aActive;
            break;
         }
      }
   }
   return changed;
}

// =================================================================================================
//! Take out the link to the specified platform part.  The effect will be permanent.
//! @param aRecipientId The recipient to remove.
bool WsfInternalLinks::RemoveLink(WsfStringId aRecipientId)
{
   std::vector<Addressee*>::iterator i = mRecipientPtrs.begin();
   while (i != mRecipientPtrs.end())
   {
      if ((*i)->mNameId == aRecipientId)
      {
         mRecipientPtrs.erase(i);
         return true;
      }
      ++i;
   }
   return false;
}

void WsfInternalLinks::ClearRecipients()
{
   for (auto recipientPtr : mRecipientPtrs)
   {
      delete recipientPtr;
   }
   mRecipientPtrs.clear();
}

// =================================================================================================
//! Link to a platform part with the given name.
bool WsfInternalLinks::LinkRecipient(WsfStringId aRecipientId)
{
   bool ok = true;

   bool   nameAlreadyLinked = false;
   size_t i                 = 0;
   for (; i < mRecipientPtrs.size(); ++i)
   {
      if (aRecipientId == mRecipientPtrs[i]->mNameId)
      {
         nameAlreadyLinked = true;
         break;
      }
   }

   // Find the FIRST part with the requested name.
   //
   // This is potentially ambiguous. Names are guaranteed to be unique ONLY within the part type.
   // It is possible to have a sensor named the same as a processor or a sensor to have the name
   // 'mover' or 'fuel' (which designate the mover and fuel parts).

   WsfPlatform*     platformPtr  = mPlatformPartPtr->GetPlatform();
   WsfPlatformPart* recipientPtr = platformPtr->GetComponent<WsfPlatformPart>(aRecipientId);
   if (recipientPtr == nullptr)
   {
      auto out = ut::log::warning() << "Platform part unable to link.";
      out.AddNote() << "Platform: " << platformPtr->GetName();
      out.AddNote() << "Part: " << mPlatformPartPtr->GetName();
      out.AddNote() << "Recipient: " << aRecipientId;
      ok = false;
   }
   else if (recipientPtr == mPlatformPartPtr)
   {
      auto out = ut::log::error() << "Platform part cannot have an internal_link to itself.";
      out.AddNote() << "Platform: " << platformPtr->GetName();
      out.AddNote() << "Part: " << mPlatformPartPtr->GetName();
      ok = false;
   }
   else if (nameAlreadyLinked)
   {
      ok = mRecipientPtrs[i]->Activate(recipientPtr);
   }
   else
   {
      ok = LinkRecipient(recipientPtr);
   }

   return ok;
}

// =================================================================================================
//! Add a link to the platform part
//! @return 'true' if the link was successfully created
bool WsfInternalLinks::LinkRecipient(WsfPlatformPart* aRecipientPtr)
{
   bool ok = true;

   // Avoid duplicate links
   for (size_t i = 0; i < mRecipientPtrs.size(); ++i)
   {
      if (aRecipientPtr == mRecipientPtrs[i]->mPartPtr)
      {
         ok = false;
      }
   }

   if (ok)
   {
      if (aRecipientPtr == mPlatformPartPtr)
      {
         auto out = ut::log::error() << "Platform part cannot have an internal_link to itself.";
         out.AddNote() << "Platform: " << mPlatformPartPtr->GetPlatform()->GetName();
         out.AddNote() << "Part: " << mPlatformPartPtr->GetName();
         ok = false;
      }
      else
      {
         if (aRecipientPtr->GetPlatform() != mPlatformPartPtr->GetPlatform())
         {
            auto out = ut::log::error() << "Platform part cannot have an internal_link to other platforms.";
            out.AddNote() << "Owning Platform: " << mPlatformPartPtr->GetPlatform()->GetName();
            out.AddNote() << "Part: " << mPlatformPartPtr->GetName();
            out.AddNote() << "Target Platform: " << aRecipientPtr->GetPlatform()->GetName();
         }
         else
         {
            mHasLinks = true;
            mRecipientPtrs.push_back(new Addressee(aRecipientPtr->GetNameId()));
            // NOTE: In the case that the Addressee object has not been initialized,
            //       the constructor for Addressee does not contain the pointer;
            //       Activating the recipient will set the pointer.
            mRecipientPtrs.back()->Activate(aRecipientPtr);
         }
      }
   }
   return ok;
}

// =================================================================================================
//! Process input from a generic source.
//!
//! Examine the current input command.  If it is NOT one of the commands
//! recognized by this class then it simply returns 'false'.  If it is
//! one of the commands recognized by this class then it processes the
//! command and returns 'true'.
//!
//! @param aInput A generic input.
//!
//! @returns 'true' if the command was one recognized by the class or 'false' if not.
bool WsfInternalLinks::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if ((command == "internal_link") || (command == "processor"))
   {
      std::string recipientName;
      aInput.ReadValue(recipientName);
      AddRecipient(recipientName);
   }
   else if (command == "clear_internal_links")
   {
      ClearRecipients();
   }
   else if (command == "debug_internal_links")
   {
      mDebugLinks = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Send a message to a list of registered platform parts.
//! @param aSimTime The current simulation time.
//! @param aMessage The message to be sent.
//! @note The WsfProcessor::ReceiveMessage method of all platform parts previously
//! registered will be called to receive the message.
#undef SendMessage // Avoid conflict with Windows macro
void WsfInternalLinks::SendMessage(double aSimTime, const WsfMessage& aMessage)
{
   if (mPlatformPartPtr->GetPlatform()->IsDeleted())
   {
      return;
   }

   for (size_t i = 0; i < mRecipientPtrs.size(); ++i)
   {
      // Test for zero is in case the recipient was DeLinked when broken.
      if (mRecipientPtrs[i]->mIsActive && mRecipientPtrs[i]->mPartPtr->IsTurnedOn())
      {
         if (mDebugLinks)
         {
            auto out = ut::log::debug() << "Sending internal message.";
            out.AddNote() << "T = " << aSimTime;
            out.AddNote() << "Platform: " << mPlatformPartPtr->GetPlatform()->GetName();
            out.AddNote() << "Sender Part: " << mPlatformPartPtr->GetName();
            out.AddNote() << "Receiver Part: " << mRecipientPtrs[i]->mPartPtr->GetName();
            out.AddNote() << "Message Type: " << aMessage.GetType();
            out.AddNote() << "Message Subtype: " << aMessage.GetSubType();
         }
         mRecipientPtrs[i]->mPartPtr->ReceiveMessage(aSimTime, aMessage);
      }
   }
}

// =================================================================================================
void WsfInternalLinks::SendMessage(double aSimTime, const WsfMessage& aMessage, WsfStringId aRecipientId)
{
   if (mPlatformPartPtr->GetPlatform()->IsDeleted())
   {
      return;
   }

   for (size_t index = 0; index < mRecipientPtrs.size(); ++index)
   {
      if (aRecipientId == mRecipientPtrs[index]->mNameId)
      {
         if (mRecipientPtrs[index]->mIsActive && mRecipientPtrs[index]->mPartPtr->IsTurnedOn())
         {
            if (mDebugLinks)
            {
               auto out = ut::log::debug() << "Sending internal message.";
               out.AddNote() << "T = " << aSimTime;
               out.AddNote() << "Platform: " << mPlatformPartPtr->GetPlatform()->GetName();
               out.AddNote() << "Sender Part: " << mPlatformPartPtr->GetName();
               out.AddNote() << "Receiver Part: " << mRecipientPtrs[index]->mPartPtr->GetName();
               out.AddNote() << "Message Type: " << aMessage.GetType();
               out.AddNote() << "Message Subtype: " << aMessage.GetSubType();
            }
            mRecipientPtrs[index]->mPartPtr->ReceiveMessage(aSimTime, aMessage);
         }
         break;
      }
   }
}

// =================================================================================================
// Nested class 'Addressee'.
// =================================================================================================

WsfInternalLinks::Addressee::Addressee()
   : mNameId()
   , mPartPtr(nullptr)
   , mIsActive(false)
{
}

// =================================================================================================
WsfInternalLinks::Addressee::Addressee(WsfStringId aNameId)
   : mNameId(aNameId)
   , mPartPtr(nullptr)
   , mIsActive(false)
{
}

// =================================================================================================
//! Copy constructor.
//! Copy the NameId, but do not copy any pointer attributes, as they will not likely be valid in the
//! destination part.  The destination part will complete its links at Initialization time.
WsfInternalLinks::Addressee::Addressee(const Addressee& aSrc)
   : mNameId(aSrc.mNameId)
   , mPartPtr(nullptr)
   , mIsActive(false)
{
}

// =================================================================================================
//! Copy constructor.
//! Copy the NameId, but do not copy any pointer attributes, as they will not likely be valid in the
//! destination part.  The destination part will complete its links at Initialization time.
WsfInternalLinks::Addressee& WsfInternalLinks::Addressee::operator=(const Addressee& aRhs)
{
   if (this != &aRhs)
   {
      mNameId   = aRhs.mNameId;
      mPartPtr  = nullptr;
      mIsActive = false;
   }
   return *this;
}

// =================================================================================================
//! Attempt to activate a part name link, by (if needed) obtaining a pointer to the part.
//! @param aPartPtr A pointer to the desired part to link to.
//! @return "true" if the status of the link actually had to be changed to the requested status.
bool WsfInternalLinks::Addressee::Activate(WsfPlatformPart* aPartPtr)
{
   bool activated = false;
   if (mNameId == aPartPtr->GetNameId())
   {
      // Previously, the part was linked by name only, with no valid pointer.
      assert(mPartPtr == nullptr);
      activated = (mIsActive == false);
      mPartPtr  = aPartPtr;
      mIsActive = true;
   }
   else
   {
      throw UtException("Invalid internal link part name.");
   }
   return activated;
}

// =================================================================================================
//! Deactivate a part name link.
//! @return "true" if the status of the link actually had to be changed to the requested status.
bool WsfInternalLinks::Addressee::Deactivate()
{
   bool deactivated = mIsActive;
   mIsActive        = false;
   return deactivated;
}
