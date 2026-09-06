// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// Classification: UNCLASSIFIED

#ifndef WSF_BM_STATUS_MESSAGE_HPP
#define WSF_BM_STATUS_MESSAGE_HPP

#include <memory>
#include <WsfBMCommon.hpp>
#include <WsfMessage.hpp>
#include <iadsLib/combatStatusMessage.hpp>
#include "WsfBMGenericTypeWrapper.hpp"
#include <UtVec3.hpp>
#include <WsfGeoPoint.hpp>
#include <UtNullCreate.hpp>

class WsfPlatform;
class WsfWeapon;
class WsfSimulation;

// currently, everything gets sliced
class WsfBMStatusMessage : public WsfMessage, public WsfBMManagedTypeWrapper<il::baseMessage>
{
   public:
      //WsfBMStatusMessage (const UtNullCreate& aNull);
      //WsfBMStatusMessage(WsfSimulation *simPtr);
      WsfBMStatusMessage(std::weak_ptr<il::logger::GlobalLogger> aGlobalLogger);
      WsfBMStatusMessage(WsfPlatform* aPlatformPtr);
      WsfBMStatusMessage(WsfPlatform* aPlatformPtr, const std::shared_ptr<il::baseMessage>& message);

      WsfBMStatusMessage(const WsfBMStatusMessage& aSrc);

      virtual ~WsfBMStatusMessage();

      WsfBMStatusMessage* Clone() const override;

      const char* GetScriptClassName() const override;
      virtual const char* GetBaseClassName();
      static WsfStringId GetTypeId();

      void SetStatusTime(const double sim_time_s);
      double GetStatusTime() const;

      void SetPlatform(const WsfPlatform* aPlatformPtr);
      const WsfPlatform* GetPlatform(const WsfSimulation* simPtr) const;
      std::string GetPlatformName() const;

      void SetMaxAssignments(const int max_assignments);
      int GetMaxAssignments() const;

      void SetNumAssignments(const int num_assignments);
      int GetNumAssignments() const;

      void SetWeaponStatus(const WsfPlatform* shooter, const WsfWeapon* weapon);

      enum eSystemStatus { E_SYS_WHITE, E_SYS_YELLOW, E_SYS_GREEN, E_SYS_RED };
      void SetSystemStatus(const eSystemStatus& status);
      eSystemStatus GetSystemStatus() const;

      void SetPosition(const double sim_time_s, const WsfGeoPoint& location);
      bool HasPosition() const;
      const WsfGeoPoint GetPosition() const;
      double GetPositionTime() const;

      void SetVelocityECEF(const UtVec3d& vel);
      bool HasVelocity() const;
      const UtVec3d GetVelocityECEF() const;

      inline std::shared_ptr<const il::combatStatusMessage> GetRawMessagePtr() const { return std::static_pointer_cast <il::combatStatusMessage>(m_wrapped_ptr); }

   private:
      inline std::shared_ptr<il::combatStatusMessage> GetMutableRawMessagePtr() { return std::static_pointer_cast <il::combatStatusMessage>(m_wrapped_ptr); }
};

#endif
