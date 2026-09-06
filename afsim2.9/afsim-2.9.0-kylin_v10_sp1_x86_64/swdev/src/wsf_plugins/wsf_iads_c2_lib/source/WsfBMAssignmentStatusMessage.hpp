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
*   File: WsfBMAssignmentStatusMessage.hpp
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

#ifndef WSF_BM_ASSIGNMENT_STATUS_MESSAGE_HPP
#define WSF_BM_ASSIGNMENT_STATUS_MESSAGE_HPP

#include <memory>
#include <WsfBMCommon.hpp>

#include <WsfMessage.hpp>
#include <WsfTrackId.hpp>
#include <UtNullCreate.hpp>

#include <iadsLib/assignmentACKMessage.hpp>
#include "WsfBMGenericTypeWrapper.hpp"

class WsfPlatform;
class WsfSimulation;
class WsfWeapon;

class WsfBMAssignmentStatusMessage : public WsfMessage, public WsfBMManagedTypeWrapper<il::baseMessage>
{
   public:
      //WsfBMAssignmentStatusMessage (const UtNullCreate& aNull);
      //WsfBMAssignmentStatusMessage (WsfSimulation *simPtr);
      WsfBMAssignmentStatusMessage(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger);
      WsfBMAssignmentStatusMessage(WsfPlatform* plat);
      WsfBMAssignmentStatusMessage(WsfPlatform* plat, const std::shared_ptr<il::baseMessage>& message);

      WsfBMAssignmentStatusMessage(const WsfBMAssignmentStatusMessage& aSrc);

      virtual ~WsfBMAssignmentStatusMessage();

      WsfBMAssignmentStatusMessage* Clone() const override;

      const char* GetScriptClassName() const override;
      static WsfStringId GetTypeId();

      void SetAckTime(const double sim_time_s);
      double GetAckTime() const;

      bool StatusTimeExists(const il::assignmentACKMessage::eAssignmentStatus status) const;
      double GetStatusTime(const il::assignmentACKMessage::eAssignmentStatus status) const;

      void SetAssignmentReferenceTrackID(const WsfTrackId& track_id);
      WsfTrackId GetAssignmentReferenceTrackID() const;

      void SetAssignedWeapon(const WsfPlatform* plat, const WsfWeapon* weapon);
      const WsfPlatform* GetAssignedPlatform(WsfSimulation* sim_ptr) const;
      const WsfWeapon* GetAssignedWeapon(WsfSimulation* sim_ptr) const;

      void SetInitiatingPlatform(const WsfPlatform* plat);
      const WsfPlatform* GetInitiatingPlatform(WsfSimulation* sim_ptr) const;

      void SetAssigningPlatform(const WsfPlatform* plat);
      const WsfPlatform* GetAssigningPlatform(WsfSimulation* sim_ptr) const;

      void SetAssignmentStatus(const il::assignmentACKMessage::eAssignmentStatus status);
      il::assignmentACKMessage::eAssignmentStatus GetAssignmentStatus() const;

      void SetSalvosFired(const int salvos_fired);
      int GetSalvosFired() const;

      void SetCantcoReason(const std::string& reason);
      std::string GetCantcoReason() const;

      void SetSystemicCantcoScope(const il::assignmentACKMessage::eSystemicCantcoScope scope);
      bool IsSystemicCantco() const;
      il::assignmentACKMessage::eSystemicCantcoScope GetSystemicCantcoScope() const;

      // will be null if status is not overridden
      void SetOverridingPlatform(const WsfPlatform* plat);
      const WsfPlatform* GetOverridingPlatform(WsfSimulation* sim_ptr) const;

      // will be null if status is not overridden
      void SetNewlyAssignedWeapon(const WsfPlatform* plat, const WsfWeapon* weapon);
      const WsfPlatform* GetNewlyAssignedPlatform(WsfSimulation* sim_ptr) const;
      const WsfWeapon* GetNewlyAssignedWeapon(WsfSimulation* sim_ptr) const;

      void Print() const;

      inline std::shared_ptr<const il::assignmentACKMessage> GetRawMessagePtr() const { return std::static_pointer_cast <il::assignmentACKMessage>(m_wrapped_ptr); }

   private:
      inline std::shared_ptr<il::assignmentACKMessage> GetMutableRawMessagePtr() { return std::static_pointer_cast <il::assignmentACKMessage>(m_wrapped_ptr); }
};

#endif
