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
*   File: WsfScriptOverridableProcessor.hpp
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
*   Abstract:
*
*******************************************************************************/

#ifndef WSF_SCRIPT_OVERRIDABLE_PROCESSOR_HPP
#define WSF_SCRIPT_OVERRIDABLE_PROCESSOR_HPP

#include <UtInput.hpp>

#include <WsfBMCommon.hpp>

class WSF_IADS_C2_LIB_EXPORT WsfScriptOverridableProcessor
{
   public:
      WsfScriptOverridableProcessor(const bool autonomous_updates)
         : m_override_internal_initialization(false)
         , m_override_internal_initialization2(false)
         , m_override_internal_on_update(false)
         , m_override_internal_on_message(false)
         , m_override_internal_receive_message(false)
         , m_override_internal_local_track_drop(false)
         , m_override_internal_local_track_initiated(false)
         , m_override_internal_local_track_updated(false)
         , m_autonomous_updates(autonomous_updates)
      { }

      // pure abstract interface
      virtual void Run(const double sim_time_s) = 0;

      inline virtual bool ProcessInput(UtInput& aInput)
      {
         bool myCommand = true;

         std::string command = aInput.GetCommand();

         if (command == "override_internal_initialization")
         {
            m_override_internal_initialization = aInput.ReadBool();
         }
         else if (command == "override_internal_initialization2")
         {
            m_override_internal_initialization2 = aInput.ReadBool();
         }
         else if (command == "override_internal_on_update")
         {
            m_override_internal_on_update = aInput.ReadBool();
         }
         else if (command == "override_internal_on_message")
         {
            m_override_internal_initialization2 = aInput.ReadBool();
         }
         else if (command == "override_internal_receive_message")
         {
            m_override_internal_receive_message = aInput.ReadBool();
         }
         else if (command == "override_internal_local_track_drop")
         {
            m_override_internal_local_track_drop = aInput.ReadBool();
         }
         else if (command == "override_internal_local_track_initiated")
         {
            m_override_internal_local_track_initiated = aInput.ReadBool();
         }
         else if (command == "override_internal_local_track_updated")
         {
            m_override_internal_local_track_updated = aInput.ReadBool();
         }
         else if (command == "autonomous_updates")
         {
            m_autonomous_updates = aInput.ReadBool();
         }
         else // unknown block
         {
            myCommand = false;
         }

         return myCommand;
      }

      inline virtual bool OverrideInternalInit() const { return m_override_internal_initialization; }
      inline virtual bool OverrideInternalInit2() const { return m_override_internal_initialization2; }
      inline virtual bool OverrideInternalOnUpdate() const { return m_override_internal_on_update; }
      inline virtual bool OverrideInternalOnMessage() const { return m_override_internal_on_message; }
      inline virtual bool OverrideInternalReceiveMessage() const { return m_override_internal_receive_message; }
      inline virtual bool OverrideInternalLocalTrackDropped() const { return m_override_internal_local_track_drop; }
      inline virtual bool OverrideInternalLocalTrackInitiated() const { return m_override_internal_local_track_initiated; }
      inline virtual bool OverrideInternalLocalTrackUpdated() const { return m_override_internal_local_track_updated; }

      inline virtual bool AllowAutonomousUpdates() const { return m_autonomous_updates; }

   protected:
      // behavior overrides
      bool m_override_internal_initialization;
      bool m_override_internal_initialization2;
      bool m_override_internal_on_update;
      bool m_override_internal_on_message;
      bool m_override_internal_receive_message;
      bool m_override_internal_local_track_drop;
      bool m_override_internal_local_track_initiated;
      bool m_override_internal_local_track_updated;

      bool m_autonomous_updates;
};

#endif
