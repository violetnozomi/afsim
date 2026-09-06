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
*   File: WsfDefaultWeaponsManagerImpl.hpp
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
*    what could be overridden in script for the Weapons Manager base class.
*    The intention was to provide the classical implementation of what was originally
*    done in script so default usage of the processor works without supplemental scripting.
*
*******************************************************************************/

#ifndef WSF_DEFAULT_WEAPONS_MANAGER_IMPL_HPP
#define WSF_DEFAULT_WEAPONS_MANAGER_IMPL_HPP

#include "UtPlugin.hpp"
#include <string>
#include <set>

#include <UtInput.hpp>
#include <WsfMessage.hpp>

#include <WsfBMCommon.hpp>
#include <WsfBMAssignmentMessage.hpp>
#include <WsfBMAssignmentStatusMessage.hpp>
#include <WsfBMStatusMessage.hpp>

// forward declarations
class WsfBMDisseminateC2;
class WsfPlatform;
class WsfAssetManager;
class WsfBattleManager;
class WsfWeapon;
class WsfBMWeaponsManager;

class WSF_IADS_C2_LIB_EXPORT WsfDefaultWeaponsManagerImpl
{
   public:
      WsfDefaultWeaponsManagerImpl();

      virtual bool ProcessInput(UtInput& aInput);
      virtual void Initialize(WsfBMWeaponsManager* wm);
      virtual void Initialize2(WsfBMWeaponsManager* wm);

      virtual void on_update(const double sim_time_s, WsfPlatform* this_plat, WsfBMWeaponsManager* wm);
      virtual bool on_message(const double sim_time_s, const WsfMessage& message, WsfBMWeaponsManager* wm);

   protected:
      void PerformSelfDefense(const double sim_time_s, WsfPlatform* this_plat, WsfBMWeaponsManager* wm);
      bool PerformSelfDefenseProcessing(const double sim_time_s, WsfPlatform* this_plat, WsfBMWeaponsManager* wm, const std::shared_ptr<il::trackRecord>& track);
      bool CreateSelfDefenseAssignment(const double sim_time_s, WsfPlatform* this_plat, WsfBMWeaponsManager* wm, const std::shared_ptr<il::trackRecord>& track);
      std::shared_ptr<il::weaponRecord> SelectSelfDefenseWeapon(const double sim_time_s, WsfPlatform* this_plat, WsfBMWeaponsManager* wm, const std::shared_ptr<il::trackRecord>& track);

   private:
      WsfAssetManager*  m_am_ref; // we don't own these allow slices
};

#endif
