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
*   File: WsfBMAssignmentMessage.hpp
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

#ifndef WSF_BM_ASSIGNMENT_MESSAGE_HPP
#define WSF_BM_ASSIGNMENT_MESSAGE_HPP

#include <memory>
#include <WsfBMCommon.hpp>

#include <WsfMessage.hpp>
#include <WsfLocalTrack.hpp>

#include <iadsLib/assignmentMessage.hpp>
#include "WsfBMGenericTypeWrapper.hpp"

class WsfPlatform;
class WsfSimulation;
class WsfWeapon;

class WsfBMAssignmentMessage : public WsfMessage, public WsfBMManagedTypeWrapper<il::baseMessage>
{
   public:
      WsfBMAssignmentMessage(WsfSimulation* simPtr);
      WsfBMAssignmentMessage(WsfPlatform* plat);
      WsfBMAssignmentMessage(WsfPlatform* plat, const std::shared_ptr<il::baseMessage>& message);

      WSF_IADS_C2_LIB_EXPORT WsfBMAssignmentMessage(const WsfBMAssignmentMessage& aSrc);

      virtual WSF_IADS_C2_LIB_EXPORT ~WsfBMAssignmentMessage();

      WsfBMAssignmentMessage* Clone() const override;

      const char* GetScriptClassName() const override;
      virtual const char* GetBaseClassName();
      static WsfStringId GetTypeId();

      void SetAssignmentTime(const double sim_time_s);
      double GetAssignmentTime() const;

      void SetAssignedWeapon(const WsfPlatform* plat, const WsfWeapon* weapon);
      const WsfPlatform* GetAssignedPlatform(WsfSimulation* sim_ptr) const;
      const WsfWeapon* GetAssignedWeapon(WsfSimulation* sim_ptr) const;

      void SetAssignmentReferenceTrackID(const WsfTrackId& track_id);
      const WsfTrackId GetAssignmentReferenceTrackID() const;

      void SetAssignmentLocalTrackID(const WsfTrackId& track_id);
      const WsfTrackId GetAssignmentLocalTrackID() const;

      void SetInitiatingPlatform(const WsfPlatform* plat);
      const WsfPlatform* GetInitiatingPlatform(WsfSimulation* sim_ptr) const;

      void SetAssigningPlatform(const WsfPlatform* plat);
      const WsfPlatform* GetAssigningPlatform(WsfSimulation* sim_ptr) const;

      void SetSelfDefenseFlag(const bool is_self_defense);
      bool GetSelfDefenseFlag() const;

      void SetMaxFiringTime(const double sim_time_s);
      double GetMaxFiringTime() const;

      void SetAssignmentReason(const il::assignmentMessage::AssignmentReason reason);
      il::assignmentMessage::AssignmentReason GetAssignmentReason() const;
      std::string GetAssignmentReasonString() const;

      void SetShotDoctrine(const il::eShotDoctrine shot_doctrine);
      il::eShotDoctrine GetShotDoctrine() const;
      std::string GetshotDoctrineString() const;

      void Print() const;

      inline std::shared_ptr<const il::assignmentMessage> GetRawMessagePtr() const { return std::static_pointer_cast <il::assignmentMessage>(m_wrapped_ptr); }

   private:
      inline std::shared_ptr<il::assignmentMessage> GetMutableRawMessagePtr() { return std::static_pointer_cast <il::assignmentMessage>(m_wrapped_ptr); }
};

#endif
