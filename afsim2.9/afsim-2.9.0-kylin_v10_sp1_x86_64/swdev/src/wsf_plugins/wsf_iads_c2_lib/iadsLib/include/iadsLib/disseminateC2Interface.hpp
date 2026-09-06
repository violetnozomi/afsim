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
*   File: disseminateC2Interface.hpp
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

#ifndef IADSLIB_DISSEMINATE_C2_INTERFACE_HPP
#define IADSLIB_DISSEMINATE_C2_INTERFACE_HPP

namespace il
{
class AssetManagerInterface;
}


#include <vector>
#include <utility>
#include <map>
#include <memory>

#include <logger.hpp>
#include <iadsLib/baseMessage.hpp>
#include <iadsLib/referencesProcessor.hpp>

namespace il
{

   class IADSLIB_EXPORT DisseminateC2Interface : public ReferencesAssetManager
   {
   public:
      typedef std::vector<std::shared_ptr<baseMessage>> tOutgoingMsgs;

      enum eMsgTypeKey {
                         E_TRACK_UPDATE,
                         E_TRACKA_UPDATE,         // Assignment Tracks
                         E_ASSIGNMENT,
                         E_ASSIGNMENT_STATUS,
                         E_ASSIGNMENT_CANCEL,
                         E_STATUS,
                         E_CUE,
                       };

      enum eRoutingKey {
                         E_SUBORDINATE = (1  << 0),
                         E_PEER        = (1  << 1),
                         E_COMMANDER   = (1  << 2),
                         E_DYNAMIC     = (1  << 3)
                       };

      enum eRoutingStyle { // for dynamic routing
                          E_NEXT_UNIT,  // messages routed to next unit in line (HELIOS/Flames mode)
                          E_NEXT_C2,    // messages routed to next C2 in line (AFSIM mode)
                          E_DIRECT      // no routing, messgaes addressed to destination
                        };

   public:
      DisseminateC2Interface ();
      virtual ~DisseminateC2Interface() = default;
      virtual DisseminateC2Interface *Clone ();

      virtual void OverWriteTableEntry (const eMsgTypeKey key, const unsigned int route);  // clears existing routes and sets new route for the specified key
      virtual void AppendTableEntry (const eMsgTypeKey key, const unsigned int route);     // appends entry to existing routes for the specified key

      virtual void updateOutgoingMessages();
      virtual const tOutgoingMsgs & getOutgoingMessages();
      virtual void clearOutgoingMessages ();

      virtual void addPendingOutMessage (const std::shared_ptr<baseMessage> &msg);

      virtual void setRoutingStyle (const eRoutingStyle type);

   protected:
      tOutgoingMsgs m_OutgoingMessages;
      tOutgoingMsgs m_PendingOutMessages; // messages that need to be dispatched but are awaiting some future time
      tOutgoingMsgs m_PendingInMessages;  // messages incoming that are awaiting some future time

      std::map<eMsgTypeKey, unsigned int>  m_routes;

      eRoutingStyle  m_routing_style;

   protected:
      DisseminateC2Interface (const DisseminateC2Interface &from);

      virtual void GetRoutedMessages (const eMsgTypeKey key, const std::shared_ptr<baseMessage> &msg, tOutgoingMsgs &msgs);
      virtual void GetGenericRoute (const std::shared_ptr<baseMessage> &msg, const std::vector<idRecord> &routes, tOutgoingMsgs &msgs);
      virtual void GetSubordinateRoutes (const std::shared_ptr<baseMessage> &msg, tOutgoingMsgs &msgs);
      virtual void GetPeerRoutes (const std::shared_ptr<baseMessage> &msg, tOutgoingMsgs &msgs);
      virtual void GetCommanderRoutes (const std::shared_ptr<baseMessage> &msg, tOutgoingMsgs &msgs);
      virtual void GetDynamicRoutes (const std::shared_ptr<baseMessage> &msg, tOutgoingMsgs &msgs);

      virtual std::vector<idRecord> GetSubordinateList () const;
      virtual std::vector<idRecord> GetCommanderList () const;
      virtual std::vector<idRecord> GetPeerList () const;
      virtual idRecord GetDynamicNextHop (const eRoutingStyle style, const idRecord dest) const;

      virtual std::shared_ptr<baseMessage> CopyMessage (const std::shared_ptr<baseMessage> &msg);

   private:
      DisseminateC2Interface & operator=(const DisseminateC2Interface &rhs);

   };
}

#endif
