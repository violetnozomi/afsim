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
*   File: WsfDefaultBattleManagerImpl.hpp
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
*    what could be overridden in script for the Battle Manager base class.
*    The intention was to provide the classical implementation of what was originally
*    done in script so default usage of the processor works without supplemental scripting.
*
*******************************************************************************/

#ifndef WSF_DEFAULT_BATTLE_MANAGER_IMPL_HPP
#define WSF_DEFAULT_BATTLE_MANAGER_IMPL_HPP

#include <string>
#include <set>

#include "UtInput.hpp"
#include "WsfMessage.hpp"

#include "WsfBMCommon.hpp"

// forward declarations
class WsfPlatform;
class WsfBattleManager;

class WSF_IADS_C2_LIB_EXPORT WsfDefaultBattleManagerImpl
{
   public:
      WsfDefaultBattleManagerImpl();

      virtual bool ProcessInput(UtInput& aInput);
      virtual void Initialize(WsfBattleManager* bm);
      virtual void Initialize2(WsfBattleManager* bm);

      virtual void on_update(const double sim_time_s, WsfPlatform* this_plat, WsfBattleManager* bm);
      virtual bool on_message(const double sim_time_s, const WsfMessage& message, WsfBattleManager* bm);
};

#endif
