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

#ifndef WSFEXTERNALLINKS_HPP
#define WSFEXTERNALLINKS_HPP

#include "wsf_export.h"

#include <string>
#include <unordered_set>
#include <vector>

class UtInput;
#include "UtTypeTraits.hpp"
#include "WsfCommAddress.hpp"
class WsfMessage;
class WsfPlatform;
class WsfSimulation;
#include "WsfStringId.hpp"

namespace wsf
{

namespace comm
{
class Comm;
}; // namespace comm

//! The external link object is a standardized API for designating how an object (typically a platform component)
//! communicates. Any communication between objects in AFSIM that are not local to the same platform are considered
//! external, and require the usage of this object (along with appropriate communication modeling) to enable
//! the transmission and reception of data using an external link. As such, external links do NOT define new linkage
//! between comm objects in AFSIM, but SPECIFY the endpoints of communication to be used to transfer information. Note
//! that what data is sent is based on the implementation of the sending object utilizing external linkage.
//!
//! Note that this class does NOT handle transfer of data local to a platform,
//! (beyond the initial transfer of data to the sending comm) as this is handled by the Internal Link
//! object. Thus, if a user wishes any data received by a comm device to be forwarded to a specific platform component
//! locally, the user should use this class in conjunction with internal links to forward the data to the correct local
//! component.
//!
//! External linkage requires a single transmitter, and one or more receivers (comm objects). Comm objects may be
//! identified in multiple ways, such as a WsfComm object reference, a platform name / comm name pair, or comm address.
//! Multiple receivers may be specified as well, by using the intrinsic comm functionality available via broadcasting,
//! multicasting (if available), or via specification of multiple comm objects via a WsfGroup object. In the case of
//! using a WsfGroup object of comm receivers, the method of comm transport of such messages is multiple unicast
//! transmissions.
//!
//! Note: This class has been modified to utilize the changes to the AFSIM comms framework. Previously, it was sufficient
//! to use the command chain to specify the target of a transmission. This is too ambiguous now, and has modified. While
//! one may still use such commands to specify a particular set of platforms to send to, a commonly named comm must also
//! be specified (that is consistent) across all members of the command chain to continue using command chains in this way.
class WSF_EXPORT ExternalLinks
{
public:
   enum class CommandChainRecipient : unsigned int
   {
      cCOMMANDER,
      cSUBORDINATES,
      cPEERS,
      cNONE
   };

   class ExternalLink;
   using Address     = comm::Address;
   using LinkVec     = std::vector<ExternalLink>;
   using LinkIt      = LinkVec::iterator;
   using LinkConstIt = LinkVec::const_iterator;
   using CommPair    = std::pair<WsfStringId, WsfStringId>;
   using AddressVec  = std::vector<Address>;

   struct CommandChainTarget
   {
      WsfStringId           mChainName;
      CommandChainRecipient mReportType;
      WsfStringId           mRcvrName;

      bool operator==(const CommandChainTarget& aRhs) const
      {
         if ((mChainName == aRhs.mChainName) && (mReportType == aRhs.mReportType) && (mRcvrName == aRhs.mRcvrName))
         {
            return true;
         }

         return false;
      }
   };

   class WSF_EXPORT ExternalLink
   {
   public:
      //! Define hashes for all of the unordered associative containers
      //! using custom key types.
      //@{
      struct CommPairHash
      {
         size_t operator()(const CommPair& aKey) const
         {
            // using multipliers to increase hash entrophy between nearby WsfStringId
            return (std::hash<WsfStringId>()(aKey.first) * 3 + std::hash<WsfStringId>()(aKey.second) * 7);
         }
      };

      struct CommandHash
      {
         size_t operator()(const CommandChainTarget& aKey) const
         {
            // using multipliers to increase hash entrophy between nearby WsfStringId
            return ((std::hash<WsfStringId>()(aKey.mChainName)) * 3 ^
                    (static_cast<ut::underlying_type_t<CommandChainRecipient>>(aKey.mReportType)) ^
                    (std::hash<WsfStringId>()(aKey.mRcvrName))) *
                   7;
         }
      };
      //@}

