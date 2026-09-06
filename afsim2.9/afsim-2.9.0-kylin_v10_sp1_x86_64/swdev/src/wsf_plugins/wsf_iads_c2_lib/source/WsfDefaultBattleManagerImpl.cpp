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
*   File: WsfDefaultBattleManagerImpl.cpp
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

#include <WsfDefaultBattleManagerImpl.hpp>

#include <WsfPlatform.hpp>

#include <WsfBMDisseminateC2.hpp>
#include <WsfAssetManager.hpp>
#include <WsfBattleManager.hpp>
#include <WsfBMUtils.hpp>
#include <logger.hpp>

#include <iadsLib/util.hpp>
#include "WsfIADSC2ScenarioExtension.hpp"

WsfDefaultBattleManagerImpl::WsfDefaultBattleManagerImpl()
{
}

bool WsfDefaultBattleManagerImpl::ProcessInput(UtInput& aInput)
{
   bool processed = false;

   return processed;
}

void WsfDefaultBattleManagerImpl::Initialize(WsfBattleManager* bm)
{
   HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(bm->GetScenario())) << bm->GetPlatform()->GetName() << "/" << bm->GetName() << ": " << bm->GetScriptClassName() << ": in on_initialize" << std::endl;
}

void WsfDefaultBattleManagerImpl::Initialize2(WsfBattleManager* bm)
{
   HCL_DEBUG_LOGGER(IADSC2ScenarioExtension::GetGlobalLogger(bm->GetScenario())) << bm->GetPlatform()->GetName() << "/" << bm->GetName() << ": " << bm->GetScriptClassName() << ": in on_initialize2" << std::endl;
}

void WsfDefaultBattleManagerImpl::on_update(const double sim_time_s, WsfPlatform* this_plat, WsfBattleManager* bm)
{
   bm->GetCoreBMRef()->run(*IADSC2SimulationExtension::Find(*this_plat->GetSimulation())->GetLogger());
}

bool WsfDefaultBattleManagerImpl::on_message(const double sim_time_s, const WsfMessage& message, WsfBattleManager* bm)
{
   // if we don't want to block other processors from recieving the message, we must keep this flag false,
   // otherwise nobody else will get a chance to process it. We do this so that other processors have a shot regardless if
   // we process it or not.
   bool block_processing_message = false;

   // nothing to do...

   return block_processing_message;
}
