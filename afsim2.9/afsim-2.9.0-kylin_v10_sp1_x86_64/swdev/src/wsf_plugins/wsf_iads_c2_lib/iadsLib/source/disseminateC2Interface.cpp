// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/*******************************************************************************
*
*   File: disseminateC2Interface.cpp
*
*   Classification: UNCLASSIFIED
*
*   Details:
*
*   Contract Number:
*   Contractor Name:	Radiance Technologies
*   Address:			350 Wynn Drive
*   					   Huntsville, AL 35805
*
*   Abstract: C2 dissemination for interfacing with the asset manager.
*
*******************************************************************************/

#include <iadsLib/assetManagerInterface.hpp>
#include <iadsLib/disseminateC2Interface.hpp>

#include <vector>
#include <utility>
#include <sstream>
#include <memory>

#include <logger.hpp>

// Message classes
#include <iadsLib/airTargetMessage.hpp>
#include <iadsLib/plotMessage.hpp>
#include <iadsLib/bearingReportMessage.hpp>
#include <iadsLib/combatStatusMessage.hpp>
#include <iadsLib/assignmentACKMessage.hpp>
#include <iadsLib/assignmentMessage.hpp>
#include <iadsLib/weaponsControlMessage.hpp>
#include <iadsLib/cueMessage.hpp>
#include <iadsLib/assignmentTrackMessage.hpp>

namespace il
{

DisseminateC2Interface::DisseminateC2Interface()
   : ReferencesAssetManager()
   , m_routing_style (E_NEXT_C2)
{

	m_routes[DisseminateC2Interface::eMsgTypeKey::E_ASSIGNMENT] = DisseminateC2Interface::eRoutingKey::E_DYNAMIC;
	m_routes[DisseminateC2Interface::eMsgTypeKey::E_ASSIGNMENT_CANCEL] = DisseminateC2Interface::eRoutingKey::E_DYNAMIC;
	m_routes[DisseminateC2Interface::eMsgTypeKey::E_ASSIGNMENT_STATUS] = DisseminateC2Interface::eRoutingKey::E_DYNAMIC;
	m_routes[DisseminateC2Interface::eMsgTypeKey::E_CUE] = DisseminateC2Interface::eRoutingKey::E_DYNAMIC;
	m_routes[DisseminateC2Interface::eMsgTypeKey::E_TRACKA_UPDATE] = DisseminateC2Interface::eRoutingKey::E_DYNAMIC;

}

DisseminateC2Interface::DisseminateC2Interface (const DisseminateC2Interface &from)
   : ReferencesAssetManager (from)
   , m_OutgoingMessages (from.m_OutgoingMessages)
   , m_routes (from.m_routes)
   , m_routing_style (from.m_routing_style)
{

}

DisseminateC2Interface *DisseminateC2Interface::Clone ()
{
   return new DisseminateC2Interface (*this);
}


void DisseminateC2Interface::OverWriteTableEntry (const DisseminateC2Interface::eMsgTypeKey key, const unsigned int route)
{
   m_routes[key] = route;
}


void DisseminateC2Interface::AppendTableEntry (const DisseminateC2Interface::eMsgTypeKey key, const unsigned int route)
{
   auto entry = m_routes.find(key);

   if (entry != m_routes.end())
      entry->second |= route;

   else
	   m_routes[key] = route;

}

const DisseminateC2Interface::tOutgoingMsgs & DisseminateC2Interface::getOutgoingMessages()
{
   return m_OutgoingMessages;
}

void DisseminateC2Interface::updateOutgoingMessages()
{
   // clear outgoing messages by assigning pending messages
   m_OutgoingMessages = m_PendingOutMessages;
   // don't forget to clear pending messages
   m_PendingOutMessages.clear();

   // get all pending message events from the asset manager
   auto &msg_events = m_asset_manager->getOutgoingMessages();
   // TTD: do we need to prioritize self status over other statuses? probably doesn't matter but verify
   for (auto it = msg_events.begin(); it != msg_events.end(); ++it)
   {
      auto &msg_event = *it;
      switch (msg_event.first)
      {
      case AssetManagerInterface::E_UPDATED_TRACK:
      case AssetManagerInterface::E_DROPPED_TRACK:
         // tracks
         GetRoutedMessages (E_TRACK_UPDATE, msg_event.second, m_OutgoingMessages);
         break;

      case AssetManagerInterface::E_ASSIGNMENT_TRACK_UPDATE:
         // assignment
         GetRoutedMessages (E_TRACKA_UPDATE, msg_event.second, m_OutgoingMessages);
         break;

      case AssetManagerInterface::E_ASSIGN_MSG_OUT:
         // assignment tracks
         GetRoutedMessages (E_ASSIGNMENT, msg_event.second, m_OutgoingMessages);
         break;

      case AssetManagerInterface::E_REJECTED_ASSIG_RESPONSE:
      case AssetManagerInterface::E_ASSIG_ACK_RCVD:
      case AssetManagerInterface::E_ASSIGN_OVERRIDE_NOTIF:
         // assignment status
         GetRoutedMessages (E_ASSIGNMENT_STATUS, msg_event.second, m_OutgoingMessages);
         break;

      case AssetManagerInterface::E_CANCEL_ASSIG_RECVD:
         // assignment cancel
         GetRoutedMessages (E_ASSIGNMENT_CANCEL, msg_event.second, m_OutgoingMessages);
         break;

      case AssetManagerInterface::E_STATUS_RCVD:
         // status
         GetRoutedMessages (E_STATUS, msg_event.second, m_OutgoingMessages);
         break;


      case AssetManagerInterface::E_SENSOR_CUE:
         // sensor cues
         GetRoutedMessages (E_CUE, msg_event.second, m_OutgoingMessages);
         break;

      default:
         HCL_ERROR_LOGGER(GetAssetManager()->getGlobalLogger()) << "DisseminateC2Interface::getOutgoingMessages(): Unknown message event type: " << msg_event.first;
         break;
      }
   }

   m_asset_manager->clearOutgoingMessages();
}

