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

#include "WsfCommUtil.hpp"

#include <iostream>

#include "WsfComm.hpp"
#include "WsfCommAddress.hpp"
#include "WsfCommNetworkManager.hpp"
#include "WsfCommandChain.hpp"
#include "WsfGroup.hpp"
#include "WsfPlatform.hpp"

namespace wsf
{
namespace comm
{
namespace util
{

// ============================================================================
//! Find a comm device on the sending platform over which a message can be sent and then received by the target
//! platform. The algorithm will only choose a device which is valid only at the time of the request.
//! @param aSourcePtr The platform wishing to send the message.
//! @param aTargetPtr The platform to whom the message is intended.
//! @returns A pointer to the comm device on the source platform over which the message should be sent.
//! The return value will be zero if an applicable device can not be determined.
Comm* SelectDevice(WsfPlatform* aSourcePtr, WsfPlatform* aTargetPtr)
{
   for (WsfComponentList::RoleIterator<Comm> sourceIter(*aSourcePtr); !sourceIter.AtEnd(); ++sourceIter)
   {
      auto sourceCommPtr = *sourceIter;
      if (sourceCommPtr->IsTurnedOn())
      {
         for (WsfComponentList::RoleIterator<Comm> targetIter(*aTargetPtr); !targetIter.AtEnd(); ++targetIter)
         {
            auto targetCommPtr = *targetIter;
            if (targetCommPtr->IsTurnedOn())
            {
               if (sourceCommPtr->GetNetwork() == targetCommPtr->GetNetwork())
               {
                  return sourceCommPtr;
               }
            }
         }
      }
   }
   return nullptr; // No path to target
}

// ============================================================================
//! A helper method to select the recipient platforms from a command chain.
//! @param aCmdChainPtr    The command chain from which the recipients are to be selected.
//! @param aRecipientMask  A bit mask of 'Recipient' values indicating which recipients
//!                        are to be selected.
//! @param aRecipients     An updated list of platforms containing the selected recipients.
//!                        The selected platforms are appended to the list!
void SelectRecipients(WsfCommandChain* aCmdChainPtr, int aRecipientMask, std::vector<WsfPlatform*>& aRecipients)
{
   assert(aCmdChainPtr != nullptr);

   if ((aRecipientMask & cCOMMANDER) != 0)
   {
      WsfPlatform* platformPtr = aCmdChainPtr->GetCommander();
      if ((platformPtr != nullptr) && (platformPtr != aCmdChainPtr->GetPlatform())) // Don't send to self
      {
         aRecipients.push_back(platformPtr);
      }
   }

   if ((aRecipientMask & cPEERS) != 0)
   {
      WsfCommandChain::PlatformList& platforms = aCmdChainPtr->GetPeers();
      for (auto& pltfrm : platforms)
      {
         aRecipients.push_back(pltfrm);
      }
   }

   if ((aRecipientMask & cSUBORDINATES) != 0)
   {
      WsfCommandChain::PlatformList& platforms = aCmdChainPtr->GetSubordinates();
      for (auto& pltfrm : platforms)
      {
         aRecipients.push_back(pltfrm);
      }
   }
}

// ============================================================================
//! A helper method to select the recipient addresses from a group.
//! @param aSenderPtr      The platform sending the message.
//! @param aGroupPtr       The group from which the recipients are to be selected.
//! @param aRecipients     An updated list of platforms containing the selected recipients.
//!                        The selected platforms are appended to the list!
void SelectRecipients(WsfPlatform* aSenderPtr, WsfGroup* aGroupPtr, std::vector<WsfPlatform*>& aRecipients)
{
   const WsfGroup::MemberList& members = aGroupPtr->GetMembers();
   for (const auto& member : members)
   {
      if (member.second == 0)
      {
         WsfPlatform* memberPtr = aSenderPtr->GetSimulation()->GetPlatformByIndex(member.first);
         if ((memberPtr != nullptr) && (memberPtr != aSenderPtr))
         {
            aRecipients.push_back(memberPtr);
         }
      }
   }
}

// ============================================================================
//! The LocalSlaveNetworkName returns the name of the correct network to join to
//! (or already is a part of) when the provided comm device uses the legacy
//! <local:slave> value. This method exists to support the legacy use of
//! creating networks based off of command chains.
//! @param aComm   The WsfComm object that received the command.
//! @returns       THe proper network name, given the <local:slave> value.
std::string LocalSlaveNetworkName(Comm& aComm)
{
   auto commandChainPtr = aComm.GetPlatform()->GetComponent<WsfCommandChain>(WsfCommandChain::GetDefaultNameId());
   if (commandChainPtr && (!commandChainPtr->GetCommanderNameId().IsNull()))
   {
      return ("master:" + commandChainPtr->GetCommanderName());
   }

   return "";
}

// ============================================================================
//! The LocalMasterNetworkName returns the name of the correct network to join to
//! (or already is a part of) when the provided comm device uses the legacy
//! <local:master> value. This method exists to support the legacy use of
//! creating networks based off of command chains.
//! @param aComm   The WsfComm object that received the command.
//! @returns       THe proper network name, given the <local:master> value.
std::string LocalMasterNetworkName(Comm& aComm)
{
   std::string networkName = "master:" + aComm.GetPlatform()->GetName();
   return networkName;
}

} // namespace util
} // namespace comm
} // namespace wsf
