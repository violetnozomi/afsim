// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RvPlatformHistoryWingRibbonData_HPP
#define RvPlatformHistoryWingRibbonData_HPP


#include <osg/Vec3>

#include "UtColor.hpp"

namespace rv
{
class MsgEntityState;
class ResultPlatform;
} // namespace rv

namespace wkf
{
class Platform;

enum TrackableStateValue : int;
enum InterpolationLimitsMethod : int;
} // namespace wkf

namespace vespa
{
class VaViewer;
}

namespace RvPlatformHistory
{
class WingRibbonData
{
public:
   WingRibbonData(wkf::Platform& aPlatform, vespa::VaViewer* aViewerPtr);
   virtual ~WingRibbonData();

   void SetVisible(bool aVisible);
   bool IsVisible() const;

   void SetMainMaxColor(const UtColor& aMainMaxColor);
   void SetMainMinColor(const UtColor& aMainMinColor);
   void SetEdgeMaxColor(const UtColor& aEdgeMaxColor);
   void SetEdgeMinColor(const UtColor& aEdgeMinColor);
   void SetMainMaxFlippedColor(const UtColor& aMainMaxFlippedColor);
   void SetMainMinFlippedColor(const UtColor& aMainMinFlippedColor);
   void SetEdgeMaxFlippedColor(const UtColor& aEdgeMaxFlippedColor);
   void SetEdgeMinFlippedColor(const UtColor& aEdgeMinFlippedColor);

   void SetMainStateBeingTracked(wkf::TrackableStateValue stateToTrack);
   void SetEdgeStateBeingTracked(wkf::TrackableStateValue stateToTrack);
   void SetEdgeLimitType(wkf::InterpolationLimitsMethod aEdgeLimitType);
   void SetMainLimitType(wkf::InterpolationLimitsMethod aMainLimitType);
   void SetHardMaxMainLimit(float aHardMaxMainLimit);
   void SetHardMinMainLimit(float aHardMinMainLimit);
   void SetHardMaxEdgeLimit(float aHardMaxEdgeLimit);
   void SetHardMinEdgeLimit(float aHardMinEdgeLimit);

   void SetWidthScale(float aWidthScale);
   void SetHardEdgeWidth(float aHardEdgeWidth);
   void SetSoftEdgeWidth(float aSoftEdgeWidth);

   void Update(rv::ResultPlatform*            aPlatformData,
               float                          aPruneBefore,
               float                          aPruneAfter,
               const std::pair<float, float>& aLowRange,
               const std::pair<float, float>& aHighRange);

   void        ResetCurrentScenarioLimitsIfNeeded(); // resets if limitType is set to eCURRENT_SCENARIO_LIMITS
   static void ResetCurrentScenarioMainLimits();
   static void ResetCurrentScenarioEdgeLimits();
   void        SetCurrentScenarioLimitsIfNeeded();

   bool           GetFullUpdateRequired() const { return mFullUpdateRequired; }
   void           SetFullUpdateRequired(bool aState) { mFullUpdateRequired = aState; }
   wkf::Platform* GetPlatform() { return mPlatformPtr; }

private:
   wkf::Platform* mPlatformPtr;

   //! id of wing ribbon attachment for fetching the attachment from the platform pointer
   unsigned int mWingRibbonUniqueId;
   double       mLastUpdateTime;
   bool         mFullUpdateRequired;
   UtColor      mWingRibbonMainMaxColor;
   UtColor      mWingRibbonMainMinColor;
   UtColor      mWingRibbonEdgeMaxColor;
   UtColor      mWingRibbonEdgeMinColor;
   UtColor      mWingRibbonMainMaxFlippedColor;
   UtColor      mWingRibbonMainMinFlippedColor;
   UtColor      mWingRibbonEdgeMaxFlippedColor;
   UtColor      mWingRibbonEdgeMinFlippedColor;
   float        mWingRibbonWidthScale;
   float        mWingRibbonHardEdgeWidth;
   float        mWingRibbonSoftEdgeWidth;

   wkf::TrackableStateValue mEdgeStateBeingTracked;
   wkf::TrackableStateValue mMainStateBeingTracked;

   wkf::InterpolationLimitsMethod mEdgeLimitType;
   wkf::InterpolationLimitsMethod mMainLimitType;
   float                          mHardMaxMainLimit;
   float                          mHardMinMainLimit;
   float                          mHardMaxEdgeLimit;
   float                          mHardMinEdgeLimit;

   float mWidthScale;
   float mHardEdgeWidth;
   float mSoftEdgeWidth;

   static float mCurrentScenarioMainMax;
   static float mCurrentScenarioMainMin;
   static float mCurrentScenarioEdgeMax;
   static float mCurrentScenarioEdgeMin;

   osg::Vec3 mWingTipA;
   osg::Vec3 mWingTipB;

   float RetrieveStateValFrom(const rv::MsgEntityState& aPlatformPtr, wkf::TrackableStateValue aStateTracked);
};
} // namespace RvPlatformHistory
#endif // RvPlatformHistoryWingRibbonData_HPP
