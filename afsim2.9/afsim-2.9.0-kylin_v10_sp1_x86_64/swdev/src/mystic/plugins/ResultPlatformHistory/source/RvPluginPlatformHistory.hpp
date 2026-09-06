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
#ifndef RvPluginPlatformHistory_HPP
#define RvPluginPlatformHistory_HPP

#include "RvPlatformHistoryBehaviorColorWidget.hpp"
#include "RvPlatformHistoryTracelineData.hpp"
#include "RvPlatformHistoryWingRibbonData.hpp"
#include "RvPlugin.hpp"

namespace RvPlatformHistory
{
/**
 * Trace the path that a platform has taken with a traceline or wing ribbon.
 */
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   void           ResetOptionStates() override;
   void           SetPlatformOptionState(int aId, bool aSetVisible, wkf::Platform* aEntityPtr) override;
   Qt::CheckState GetPlatformOptionState(int aId, const wkf::Platform* aPlatformPtr) const override;

private slots:
   void AdvanceTimeRead(const rv::ResultData& aDataPtr) override;
   void RegularRead(const rv::ResultData& aData) override;
   void ClearScenario(bool aFullReset) override;

   void TraceLineColorationHandler(wkf::TraceStateValue aColoration, const std::vector<QColor>& aColorList);
   void TraceLineBehaviorColorChangeHandler(wkf::TraceStateValue                 aColoration,
                                            const std::map<std::string, QColor>& aBehaviorTraceLineMap);
   void TraceLineLengthHandler(double aLength);
   void TraceLineWidthHandler(unsigned int aWidth);
   void WingRibbonLengthHandler(double aLength);

   void WingRibbonMainMaxColorHandler(QColor aMainMaxColor);
   void WingRibbonMainMinColorHandler(QColor aMainMinColor);
   void WingRibbonEdgeMaxColorHandler(QColor aEdgeMaxColor);
   void WingRibbonEdgeMinColorHandler(QColor aEdgeMinColor);
   void WingRibbonMainMaxFlippedColorHandler(QColor aMainMaxFlippedColor);
   void WingRibbonMainMinFlippedColorHandler(QColor aMainMinFlippedColor);
   void WingRibbonEdgeMaxFlippedColorHandler(QColor aEdgeMaxFlippedColor);
   void WingRibbonEdgeMinFlippedColorHandler(QColor aEdgeMinFlippedColor);

   void WingRibbonMainStateBeingTrackedHandler(wkf::TrackableStateValue stateToTrack);
   void WingRibbonEdgeStateBeingTrackedHandler(wkf::TrackableStateValue stateToTrack);

   void WingRibbonEdgeLimitTypeHandler(wkf::InterpolationLimitsMethod aEdgeLimitType);
   void WingRibbonMainLimitTypeHandler(wkf::InterpolationLimitsMethod aMainLimitType);

   void WingRibbonHardMaxMainLimitHandler(float aHardMaxMainLimit);
   void WingRibbonHardMinMainLimitHandler(float aHardMinMainLimit);
   void WingRibbonHardMaxEdgeLimitHandler(float aHardMaxEdgeLimit);
   void WingRibbonHardMinEdgeLimitHandler(float aHardMinEdgeLimit);

   void WingRibbonDeathTransparencyPercentHandler(int aDeathTransparencyPercent);
   void WingRibbonColorSchemeHandler();

   void WingRibbonWidthScaleHandler(float aWidthScale);
   void WingRibbonHardEdgeWidthHandler(float aHardEdgeWidth);
   void WingRibbonSoftEdgeWidthHandler(float aSoftEdgeWidth);

private:
   using TimeInterval = std::pair<float, float>;

   void BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr) override;
   void EntityRemovedCB(vespa::VaEntity* aEntityPtr);
   void ViewerDestroyedCB(vespa::VaViewer* aViewerPtr);

   TimeInterval Intersect(const TimeInterval& aFirst, const TimeInterval& aSecond) const;
   TimeInterval LowDifference(const TimeInterval& aFirst, const TimeInterval& aSecond) const;
   TimeInterval HighDifference(const TimeInterval& aFirst, const TimeInterval& aSecond) const;

   void InitializeStateData(unsigned int aIndex);

   void FindBehaviorStates(const rv::ResultData& aDataPtr);

   std::map<std::string, int> mStateNames;

   std::map<unsigned int, std::unique_ptr<TracelineData>> mTraceLineMap;
   std::map<unsigned int, std::unique_ptr<TracelineData>> mImmersiveTracelines;

   void AddImmersiveTraceline(wkf::Platform& aPlatform, vespa::VaViewer* aViewerPtr);

   void AddImmersiveWingRibbon(wkf::Platform& aPlatform, vespa::VaViewer* aViewerPtr);

   std::map<unsigned int, std::unique_ptr<WingRibbonData>> mWingRibbonMap;
   std::map<unsigned int, std::unique_ptr<WingRibbonData>> mImmersiveWingRibbons;

   vespa::VaCallbackHolder mCallbacks;

   const wkf::PlatformHistoryPrefObject* mPrefObjectPtr;
   // Use guarded pointers because objects will be owned by main window
   PluginUiPointer<wkf::PlatformHistoryPrefWidget>     mPrefWidgetPtr;
   PluginUiPointer<PlatformHistoryBehaviorColorWidget> mBehaviorWidgetPtr;

   // In seconds
   double       mTraceLineLength;
   TimeInterval mTraceLineRangeLoaded;

   // In seconds
   double       mWingRibbonLengthSec;
   TimeInterval mWingRibbonRangeLoaded;

   int mTraceLineType;
   int mWingRibbonType;

   bool mBehaviorsKnown{false};
};
} // namespace RvPlatformHistory
#endif