 void DisseminateC2Interface::clearOutgoingMessages ()
{
   m_OutgoingMessages.clear();
}

void DisseminateC2Interface::addPendingOutMessage (const std::shared_ptr<baseMessage> &msg)
{
   m_PendingOutMessages.push_back (msg);
}

void DisseminateC2Interface::setRoutingStyle (const eRoutingStyle type)
{
   m_routing_style = type;
}

void DisseminateC2Interface::GetRoutedMessages (const eMsgTypeKey key, const std::shared_ptr<baseMessage> &msg, tOutgoingMsgs &msgs)
{
   // lookup key
   auto route_iter = m_routes.find (key);
   if (route_iter == m_routes.end())
      return; // no routes - nothing to do

   // subordinates
   if (route_iter->second & E_SUBORDINATE)
      GetSubordinateRoutes (msg, msgs);

   // peers
   if (route_iter->second & E_PEER)
      GetPeerRoutes (msg, msgs);

   // commander
   if (route_iter->second & E_COMMANDER)
      GetCommanderRoutes (msg, msgs);

   // dynamic
   if (route_iter->second & E_DYNAMIC)
      GetDynamicRoutes (msg, msgs);
}

void DisseminateC2Interface::GetGenericRoute (const std::shared_ptr<baseMessage> &msg, const std::vector<idRecord> &routes, tOutgoingMsgs &msgs)
{
   const auto our_id = m_asset_manager->getThisAsset()->getID();

   for (auto it = routes.begin(); it != routes.end(); ++it)
   {
      auto &route = *it;
      if (route.isValid())
      {
         // copy message, set headers, queue message
         auto msg_copy = CopyMessage (msg);
         msg_copy->setSenderID (our_id);
         msg_copy->setDestinationID (route);
         msg_copy->setDestinationBroadcast (false); // no messages are broadcast, all point-to-point in our implementation of AFSIM
         msg_copy->setTransmitTime (m_asset_manager->getCurrentTime());
         msgs.push_back (msg_copy);
      }
   }
}

void DisseminateC2Interface::GetSubordinateRoutes (const std::shared_ptr<baseMessage> &msg, tOutgoingMsgs &msgs)
{
   GetGenericRoute (msg, GetSubordinateList(), msgs);
}

void DisseminateC2Interface::GetPeerRoutes (const std::shared_ptr<baseMessage> &msg, tOutgoingMsgs &msgs)
{
   GetGenericRoute (msg, GetPeerList(), msgs);
}

void DisseminateC2Interface::GetCommanderRoutes (const std::shared_ptr<baseMessage> &msg, tOutgoingMsgs &msgs)
{
   GetGenericRoute (msg, GetCommanderList(), msgs);
}

void DisseminateC2Interface::GetDynamicRoutes (const std::shared_ptr<baseMessage> &msg, tOutgoingMsgs &msgs)
{
   idRecord destination;

   // check message for proper message type for dynamic routing (track(A), assignment, and assignment status)
   switch (msg->getMessageType())
   {
   case AIR_TARGET_MESSAGE:
   case PLOT_MESSAGE:
   case BEARING_REPORT_MESSAGE:
   case COMBAT_STATUS_MESSAGE:
   case WEAPONS_CONTROL_MESSAGE:
      HCL_ERROR_LOGGER(GetAssetManager()->getGlobalLogger()) << "Dynamic routing not supported for message type: " << msg->getMessageTypeString();
      break;

   case ASSIGNMENT_ACK_MESSAGE:
      {
         auto ack_msg = dynamic_cast<assignmentACKMessage *> (msg.get());
         destination = ack_msg->getInitiatingID();
         break;
      }

   case ASSIGNMENT_MESSAGE:
      {
         auto assign_msg = dynamic_cast<assignmentMessage *> (msg.get());
         destination = assign_msg->getAssignedID().getID();
         break;
      }

   case ASSIGNMENT_TRACK_MESSAGE:
      {
         auto tracka_msg = dynamic_cast<assignmentTrackMessage *> (msg.get());
         destination = tracka_msg->getAssignedUnitID().getID();
         break;
      }

   case CUE_MESSAGE:
      {
         auto cue_msg = dynamic_cast<cueMessage *> (msg.get());
         destination = cue_msg->getCuedUnitID().getID();
         break;
      }
   }

   auto next_hop = GetDynamicNextHop (m_routing_style, destination);
   auto msg_copy = CopyMessage (msg);
   msg_copy->setSenderID (m_asset_manager->getThisAsset()->getID());
   msg_copy->setDestinationID (next_hop);
   msg_copy->setDestinationBroadcast (false); // no messages are broadcast, all point-to-point in our implementation of AFSIM
   msg_copy->setTransmitTime (m_asset_manager->getCurrentTime());
   msgs.push_back (msg_copy);
}


std::vector<idRecord> DisseminateC2Interface::GetSubordinateList () const
{
  return m_asset_manager->getThisAsset()->getDirectSubordinates();
}


std::vector<idRecord> DisseminateC2Interface::GetCommanderList () const
{
   std::vector<idRecord> ids;

   ids.push_back( m_asset_manager->getThisAsset()->getCommanderID());

   return ids;
}


std::vector<idRecord> DisseminateC2Interface::GetPeerList () const
{
   return m_asset_manager->getThisAsset()->getDirectPeers();
}


idRecord DisseminateC2Interface::GetDynamicNextHop (const eRoutingStyle style, const idRecord dest) const
{
   idRecord next_hop;

   // test for self assignment
   if (dest == m_asset_manager->getThisAsset()->getID())
      return next_hop = dest;

   const auto &assets = m_asset_manager->getAssets();

   // note that we need to determine if this is up or down chain to get the next hop
   auto findNextInChain = [](std::shared_ptr<assetRecord> starting, const idRecord &ending, const assetMap &assets) -> idRecord
   {
      if (starting->isInCommandChain (ending, assets))
         return starting->findNextCommanderInChain (ending, assets);
      else if (starting->isSubordinate (ending, assets))
         return starting->findNextSubordinateInChain (ending, assets);

      return idRecord ();
   };


   if (style == E_NEXT_UNIT)
      next_hop = findNextInChain (m_asset_manager->getThisAsset(), dest, assets);
   else if (style == E_DIRECT)
      next_hop = dest;
   else if (style == E_NEXT_C2)
   {
      bool found_c2 = false;
      auto curr_asset_ptr = m_asset_manager->getThisAsset();
      while (!found_c2)
      {
         auto next_id = findNextInChain (curr_asset_ptr, dest, assets);
         curr_asset_ptr = assets.GetAsset (next_id);
         if (curr_asset_ptr)
         {
            if (curr_asset_ptr->getC2Capable())
            {
               next_hop = curr_asset_ptr->getID();
               found_c2 = true;
            }
         }
         else
            break;
      }

      // if we haven't found an intermediate C2, send to the destination but flag it
      if (!found_c2)
      {
         HCL_ERROR_LOGGER(GetAssetManager()->getGlobalLogger()) << "Could not find route from " << m_asset_manager->getThisAsset()->getID() << " to " << dest << std::endl;
         next_hop = dest;
      }
   }

   return next_hop;
}


std::shared_ptr<baseMessage> DisseminateC2Interface::CopyMessage (const std::shared_ptr<baseMessage> &msg)
{
   std::shared_ptr<baseMessage> msg_copy (msg->clone());


   return msg_copy;
}

} // namespace il
