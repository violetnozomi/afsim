// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCYBERTRACKPROCESSOREFFECT_HPP
#define WSFCYBERTRACKPROCESSOREFFECT_HPP

#include <vector>

#include "WsfCyberTrackManagerEffect.hpp"

class WsfTrackProcessor;

namespace wsf
{
namespace cyber
{

//! This effect is a specialization of the TrackManagerEffect specifically
//! targeted for usage on the WsfTrackProcessor object. A WsfTrackProcessor
//! may use the platform based track manager, or may use its own track manager
//! based upon the user definition. This effect will target the track manager
//! being used by the victim WsfTrackProcessor for effects relating to the manipulation
//! of track processing.
class TrackProcessorEffect : public TrackManagerEffect
{
public:
   using ProcessorVector = std::vector<WsfTrackProcessor*>;

   TrackProcessorEffect(WsfScenario& aScenario);
   ~TrackProcessorEffect() override = default;

   TrackProcessorEffect& operator=(const TrackProcessorEffect& aRhs) = delete;

   TrackProcessorEffect* Clone() const override;
   bool                  ProcessInput(UtInput& aInput) override;

   void Attack(double aSimTime, Engagement& aEngagement) override;

   void Restore(double aSimTime, Engagement& aEngagement) override;

   ProcessorVector GetValidProcessors(const WsfPlatform& aPlatform) const;

protected:
   TrackProcessorEffect(const TrackProcessorEffect& aSrc);
   TrackProcessorEffect(TrackProcessorEffect&& aSrc) = default;

   void InjectTrackManager(double aSimTime, const Engagement& aEngagement) override;

   const std::set<std::string>&      GetProcessorTargets() const { return mProcessorTargets; }
   const std::vector<TrackManager*>& GetTargetManagers() const { return mInjectedManagers; }

private:
   std::set<std::string>      mProcessorTargets{};
   std::vector<TrackManager*> mInjectedManagers{};
};

} // namespace cyber
} // namespace wsf

#endif
