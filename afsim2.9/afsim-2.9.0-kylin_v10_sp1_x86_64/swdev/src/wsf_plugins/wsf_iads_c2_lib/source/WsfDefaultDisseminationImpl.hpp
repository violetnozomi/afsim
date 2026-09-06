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
*   File: WsfDefaultDisseminationImpl.hpp
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

#ifndef WSF_DEFAULT_DISSEMINATION_IMPL_HPP
#define WSF_DEFAULT_DISSEMINATION_IMPL_HPP

#include <string>
#include <vector>
#include <set>
#include <utility>
#include <memory>

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

class WSF_IADS_C2_LIB_EXPORT WsfDefaultDisseminationImpl
{
   public:
      WsfDefaultDisseminationImpl();

      virtual bool ProcessInput(UtInput& aInput);
      virtual void Initialize(WsfBMDisseminateC2* dc2);
      virtual void Initialize2(WsfBMDisseminateC2* dc2);

      virtual void on_update(const double sim_time_s, WsfPlatform* this_plat, WsfBMDisseminateC2* dc2);
      virtual bool on_message(const double sim_time_s, const WsfMessage& message, WsfBMDisseminateC2* dc2);

   protected:
      void HandleOutgoingAssignments(const double sim_time_s, std::vector<WsfBMAssignmentMessage>& messages, WsfBMDisseminateC2* dc2);
      void HandleDelayedMessages(const double sim_time_s, WsfPlatform* this_plat, WsfBMDisseminateC2* dc2);

   private:
      WsfAssetManager*  m_am_ref; // we don't own these allow slices
      std::vector<std::pair<double, std::shared_ptr<WsfMessage>>> m_delayed_messages; // unique_ptr here has bad semantic for exporting an interface
      std::set <std::string> m_direct_assigned_weapons; // really for testing purposes but will override message destination to go direct if assigned to
};

#endif
