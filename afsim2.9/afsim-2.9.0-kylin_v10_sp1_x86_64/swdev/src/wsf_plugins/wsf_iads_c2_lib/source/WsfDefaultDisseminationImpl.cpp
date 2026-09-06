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
*   File: WsfDefaultDisseminationImpl.cpp
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
*   Abstract: This class provides default implementations in C++ for
*    what could be overridden in script for the Dissemination Manager base class.
*    The intention was to provide the classical implementation of what was originally
*    done in script so default usage of the processor works without supplemental scripting.
*
*******************************************************************************/

#include "WsfDefaultDisseminationImpl.hpp"

#include <memory>

#include "iadsLib/NumericUtils.hpp"
#include "iadsLib/util.hpp"
#include "logger.hpp"

#include "WsfPlatform.hpp"
#include "WsfWeapon.hpp"

#include "WsfBMDisseminateC2.hpp"
#include "WsfAssetManager.hpp"
#include "WsfBattleManager.hpp"
#include "WsfBMCueMessage.hpp"
#include "WsfBMAssignmentTrackMessage.hpp"
#include "WsfBMUtils.hpp"
#include "WsfIADSC2ScenarioExtension.hpp"

namespace
{
   template <class tMsgWrapperType, int MSG_TYPE>
   std::vector<tMsgWrapperType> GetOutgoingMessages(const double sim_time_s, WsfBMDisseminateC2* dc2)
   {
      std::vector<tMsgWrapperType> messages;
      auto& outgoing_msgs = dc2->GetCoreDC2Ref()->getOutgoingMessages();
      auto dc2_ref = dc2->GetCoreDC2Ref();

      for (auto it = outgoing_msgs.begin(); it != outgoing_msgs.end(); ++it)
      {
         auto& msg_event = *it;
         if (msg_event->getMessageType() == MSG_TYPE)
         {
            if (msg_event->isTimeToProcess(sim_time_s))
            {
               if (msg_event->isLatentProcessFlagSet())
               {
                  msg_event->resetLatentProcessFlags();   // reset flags to avoid confusion on passing off
               }
               messages.push_back(tMsgWrapperType(dc2->GetPlatform(), msg_event));
            }
            else
            {
               HCL_TRACE_LOGGER(dc2->GetCoreAMRef()->getGlobalLogger()) << sim_time_s << ": " << dc2->GetPlatform()->GetName() << ": " << msg_event->getMessageTypeString() << " message was marked for delay of " << msg_event->getLatentProcessTime() - sim_time_s;
               dc2_ref->addPendingOutMessage(msg_event);
            }
         }
      }

      return messages;
   }

   template <>
   std::vector<WsfBMAssignmentTrackMessage> GetOutgoingMessages<WsfBMAssignmentTrackMessage, il::ASSIGNMENT_TRACK_MESSAGE>(const double sim_time_s, WsfBMDisseminateC2* dc2)
   {
      std::vector<WsfBMAssignmentTrackMessage> messages;
      auto& outgoing_msgs = dc2->GetCoreDC2Ref()->getOutgoingMessages();
      auto dc2_ref = dc2->GetCoreDC2Ref();

      for (auto it = outgoing_msgs.begin(); it != outgoing_msgs.end(); ++it)
      {
         auto& msg_event = *it;
         if (msg_event->getMessageType() == il::ASSIGNMENT_TRACK_MESSAGE)
         {
            if (msg_event->isTimeToProcess(sim_time_s))
            {
               if (msg_event->isLatentProcessFlagSet())
               {
                  msg_event->resetLatentProcessFlags();   // reset flags to avoid confusion on passing off
               }

               auto this_plat = dc2->GetPlatform();
               WsfBMAssignmentTrackMessage at(dc2->GetPlatform(), msg_event);
               // set the track
               if (auto master_track = this_plat->GetMasterTrackList().FindTrack(WsfBMUtils::ConvertTrackId(std::static_pointer_cast<il::assignmentTrackMessage>(msg_event)->getTrack().getID())))
               {
                  at.SetTrack(dc2->GetSimulation(), *master_track);
                  messages.push_back(at);
               }
               else
               {
                  HCL_ERROR_LOGGER(dc2->GetCoreAMRef()->getGlobalLogger()) << "Attempt to send assignment track but master track is lost..." << std::endl;
               }
            }
            else
            {
               HCL_TRACE_LOGGER(dc2->GetCoreAMRef()->getGlobalLogger()) << sim_time_s << ": " << dc2->GetPlatform()->GetName() << ": " << msg_event->getMessageTypeString() << " message was marked for delay of " << msg_event->getLatentProcessTime() - sim_time_s;
               dc2_ref->addPendingOutMessage(msg_event);
            }
         }
      }

      return messages;
   }

