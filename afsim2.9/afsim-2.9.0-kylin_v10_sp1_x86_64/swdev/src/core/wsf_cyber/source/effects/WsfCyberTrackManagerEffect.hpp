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

#ifndef WSFCYBERTRACKMANAGEREFFECT_HPP
#define WSFCYBERTRACKMANAGEREFFECT_HPP

#include "wsf_cyber_export.h"

#include <set>
#include <string>

#include "WsfCyberEffect.hpp"

class UtInput;
class UtScript;
class WsfScenario;
#include "WsfTrackManager.hpp"
#include "script/WsfScriptContext.hpp"

namespace wsf
{
namespace cyber
{

class Engagement;
class TrackManager;

//! A derived cyber effect for direct manipulation of a track manager object.
//! In conjunction with the derived cyber track manager object, this effect
//! replaces the specified track manager with the derived type. This allows
//! added functionality by granting the user the ability to provide script method
//! overloads that introduce logic that is called on any of the exposed conditions
//! to potentially alter how the track manager behaves. These are specifically limited
//! to conditions where a track is to be introduced to or removed from the track manager.
//! Thus, the user may prevent a track drop or add, or modify the track in some way
//! when the script based logic is called prior to any of the standard track manager calls.
//!
//! This effect is generic in nature, in that it only affects the track manager held specifically
//! by the intended victim platform itself. As such, ANY processor that reports to this track manager
//! will be affected.
class TrackManagerEffect : public Effect
{
public:
   TrackManagerEffect(WsfScenario& aScenario);
   ~TrackManagerEffect() override = default;

   TrackManagerEffect& operator=(const TrackManagerEffect& aRhs) = delete;

   TrackManagerEffect* Clone() const override;
   bool                Initialize(Engagement& aEngagement) override;
   bool                ProcessInput(UtInput& aInput) override;

   void Attack(double aSimTime, Engagement& aEngagement) override;

   void Restore(double aSimTime, Engagement& aEngagement) override;

   WsfPlatform* GetValidTarget(const Engagement& aEngagement);

protected:
   TrackManagerEffect(const TrackManagerEffect& aSrc);
   TrackManagerEffect(TrackManagerEffect&& aSrc) = default;

   virtual void InjectTrackManager(double aSimTime, const Engagement& aEngagement);

   const std::shared_ptr<WsfScriptContext>& GetContext() const { return mContextPtr; }
   const std::set<std::string>&             GetTargets() const { return mTargets; }
   UtScript*                                GetOnEntry() const { return mOnEntry; }
   UtScript*                                GetOnExit() const { return mOnExit; }
   UtScript*                                GetOnTrack() const { return mOnTrack; }
   UtScript*                                GetOnRawTrack() const { return mOnRawTrack; }
   UtScript*                                GetOnTrackDrop() const { return mOnTrackDrop; }
   TrackManager*                            GetInjectedManager() const { return mInjectedManagerPtr; }
   bool                                     GetTrackPurging() const { return mInactiveTrackPurging; }
   bool                                     GetRawTrackPurging() const { return mInactiveRawTrackPurging; }
   bool                                     GetTrackHistoryPurging() const { return mLocalTrackHistoryPurging; }

private:
   std::shared_ptr<WsfScriptContext> mContextPtr;

   std::set<std::string> mTargets{};

   UtScript* mOnEntry{nullptr};
   UtScript* mOnExit{nullptr};
   UtScript* mOnTrack{nullptr};
   UtScript* mOnRawTrack{nullptr};
   UtScript* mOnTrackDrop{nullptr};

   TrackManager* mInjectedManagerPtr{nullptr};

   bool mInactiveTrackPurging{true};
   bool mInactiveRawTrackPurging{true};
   bool mLocalTrackHistoryPurging{true};
};

} // namespace cyber
} // namespace wsf

#endif
