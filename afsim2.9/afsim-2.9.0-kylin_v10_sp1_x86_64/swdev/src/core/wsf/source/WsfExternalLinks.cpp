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

#include "WsfExternalLinks.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "WsfComm.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommandChain.hpp"
#include "WsfComponentList.hpp"
#include "WsfGroup.hpp"
#include "WsfGroupManager.hpp"
#include "WsfMessage.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace wsf
{

// =================================================================================================
bool ExternalLinks::ExternalLink::AddAddressRecipient(const Address& aAddress)
{
   return mAddressRecipients.insert(aAddress).second;
}

// =================================================================================================
bool ExternalLinks::ExternalLink::AddGroupRecipient(WsfStringId aGroupName)
{
   return mGroups.insert(aGroupName).second;
}

// =================================================================================================
bool ExternalLinks::ExternalLink::AddCommandRecipient(const CommandChainTarget& aCommandChainTarget)
{
   return mCommandRecipients.insert(aCommandChainTarget).second;
}

// =================================================================================================
bool ExternalLinks::ExternalLink::AddCommRecipient(const CommPair& aCommPair)
{
   return mCommRecipients.insert(aCommPair).second;
}

// =================================================================================================
bool ExternalLinks::ExternalLink::RemoveAddressRecipient(const Address& aAddress)
{
   return (mAddressRecipients.erase(aAddress) == 1U);
}

// =================================================================================================
bool ExternalLinks::ExternalLink::RemoveGroupRecipient(WsfStringId aGroupName)
{
   return (mGroups.erase(aGroupName) == 1U);
}

// =================================================================================================
bool ExternalLinks::ExternalLink::RemoveCommandRecipient(const CommandChainTarget& aCommandChainTarget)
{
   return (mCommandRecipients.erase(aCommandChainTarget) == 1U);
}

// =================================================================================================
bool ExternalLinks::ExternalLink::RemoveCommRecipient(const CommPair& aCommPair)
{
   return (mCommRecipients.erase(aCommPair) == 1U);
}

// =================================================================================================
void ExternalLinks::ExternalLink::DeleteAllRecipients()
{
   mAddressRecipients.clear();
   mGroups.clear();
   mCommandRecipients.clear();
   mCommRecipients.clear();
}

// =================================================================================================
ExternalLinks::AddressVec ExternalLinks::ExternalLink::GetRecipients(bool aNotify) const
{
   AddressVec recipients;
   auto       simPtr = mParentPtr->GetSimulation();
   if ((!simPtr) || (!mParentPtr->GetPlatform()))
   {
      return recipients;
   }

   auto  networkManagerPtr = simPtr->GetCommNetworkManager();
   auto& groupManager      = simPtr->GetGroupManager();

   //! Add recipients identified by an address.
   for (const auto& address : mAddressRecipients)
   {
      //! Validate addresses with network manager. Exclude any addresses that are not resolved.
      //! Allow any address that is an interface, broadcast, or multicast.
      if (networkManagerPtr->IsAddressManaged(address) || (address.GetAddress() == address.GetBroadcastAddress()) ||
          (networkManagerPtr->IsMulticastManaged(address)))
      {
         recipients.push_back(address);
      }
      else if (aNotify)
      {
         auto out = ut::log::warning() << "Unable to resolve external link.";
         out.AddNote() << "Address: " << address;
      }
   }

   //! Add recipients identified by a group.
   for (const auto& groupName : mGroups)
   {
      auto groupPtr = groupManager.GetGroup(groupName);
      if (groupPtr)
      {
         auto members = groupPtr->GetMembers();
         for (const auto& member : members)
         {
            //! We only allow platform parts, specifically comms.
            if (member.second != 0U)
            {
               auto platformPtr = simPtr->GetPlatformByIndex(member.first);
               if (platformPtr)
               {
                  auto partPtr = platformPtr->GetArticulatedPart(member.second);
                  auto commPtr = dynamic_cast<comm::Comm*>(partPtr);
                  if (commPtr)
                  {
                     recipients.push_back(commPtr->GetAddress());
                  }
               }
            }
         }
      }
      else if (aNotify)
      {
         auto out = ut::log::warning() << "Unable to find external link.";
         out.AddNote() << "Group Name: " << groupName;
      }
   }

   //! Add recipients identified by command chains
   for (const auto& chainRecipient : mCommandRecipients)
   {
      auto commandChainPtr = mParentPtr->GetPlatform()->GetComponent<WsfCommandChain>(chainRecipient.mChainName);
      if (commandChainPtr)
      {
         switch (chainRecipient.mReportType)
         {
         case CommandChainRecipient::cCOMMANDER:
         {
            auto commanderPtr = commandChainPtr->GetCommander();
            if (commanderPtr)
            {
               auto commPtr = commanderPtr->GetComponent<comm::Comm>(chainRecipient.mRcvrName);
               if (commPtr)
               {
                  recipients.push_back(commPtr->GetAddress());
               }
               else if (aNotify)
               {
                  auto out = ut::log::warning() << "Unable to resolve external link.";
                  out.AddNote() << "Commander Recipient: " << chainRecipient.mRcvrName;
               }
            }
            break;
         }
         case CommandChainRecipient::cSUBORDINATES:
         {
            auto subs = commandChainPtr->GetSubordinates();
            for (const auto& sub : subs)
            {
               auto commPtr = sub->GetComponent<comm::Comm>(chainRecipient.mRcvrName);
               if (commPtr)
               {
                  recipients.push_back(commPtr->GetAddress());
               }
               else if (aNotify)
               {
                  auto out = ut::log::warning() << "Unable to resolve external link.";
                  out.AddNote() << "Subordinate Recipient: " << chainRecipient.mRcvrName;
               }
            }
            break;
         }
         case CommandChainRecipient::cPEERS:
         {
            auto peers = commandChainPtr->GetPeers();
            for (const auto& peer : peers)
            {
               auto commPtr = peer->GetComponent<comm::Comm>(chainRecipient.mRcvrName);
               if (commPtr)
               {
                  recipients.push_back(commPtr->GetAddress());
               }
               else if (aNotify)
               {
                  auto out = ut::log::warning() << "Unable to resolve external link";
                  out.AddNote() << "Peer Recipient: " << chainRecipient.mRcvrName;
               }
            }
            break;
         }
         default:
         {
            break;
         }
         }
      }
   }

   //! Add recipients identified by platform-name comm-name pair
   for (const auto& namePair : mCommRecipients)
   {
      auto platformPtr = simPtr->GetPlatformByName(namePair.first);
      if (platformPtr)
      {
         auto commPtr = platformPtr->GetComponent<comm::Comm>(namePair.second);
         if (commPtr)
         {
            recipients.push_back(commPtr->GetAddress());
         }
         else if (aNotify)
         {
            auto out  = ut::log::warning() << "Unable to resolve external link.";
            auto out2 = out.AddNote() << "Subordinate Recipient: ";
            out2.AddNote() << "Platform: " << namePair.first;
            out2.AddNote() << "Comm: " << namePair.second;
         }
      }
      else if (aNotify)
      {
         auto out  = ut::log::warning() << "Unable to resolve external link.";
         auto out2 = out.AddNote() << "Subordinate Recipient: ";
         out2.AddNote() << "Platform: " << namePair.first;
         out2.AddNote() << "Comm: " << namePair.second;
      }
   }

   return recipients;
}

// =================================================================================================
comm::Comm* ExternalLinks::ExternalLink::GetXmtr() const
{
   auto platformPtr = mParentPtr->GetPlatform();
   if (platformPtr)
   {
      return platformPtr->GetComponent<comm::Comm>(GetXmtrName());
   }
   return nullptr;
}

// =================================================================================================
size_t ExternalLinks::ExternalLink::GetRecipientCount() const
{
   size_t count = 0;

   if (mParentPtr)
   {
      auto groupManager = mParentPtr->GetSimulation()->GetGroupManager();
      for (const auto& curGroup : mGroups)
      {
         auto groupPtr = groupManager.GetGroup(curGroup);
         if (groupPtr)
         {
            count += groupPtr->GetMembers().size();
         }
      }

      for (const auto& curChain : mCommandRecipients)
      {
         auto chainPtr = mParentPtr->GetPlatform()->GetComponent<WsfCommandChain>(curChain.mChainName);
         if (chainPtr)
         {
            switch (curChain.mReportType)
            {
            case CommandChainRecipient::cCOMMANDER:
            {
               if (chainPtr->GetCommander())
               {
                  ++count;
               }
               break;
            }
            case CommandChainRecipient::cPEERS:
            {
               for (const auto& peer : chainPtr->GetPeers())
               {
                  if (peer)
                  {
                     ++count;
                  }
               }
               break;
            }
            case CommandChainRecipient::cSUBORDINATES:
            {
               for (const auto& sub : chainPtr->GetSubordinates())
               {
                  if (sub)
                  {
                     ++count;
                  }
               }
               break;
            }
            default:
            {
               throw UtException(
                  "Unexpected value for ExternalLinks::ExternalLink::GetRecipientCount CommandChainRecipient.");
            }
            }
         }
      }
   }

   count += mAddressRecipients.size();
   count += mCommRecipients.size();

   return count;
}

// =================================================================================================
bool ExternalLinks::Initialize(double aSimTime, WsfPlatform* aPlatformPtr)
{
   bool ok      = true;
   mPlatformPtr = aPlatformPtr;

   if (!mPlatformPtr)
   {
      ut::log::error() << "Unable to initialize external link without a valid parent platform.";
      return false;
   }

   //! Ensure that any and all links have valid transmitters.
   for (auto& link : mLinks)
   {
      link.SetParent(this);
      auto commName = link.GetXmtrName();
      auto commPtr  = mPlatformPtr->GetComponent<comm::Comm>(commName);
      if (!commPtr)
      {
         auto out = ut::log::error() << "External link reference is not a valid comm object on platform.";
         out.AddNote() << "Platform: " << mPlatformPtr->GetName();
         out.AddNote() << "Comm: " << commName;

         ok = false;
      }
   }

   //! We won't check for actual existence of any addresses, command chains, or groups.
   //! These objects may not be available during initialization, or may be dynamic in nature.
   return ok;
}

// =================================================================================================
bool ExternalLinks::ProcessInput(UtInput& aInput)
{
   bool                  myCommand  = true;
   auto                  command    = aInput.GetCommand();
   CommandChainRecipient reportType = CommandChainRecipient::cNONE;

   //! Multiple versions of the report_to/external_link command may be used, depending on how
   //! the user wishes to ultimately designate which comm device the message gets sent to.
   //! The difference is that comms now require an explicit target receiver for all transmissions,
   //! and will not accept ambiguous targets (such as a platform, since it may have multiple comms).
   //! The input has changed slightly to reflect this, while attempting to minimize changes to the
   //! greatest extent possible. The updates are as follows:
   //! A. Command chains may still be used to designate the target platform. However, the transmitter
   //!    is now required to be named (using the 'via' token, as was done previously). In addition, comms
   //!    require a target receiver be named for a destination. This may be explicitly named by using the
   //!    'to' token, or it is assumed implicitly to be the same name as the transmitter.
   //! B. Groups of platforms are no longer sufficient as recipients. These are now replaced by groups of
   //!    comms.
   //! C. Linkage now has the added capability to be specified by a platform-name comm-name pair, or via
   //!    specifying a specific address. Using an address as the link target allows for non-standard methods
   //!    of reporting, such that an appropriate broadcast or multicast address will result in those methods
   //!    being used for external link message transmissions (as opposed to unicast).
   //!
   //! 1. [external_link | report_to] [ command_chain <cmd-chain-name> ] [commander | peers | subordinates] [ via
   //! <xmtr-name> ] [ to <rcvr-name> ]
   //! 2. [report_to_group] <group-name> [ via <xmtr-name> ]
   //! 3. [external_link | report_to] [ platform <platform-name> ] [ comm <comm-name> ] [ via <xmtr-name> ]
   //! 4. [external_link | report_to] [ address <comm-address> ] [ via <xmtr-name> ]

   if ((command == "external_link") || (command == "report_to"))
   {
      std::string subCommand;
      aInput.ReadValue(subCommand);

      if (subCommand == "command_chain")
      {
         CommandChainTarget cmdChainTarget;
         aInput.ReadValue(cmdChainTarget.mChainName);

         std::string chainType;
         aInput.ReadValue(chainType);

         auto result = CommandChainCommand(chainType, cmdChainTarget.mReportType);
         if (!result)
         {
            UtInput::BadValue(
               aInput,
               "Missing external_link/report_to command chain type - must be commander, subordinates, or peers.");
         }

         std::string viaStr;
         aInput.ReadValue(viaStr);

         if (viaStr != "via")
         {
            UtInput::BadValue(aInput, "Malformed input, expected transmitter command usage 'via'");
         }

         std::string xmtrTarget;
         aInput.ReadValue(xmtrTarget);

         std::string toStr;
         aInput.ReadValue(toStr);

         if (toStr != "to")
         {
            cmdChainTarget.mRcvrName = xmtrTarget;
            aInput.PushBack(toStr);
         }
         else
         {
            aInput.ReadValue(cmdChainTarget.mRcvrName);
         }

         result = AddRecipient(xmtrTarget, cmdChainTarget);

         if ((!result) && mDebug)
         {
            auto out = ut::log::warning() << "Could not add external link.";
            out.AddNote() << "Command Chain Target: " << cmdChainTarget.mChainName;
            out.AddNote() << "Repeated targets are ignored. Ensure the same command is not repeated on the same object "
                             "or a derived instance.";
         }
      }
      else if (subCommand == "platform")
      {
         CommPair commPair;
         aInput.ReadValue(commPair.first);

         std::string commValue;
         aInput.ReadValue(commValue);
         if (commValue != "comm")
         {
            UtInput::BadValue(aInput, "Malformed input, expected 'comm' input usage.");
         }

         aInput.ReadValue(commPair.second);

         std::string viaStr;
         aInput.ReadValue(viaStr);

         if (viaStr != "via")
         {
            UtInput::BadValue(aInput, "Malformed input, expected transmitter command usage 'via'");
         }

         std::string xmtrTarget;
         aInput.ReadValue(xmtrTarget);

         auto result = AddRecipient(xmtrTarget, commPair);
         if ((!result) && mDebug)
         {
            auto out = ut::log::warning() << "Could not add external link.";
            out.AddNote() << "Explicit Target: " << commPair.first << "." << commPair.second;
            out.AddNote() << "Repeated targets are ignored. Ensure the same command is not repeated on the same object "
                             "or a derived instance.";
         }
      }
      else if (subCommand == "address")
      {
         auto address = Address::ProcessInput(aInput);

         std::string viaStr;
         aInput.ReadValue(viaStr);

         if (viaStr != "via")
         {
            UtInput::BadValue(aInput, "Malformed input, expected transmitter command usage 'via'");
         }

         std::string xmtrTarget;
         aInput.ReadValue(xmtrTarget);

         auto result = AddRecipient(xmtrTarget, address);
         if ((!result) && mDebug)
         {
            auto out = ut::log::warning() << "Could not add external link.";
            out.AddNote() << "Address: " << address;
         }
      }
      else if (CommandChainCommand(subCommand, reportType))
      {
         CommandChainTarget cmdChainTarget;
         cmdChainTarget.mReportType = reportType;
         cmdChainTarget.mChainName  = "default";

         std::string viaStr;
         aInput.ReadValue(viaStr);

         if (viaStr != "via")
         {
            UtInput::BadValue(aInput, "Malformed input, expected transmitter command usage 'via'");
         }

         std::string xmtrTarget;
         aInput.ReadValue(xmtrTarget);

         std::string toStr;
         aInput.ReadValue(toStr);

         if (toStr != "to")
         {
            cmdChainTarget.mRcvrName = xmtrTarget;
            aInput.PushBack(toStr);
         }
         else
         {
            aInput.ReadValue(cmdChainTarget.mRcvrName);
         }

         auto result = AddRecipient(xmtrTarget, cmdChainTarget);

         if ((!result) && mDebug)
         {
            auto out = ut::log::warning() << "Could not add external link.";
            out.AddNote() << "Command Chain Target: " << cmdChainTarget.mChainName;
            out.AddNote() << "Repeated targets are ignored. Ensure the same command is not repeated on the same object "
                             "or a derived instance.";
         }
      }
      else
      {
         UtInput::BadValue(aInput, "Invalid external_link/report_to target type.");
      }
   }
   else if (command == "report_to_group")
   {
      WsfStringId groupName;
      aInput.ReadValue(groupName);

      std::string viaStr;
      aInput.ReadValue(viaStr);

      if (viaStr != "via")
      {
         UtInput::BadValue(aInput, "Malformed input, expected transmitter command usage 'via'");
      }

      std::string xmtrTarget;
      aInput.ReadValue(xmtrTarget);

      auto result = AddRecipient(xmtrTarget, groupName);

      if ((!result) && mDebug)
      {
         auto out = ut::log::warning() << "Could not add group external link.";
         out.AddNote() << "Group Name: " << groupName;
         out.AddNote() << "Repeated targets are ignored. Ensure the same command is not repeated on the same object or "
                          "a derived instance.";
      }
   }
   else if (command == "clear_external_links")
   {
      DeleteAllRecipients();
   }
   else if (command == "debug_external_links")
   {
      mDebug = true;
   }
   else
   {
      myCommand = false;
   }

   return myCommand;
}

// =================================================================================================
bool ExternalLinks::CommandChainCommand(const std::string& aCommand, CommandChainRecipient& aReportType)
{
   bool myCommand = true;

   if (aCommand == "commander")
   {
      aReportType = CommandChainRecipient::cCOMMANDER;
   }
   else if (aCommand == "subordinates")
   {
      aReportType = CommandChainRecipient::cSUBORDINATES;
   }
   else if (aCommand == "peers")
   {
      aReportType = CommandChainRecipient::cPEERS;
   }
   else
   {
      aReportType = CommandChainRecipient::cNONE;
      myCommand   = false;
   }

   return myCommand;
}

// =================================================================================================
#undef SendMessage // Avoid conflict with Windows macro
void ExternalLinks::SendMessage(double aSimTime, const WsfMessage& aMessage)
{
   for (const auto& link : mLinks)
   {
      auto xmtrPtr    = link.GetXmtr();
      auto recipients = link.GetRecipients(mDebug);

      for (const auto& recipient : recipients)
      {
         if (xmtrPtr->GetAddress() != recipient)
         {
            xmtrPtr->Send(aSimTime, ut::clone(aMessage), recipient);
         }
         else if (mDebug)
         {
            auto out = ut::log::warning() << "External link message send aborted.";
            out.AddNote() << "Sender and recipient was the same address.";
            out.AddNote() << "Address: " << recipient;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
      }
   }
}

// =================================================================================================
void ExternalLinks::SendMessage(double aSimTime, const WsfMessage& aMessage, WsfStringId aExcludedPlatformName)
{
   if (aExcludedPlatformName.Empty())
   {
      SendMessage(aSimTime, aMessage);
      return;
   }

   auto excludedPlatformPtr = GetSimulation()->GetPlatformByName(aExcludedPlatformName);
   if (!excludedPlatformPtr)
   {
      SendMessage(aSimTime, aMessage);
      return;
   }

   auto networkManagerPtr = GetSimulation()->GetCommNetworkManager();

   for (const auto& link : mLinks)
   {
      auto xmtrPtr    = link.GetXmtr();
      auto recipients = link.GetRecipients(mDebug);

      for (const auto& recipient : recipients)
      {
         auto commPtr = networkManagerPtr->GetComm(recipient);
         if (commPtr)
         {
            if (commPtr->GetPlatform()->GetNameId() == aExcludedPlatformName)
            {
               continue;
            }
         }

         if (xmtrPtr->GetAddress() != recipient)
         {
            xmtrPtr->Send(aSimTime, ut::clone(aMessage), recipient);
         }
         else if (mDebug)
         {
            auto out = ut::log::warning() << "External link message send aborted.";
            out.AddNote() << "Sender and recipient was the same address.";
            out.AddNote() << "Address: " << recipient;
            out.AddNote() << "Platform: " << GetPlatform()->GetName();
         }
      }
   }
}

// =================================================================================================
WsfSimulation* ExternalLinks::GetSimulation() const
{
   return mPlatformPtr ? mPlatformPtr->GetSimulation() : nullptr;
}

// =================================================================================================
bool ExternalLinks::AddRecipient(WsfStringId aXmtrName, const Address& aRcvrAddress)
{
   bool added = false;
   auto it    = FindLink(aXmtrName);

   if (it != std::end(mLinks))
   {
      added = it->AddAddressRecipient(aRcvrAddress);
   }
   else
   {
      ExternalLink link;
      link.SetXmtrName(aXmtrName);
      added = link.AddAddressRecipient(aRcvrAddress);
      mLinks.push_back(link);
   }

   return added;
}

// =================================================================================================
bool ExternalLinks::AddRecipient(WsfStringId aXmtrName, WsfStringId aCommGroupName)
{
   bool added = false;
   auto it    = FindLink(aXmtrName);

   if (it != std::end(mLinks))
   {
      added = it->AddGroupRecipient(aCommGroupName);
   }
   else
   {
      ExternalLink link;
      link.SetXmtrName(aXmtrName);
      added = link.AddGroupRecipient(aCommGroupName);
      mLinks.push_back(link);
   }

   return added;
}

// =================================================================================================
bool ExternalLinks::AddRecipient(WsfStringId aXmtrName, const CommandChainTarget& aCommandChainTarget)
{
   bool added = false;
   auto it    = FindLink(aXmtrName);

   if (it != std::end(mLinks))
   {
      added = it->AddCommandRecipient(aCommandChainTarget);
   }
   else
   {
      ExternalLink link;
      link.SetXmtrName(aXmtrName);
      added = link.AddCommandRecipient(aCommandChainTarget);
      mLinks.push_back(link);
   }

   return added;
}

// =================================================================================================
bool ExternalLinks::AddRecipient(WsfStringId aXmtrName, const CommPair& aCommPair)
{
   bool added = false;
   auto it    = FindLink(aXmtrName);

   if (it != std::end(mLinks))
   {
      added = it->AddCommRecipient(aCommPair);
   }
   else
   {
      ExternalLink link;
      link.SetXmtrName(aXmtrName);
      added = link.AddCommRecipient(aCommPair);
      mLinks.push_back(link);
   }

   return added;
}

// =================================================================================================
bool ExternalLinks::RemoveRecipient(WsfStringId aXmtrName, const Address& aRcvrAddress)
{
   bool removed = false;
   auto it      = FindLink(aXmtrName);

   if (it != std::end(mLinks))
   {
      removed = it->RemoveAddressRecipient(aRcvrAddress);
   }

   if (removed && (it->GetRecipientCount() == 0))
   {
      mLinks.erase(it);
   }

   return removed;
}

// =================================================================================================
bool ExternalLinks::RemoveRecipient(WsfStringId aXmtrName, WsfStringId aCommGroupName)
{
   bool removed = false;
   auto it      = FindLink(aXmtrName);

   if (it != std::end(mLinks))
   {
      removed = it->RemoveGroupRecipient(aCommGroupName);
   }

   if (removed && (it->GetRecipientCount() == 0))
   {
      mLinks.erase(it);
   }

   return removed;
}

// =================================================================================================
bool ExternalLinks::RemoveRecipient(WsfStringId aXmtrName, const CommandChainTarget& aCommandChainTarget)
{
   bool removed = false;
   auto it      = FindLink(aXmtrName);

   if (it != std::end(mLinks))
   {
      removed = it->RemoveCommandRecipient(aCommandChainTarget);
   }

   if (removed && (it->GetRecipientCount() == 0))
   {
      mLinks.erase(it);
   }

   return removed;
}

// =================================================================================================
bool ExternalLinks::RemoveRecipient(WsfStringId aXmtrName, const CommPair& aCommPair)
{
   bool removed = false;
   auto it      = FindLink(aXmtrName);

   if (it != std::end(mLinks))
   {
      removed = it->RemoveCommRecipient(aCommPair);
   }

   if (removed && (it->GetRecipientCount() == 0))
   {
      mLinks.erase(it);
   }

   return removed;
}

// =================================================================================================
void ExternalLinks::DeleteAllRecipients()
{
   mLinks.clear();
}

// =================================================================================================
size_t ExternalLinks::GetRecipientCount() const
{
   size_t count = 0;

   for (const auto& link : mLinks)
   {
      count += link.GetRecipientCount();
   }

   return count;
}

// =================================================================================================
size_t ExternalLinks::GetRecipientCount(WsfStringId aXmtrName) const
{
   auto it = FindLink(aXmtrName);
   if (it != std::end(mLinks))
   {
      return it->GetRecipientCount();
   }
   return 0;
}

// =================================================================================================
ExternalLinks::LinkIt ExternalLinks::FindLink(WsfStringId aXmtrName)
{
   auto it =
      std::find_if(std::begin(mLinks),
                   std::end(mLinks),
                   [aXmtrName](const ExternalLinks::ExternalLink& aLink) { return aXmtrName == aLink.GetXmtrName(); });

   return it;
}

// =================================================================================================
ExternalLinks::LinkConstIt ExternalLinks::FindLink(WsfStringId aXmtrName) const
{
   auto it = const_cast<ExternalLinks&>(*this).FindLink(aXmtrName);
   return it;
}

} // namespace wsf