   template <class tMessage>
   bool SendMessage(double aSimTime, const tMessage& aMessage, WsfBMDisseminateC2* dc2)
   {
      return dc2->SendMessage(aSimTime, aMessage, WsfBMUtils::GetPlatformFromUnitIDRecord(dc2->GetSimulation(), aMessage.GetRawMessagePtr()->getDestinationID()));
   }

   template <class tMessage>
   void HandleOutgoingMessages(const double sim_time_s, std::vector<tMessage>& messages, WsfBMDisseminateC2* dc2)
   {
      for (auto it = messages.begin(); it != messages.end(); ++it)
      {
         auto& message = *it;
         SendMessage(sim_time_s, message, dc2);
      }
   }
}

WsfDefaultDisseminationImpl::WsfDefaultDisseminationImpl()
{
}

bool WsfDefaultDisseminationImpl::ProcessInput(UtInput& aInput)
{
   bool processed = true;

   std::string command = aInput.GetCommand();

   if (command == "send_direct_assignment_to_weapon_plat")
   {
      std::string direct_to_plat;
      aInput.ReadValue(direct_to_plat);
      m_direct_assigned_weapons.insert(direct_to_plat);
   }
   else
      // unknown block
   {
      processed = false;
   }

   return processed;
}

void WsfDefaultDisseminationImpl::Initialize(WsfBMDisseminateC2* dc2)
{
   HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(dc2->GetScenario())) << dc2->GetPlatform()->GetName() << "/" << dc2->GetName() << ": " << dc2->GetScriptClassName() << ": in on_initialize" << std::endl;
}

void WsfDefaultDisseminationImpl::Initialize2(WsfBMDisseminateC2* dc2)
{
   HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(dc2->GetScenario())) << dc2->GetPlatform()->GetName() << "/" << dc2->GetName() << ": " << dc2->GetScriptClassName() << ": in on_initialize2" << std::endl;

   // locate the other managers
   m_am_ref = WsfBMUtils::FindAttachedAssetManagerRef2(dc2->GetPlatform(), true);

   // just some debugging carryover from script - may eventually delete
   //for (unsigned proc_idx = 0; proc_idx < dc2->GetPlatform()->GetProcessorCount(); ++proc_idx)
   for (unsigned proc_idx = 0; proc_idx < dc2->GetPlatform()->GetComponentCount<WsfProcessor>(); ++proc_idx)
   {
      //auto proc = dc2->GetPlatform()->GetProcessorEntry (proc_idx);
      auto proc = dc2->GetPlatform()->GetComponentEntry<WsfProcessor>(proc_idx);

      if (proc->IsA_TypeOf("WSF_BATTLE_MANAGER")) // TODO: replace with WsfScriptBattleManagerClass::BASE_CLASS_NAME? Would require incl header.
      {
         HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(dc2->GetScenario())) << "Disseminate C2: found local battle manager: proc name = " << proc->GetName() << " type = " << proc->GetType() << std::endl;
      }
   }

   if (!m_am_ref)
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(dc2->GetScenario())) << "Disseminate C2: could not locate asset manager - all platforms on the C2 network must have an asset manager!" << std::endl;
   }
}

void WsfDefaultDisseminationImpl::on_update(const double sim_time_s, WsfPlatform* this_plat, WsfBMDisseminateC2* dc2)
{
   // dispatch any delayed messages that are now ready to go
   HandleDelayedMessages(sim_time_s, this_plat, dc2);

   // collect message events
   dc2->GetCoreDC2Ref()->updateOutgoingMessages(); // PROCESSOR.Run();

   // asset status
   {
      std::vector<WsfBMStatusMessage> messages = GetOutgoingMessages<WsfBMStatusMessage, il::COMBAT_STATUS_MESSAGE>(sim_time_s, dc2);
      HandleOutgoingMessages(sim_time_s, messages, dc2);
   }

   // assignment tracks
   {
      std::vector<WsfBMAssignmentTrackMessage> messages = GetOutgoingMessages<WsfBMAssignmentTrackMessage, il::ASSIGNMENT_TRACK_MESSAGE>(sim_time_s, dc2);
      HandleOutgoingMessages(sim_time_s, messages, dc2);
   }

   // assignment sending
   {
      std::vector<WsfBMAssignmentMessage> messages = GetOutgoingMessages<WsfBMAssignmentMessage, il::ASSIGNMENT_MESSAGE>(sim_time_s, dc2);
      HandleOutgoingAssignments(sim_time_s, messages, dc2);
   }

   // assignment status
   {
      std::vector<WsfBMAssignmentStatusMessage> messages = GetOutgoingMessages<WsfBMAssignmentStatusMessage, il::ASSIGNMENT_ACK_MESSAGE>(sim_time_s, dc2);
      HandleOutgoingMessages(sim_time_s, messages, dc2);
   }

   // sensor cues
   {
      std::vector<WsfBMCueMessage> messages = GetOutgoingMessages<WsfBMCueMessage, il::CUE_MESSAGE>(sim_time_s, dc2);
      HandleOutgoingMessages(sim_time_s, messages, dc2);
   }

   dc2->GetCoreDC2Ref()->clearOutgoingMessages(); // PROCESSOR.ClearOutgoingMessages();
}

