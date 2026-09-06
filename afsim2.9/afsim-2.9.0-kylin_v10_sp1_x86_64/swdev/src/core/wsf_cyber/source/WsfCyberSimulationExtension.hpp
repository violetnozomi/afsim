// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERSIMULATIONEXTENSION_HPP
#define WSFCYBERSIMULATIONEXTENSION_HPP

#include <map>
#include <memory>
#include <vector>

#include "UtCallbackHolder.hpp"
#include "WsfCyberEngagement.hpp"
#include "WsfCyberEngagementManager.hpp"
#include "WsfCyberEventManager.hpp"
#include "WsfCyberEventPipe.hpp"
#include "WsfCyberObserver.hpp"
#include "WsfCyberRandom.hpp"
#include "WsfCyberVisualizationManager.hpp"
#include "triggers/WsfCyberTriggerManager.hpp"
class WsfPlatform;
#include "WsfSimulationExtension.hpp"

namespace wsf
{
namespace cyber
{

class WSF_CYBER_EXPORT SimulationExtension : public WsfSimulationExtension
{
public:
   static SimulationExtension& Get(const WsfSimulation& aSimulation);

   void AddedToSimulation() override;
   bool Initialize() override;

   Observer&              GetObserver() { return mObserver; }
   random::RandomManager& GetCyberDrawManager() { return *mDrawManagerPtr; }
   EngagementManager&     GetCyberEngagementManager() { return mEngagementManager; }
   VisualizationManager&  GetCyberVisualizationManager() { return mVisualizationManager; }
   trigger::Manager&      GetCyberTriggerManager() { return *mTriggerManagerPtr; }
   EventManager&          GetCyberEventManager() { return *mEventManagerPtr; }

   void PlatformDeleted(double aSimTime, WsfPlatform* aPlatformPtr);

private:
   Observer                               mObserver;
   std::unique_ptr<random::RandomManager> mDrawManagerPtr{nullptr};
   EngagementManager                      mEngagementManager;
   VisualizationManager                   mVisualizationManager;
   std::unique_ptr<EventPipe>             mEventPipePtr;
   std::unique_ptr<trigger::Manager>      mTriggerManagerPtr;
   std::unique_ptr<EventManager>          mEventManagerPtr{nullptr};

   UtCallbackHolder mCallbacks;
};

} // namespace cyber
} // namespace wsf

#endif