      using AddressSet   = std::unordered_set<Address>;
      using WsfStringSet = std::unordered_set<WsfStringId>;
      using CommandSet   = std::unordered_set<CommandChainTarget, CommandHash>;
      using CommSet      = std::unordered_set<CommPair, CommPairHash>;

      ExternalLink()          = default;
      virtual ~ExternalLink() = default;

      //! @name Add/Remove methods
      //@{
      bool AddAddressRecipient(const Address& aAddress);
      bool AddGroupRecipient(WsfStringId aGroupName);
      bool AddCommandRecipient(const CommandChainTarget& aCommandChainTarget);
      bool AddCommRecipient(const CommPair& aCommPair);

      bool RemoveAddressRecipient(const Address& aAddress);
      bool RemoveGroupRecipient(WsfStringId aGroupName);
      bool RemoveCommandRecipient(const CommandChainTarget& aCommandChainTarget);
      bool RemoveCommRecipient(const CommPair& aCommPair);
      //@}

      void DeleteAllRecipients();

      //! @note Do not use these calls prior to simulation pending start.
      //! They require full initialization of the comm framework to resolve addressing.
      //@{
      AddressVec  GetRecipients(bool aNotify = false) const;
      comm::Comm* GetXmtr() const;
      size_t      GetRecipientCount() const;
      //@}

      WsfStringId GetXmtrName() const { return mXmtrName; }
      void        SetXmtrName(WsfStringId aXmtrName) { mXmtrName = aXmtrName; }
      void        SetParent(ExternalLinks* aParentPtr) { mParentPtr = aParentPtr; }

   protected:
      WsfStringId    mXmtrName{};
      AddressSet     mAddressRecipients{};
      WsfStringSet   mGroups{};
      CommandSet     mCommandRecipients{};
      CommSet        mCommRecipients{};
      ExternalLinks* mParentPtr{nullptr};
   };

   ExternalLinks()          = default;
   virtual ~ExternalLinks() = default;

   bool HasLinks() const { return (!mLinks.empty()); }

   bool Initialize(double aSimTime, WsfPlatform* aPlatformPtr);

   bool ProcessInput(UtInput& aInput);
   bool CommandChainCommand(const std::string& aCommand, CommandChainRecipient& aReportType);

#undef SendMessage // Avoid conflict with Windows macro
   void SendMessage(double aSimTime, const WsfMessage& aMessage);

   //! Legacy method used by the track manager to avoid reporting
   //! tracks back to the last contributor.
   void SendMessage(double aSimTime, const WsfMessage& aMessage, WsfStringId aExcludedPlatformName);

   WsfSimulation* GetSimulation() const;
   WsfPlatform*   GetPlatform() const { return mPlatformPtr; }

   //! @name Recipient management methods.
   //@{
   bool AddRecipient(WsfStringId aXmtrName, const Address& aRcvrAddress);
   bool AddRecipient(WsfStringId aXmtrName, WsfStringId aCommGroupName);
   bool AddRecipient(WsfStringId aXmtrName, const CommandChainTarget& aCommandChainTarget);
   bool AddRecipient(WsfStringId aXmtrName, const CommPair& aCommPair);
   bool RemoveRecipient(WsfStringId aXmtrName, const Address& aRcvrAddress);
   bool RemoveRecipient(WsfStringId aXmtrName, WsfStringId aCommGroupName);
   bool RemoveRecipient(WsfStringId aXmtrName, const CommandChainTarget& aCommandChainTarget);
   bool RemoveRecipient(WsfStringId aXmtrName, const CommPair& aCommPair);

   void DeleteAllRecipients();

   //! Note: Do not use these calls prior to simulation pending start.
   //! They require full initialization of the comm framework to resolve addressing.
   size_t GetRecipientCount() const;
   size_t GetRecipientCount(WsfStringId aXmtrName) const;
   //@}

   size_t        GetLinkCount() const { return mLinks.size(); }
   ExternalLink& GetLink(size_t aIndex) { return mLinks.at(aIndex); }

protected:
   LinkIt      FindLink(WsfStringId aXmtrName);
   LinkConstIt FindLink(WsfStringId aXmtrName) const;

   LinkVec      mLinks{};
   WsfPlatform* mPlatformPtr{nullptr};
   bool         mDebug{false};
};

} // namespace wsf
#endif