bool WsfDefaultDisseminationImpl::on_message(const double sim_time_s, const WsfMessage& message, WsfBMDisseminateC2* dc2)
{
   // if we don't want to block other processors from recieving the message, we must keep this flag false,
   // otherwise nobody else will get a chance to process it. We do this so that other processors have a shot regardless if
   // we process it or not.
   bool block_processing_message = false;

   // all messages get dispatched to the asset manager
   if (m_am_ref)
   {
      bool delay_msg = false;
      double delay_amount = -1;
      std::tie(delay_msg, delay_amount) = m_am_ref->ShouldDelayIncomingMessage(sim_time_s, message);
      if (!delay_msg)
      {
         m_am_ref->ProcessMessage(sim_time_s, message);
      }
      else
      {
         HCL_TRACE_LOGGER(m_am_ref->GetCoreAMRef()->getGlobalLogger()) << sim_time_s << ": " << dc2->GetPlatform()->GetName() << ": Received message marked for delay. Will delay by " << delay_amount << " seconds.";
         // postpone message by delay amount + sim_time and queue it for dispatch later,
         // don't forget to also block other processors from getting it now
         m_delayed_messages.push_back(std::move(std::make_pair(sim_time_s + delay_amount, std::shared_ptr<WsfMessage>(message.Clone()))));

         block_processing_message = true;
      }
   }
   else
   {
      HCL_ERROR_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(dc2->GetScenario())) << "WsfDefaultDisseminationImpl::on_message(): no asset manager to dispatch message to..." << std::endl;
   }

   return block_processing_message;
}

void WsfDefaultDisseminationImpl::HandleOutgoingAssignments(const double sim_time_s, std::vector<WsfBMAssignmentMessage>& messages, WsfBMDisseminateC2* dc2)
{
   for (auto it = messages.begin(); it != messages.end(); ++it)
   {
      auto& message = *it;
      bool is_new_assignment = !(message.GetRawMessagePtr()->getAssignmentReason() == il::assignmentMessage::CANCEL);

      if (!SendMessage(sim_time_s, message, dc2))
      {
         // wasn't a cancel
         if (is_new_assignment)
            // cancel with a global systemic CANTCO
         {
            m_am_ref->GetCoreAMRef()->cantcoAssignment(*IADSC2SimulationExtension::Find(*dc2->GetSimulation())->GetLogger(), std::const_pointer_cast<il::assignmentMessage>(message.GetRawMessagePtr()), "Communications Error", true, true);
         }
      }
      else
      {
         auto sent_assignment = m_am_ref->GetCoreAMRef()->getAssignment(message.GetRawMessagePtr()->getLocalTrackID(), message.GetRawMessagePtr()->getAssignedID());
      }
   }
}

void WsfDefaultDisseminationImpl::HandleDelayedMessages(const double sim_time_s, WsfPlatform* this_plat, WsfBMDisseminateC2* dc2)
{
   for (auto pending_iter = m_delayed_messages.begin(); pending_iter != m_delayed_messages.end(); )
   {
      if (NumericUtils::GreaterThanEqual(sim_time_s, pending_iter->first))
      {
         HCL_TRACE_LOGGER(m_am_ref->GetCoreAMRef()->getGlobalLogger()) << sim_time_s << ": " << dc2->GetPlatform()->GetName() << ": Time to dispatch delayed message.";
         // dispatch message
         m_am_ref->ProcessMessage(sim_time_s, *pending_iter->second);

         pending_iter = m_delayed_messages.erase(pending_iter);
      }
      else
      {
         ++pending_iter;
      }
   }
}
