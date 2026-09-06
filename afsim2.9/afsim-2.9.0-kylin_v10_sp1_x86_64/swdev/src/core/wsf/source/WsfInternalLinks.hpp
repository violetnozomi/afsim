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

#ifndef WSFINTERNALLINKS_HPP
#define WSFINTERNALLINKS_HPP

#include "wsf_export.h"

#include <cassert>
#include <vector>

class UtInput;
class WsfMessage;
class WsfPlatformPart;
#include "WsfStringId.hpp"

//! A connection to one or more platform part over which a message can be sent.
//!
//! This class implements the ability for a platform part (a processor, comm, sensor, etc.)
//! to send a 'message' to another platform part on the same platform.
class WSF_EXPORT WsfInternalLinks
{
public:
   WsfInternalLinks();
   WsfInternalLinks(const WsfInternalLinks& aSrc);
   WsfInternalLinks& operator=(const WsfInternalLinks& aRhs);
   ~WsfInternalLinks();

   //! Have links been established?
   //!   @return 'true' if one or more links have been established;
   //!           'false' if no links have been established.
   //!   @note A caller can use this to determine if anyone might be interested in a message
   //!   before going through the overhead of creating the message.
   bool HasLinks() const { return mHasLinks; }

   bool Initialize(double aSimTime, WsfPlatformPart* aPlatformPartPtr);

   bool ProcessInput(UtInput& aInput);

   void AddRecipient(WsfStringId aRecipientId);
   bool AddRecipient(WsfPlatformPart* aRecipientPtr);

   bool SetStatus(WsfStringId aRecipientId, bool aActive);
   bool RemoveLink(WsfStringId aRecipientId);

   void SendMessage(double aSimTime, const WsfMessage& aMessage);

   void SendMessage(double aSimTime, const WsfMessage& aMessage, WsfStringId aRecipientId);

   //! Return the number of recipients.
   //! @note This is valid only after initialization
   unsigned int GetRecipientCount() const
   {
      assert(mIsInitialized);
      return static_cast<unsigned int>(mRecipientPtrs.size());
   }

   struct Addressee
   {
      Addressee();
      Addressee(WsfStringId aNameId);
      Addressee(const Addressee& aSrc);
      Addressee& operator=(const Addressee& aRhs);
      ~Addressee()       = default;

      bool Activate(WsfPlatformPart* aPartPtr);
      bool Deactivate();

      WsfStringId      mNameId;
      WsfPlatformPart* mPartPtr;
      bool             mIsActive;
   };

   //! Return a pointer to the recipient entry.  If the part link is inactive, will return zero.
   //! @note This is valid only after initialization
   WsfPlatformPart* GetRecipientEntry(unsigned int aEntry) const
   {
      assert(mIsInitialized);
      const Addressee& link = *mRecipientPtrs[aEntry];
      return (link.mIsActive) ? link.mPartPtr : nullptr;
   }

protected:
   void ClearRecipients();

private:
   bool LinkRecipient(WsfStringId aRecipientId);
   bool LinkRecipient(WsfPlatformPart* aRecipientPtr);

   //! The platform part to which this is attached.
   WsfPlatformPart* mPlatformPartPtr;

   //! The collection of potential recipient platform parts for messages from this source.
   std::vector<Addressee*> mRecipientPtrs;

   //! 'true' if link debugging is enabled.
   bool mDebugLinks;

   //! 'true' if the internal links are initialized.
   bool mIsInitialized;

   //! 'true' if some links have been established
   bool mHasLinks;
};

#endif
