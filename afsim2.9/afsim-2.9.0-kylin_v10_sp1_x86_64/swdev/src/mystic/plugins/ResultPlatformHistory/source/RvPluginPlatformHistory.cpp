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
#include "RvPluginPlatformHistory.hpp"

#include <QAction>
#include <QMenu>

#include "RvPlatformHistoryBehaviorStateData.hpp"
#include "RvPlatformHistoryMonoStateData.hpp"
#include "RvPlatformHistoryPlatformStateData.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "VaAttachmentWingRibbon.hpp"
#include "VaCamera.hpp"
#include "VaCameraMotionTethered.hpp"
#include "WkfObserver.hpp"
#include "WkfUtils.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(
   RvPlatformHistory::Plugin,
   "Platform History",
   "Plugin that supports drawing platform history such as tracelines, wing ribbons, and altitude poles",
   "mystic")

RvPlatformHistory::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mPrefWidgetPtr(new wkf::PlatformHistoryPrefWidget())
   , mBehaviorWidgetPtr(new PlatformHistoryBehaviorColorWidget())
   , mTraceLineLength(30.0)
   , mTraceLineRangeLoaded(0.0, 0.0)
   , mWingRibbonLengthSec(30.0)
   , mWingRibbonRangeLoaded(0.0, 0.0)
{
   mCallbacks.Add(wkf::Observer::EntityRemoved.Connect(&RvPlatformHistory::Plugin::EntityRemovedCB, this));
   mCallbacks.Add(wkf::Observer::ViewerDestroyed.Connect(&RvPlatformHistory::Plugin::ViewerDestroyedCB, this));

   mPrefObjectPtr = mPrefWidgetPtr->GetPreferenceObject();

   connect(mPrefObjectPtr,
           &wkf::PlatformHistoryPrefObject::TraceLineColorationChanged,
           this,
           &RvPlatformHistory::Plugin::TraceLineColorationHandler);
   connect(mPrefObjectPtr,
           &wkf::PlatformHistoryPrefObject::TraceLineBehaviorColorChanged,
           this,
           &RvPlatformHistory::Plugin::TraceLineBehaviorColorChangeHandler);
   connect(mPrefObjectPtr,
           &wkf::PlatformHistoryPrefObject::TraceLineLengthChanged,
           this,
           &RvPlatformHistory::Plugin::TraceLineLengthHandler);
   connect(mPrefObjectPtr,
           &wkf::PlatformHistoryPrefObject::TraceLineWidthChanged,
           this,
           &RvPlatformHistory::Plugin::TraceLineWidthHandler);
   connect(mPrefObjectPtr,
           &wkf::PlatformHistoryPrefObject::WingRibbonsLengthChanged,
           this,
           &RvPlatformHistory::Plugin::WingRibbonLengthHandler);
   connect(mPrefObjectPtr,
           &wkf::PlatformHistoryPrefObject::WingRibbonWidthScaleFieldChanged,
           this,
           &RvPlatformHistory::Plugin::WingRibbonWidthScaleHandler);
   connect(mPrefObjectPtr,
           &wkf::PlatformHistoryPrefObject::DeathTransparencySliderChanged,
           this,
           &RvPlatformHistory::Plugin::WingRibbonDeathTransparencyPercentHandler);
   connect(mPrefObjectPtr,
           &wkf::PlatformHistoryPrefObject::ColorSchemeChanged,
           this,
           &RvPlatformHistory::Plugin::WingRibbonColorSchemeHandler);

   mPrefWidgetPtr->AddTraceLineMode("behavior", wkf::TraceStateValue::eTRACE_BEHAVIOR, mBehaviorWidgetPtr.data());

   QTreeWidgetItem* parent = RegisterOptionGroup(nullptr, "History");
   mTraceLineType          = RegisterOption(parent, "Trace lines")->type();
   mWingRibbonType         = RegisterOption(parent, "Wing ribbons")->type();
}

QList<wkf::PrefWidget*> RvPlatformHistory::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> list;
   list.push_back(mPrefWidgetPtr);
   return list;
}

/*************************************************************************************************
 * @brief   Handler, called when the trace line coloration is changed by the gui.
 * @param   aColoration  The mode of coloration, either state or team color.
 * @date 8/2/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::TraceLineColorationHandler(wkf::TraceStateValue       aColoration,
                                                           const std::vector<QColor>& aColorList)
{
   std::vector<UtColor> ucl;
   for (auto qc : aColorList)
   {
      ucl.emplace_back(UtColor(qc.redF(), qc.greenF(), qc.blueF(), qc.alphaF()));
   }
   for (auto& iter : mTraceLineMap)
   {
      iter.second->SetColoration(aColoration, ucl);
   }
   for (auto& iter : mImmersiveTracelines)
   {
      iter.second->SetColoration(aColoration, ucl);
   }
}

void RvPlatformHistory::Plugin::TraceLineBehaviorColorChangeHandler(wkf::TraceStateValue                 aColoration,
                                                                    const std::map<std::string, QColor>& aBehaviorTraceLineMap)
{
   std::vector<UtColor> ucl{UtQtColor(mPrefObjectPtr->GetBehaviorDefaultColor())};
   mStateNames.clear();
   for (const auto& qc : aBehaviorTraceLineMap)
   {
      mStateNames.emplace(qc.first, ucl.size());
      ucl.emplace_back(UtColor(qc.second.redF(), qc.second.greenF(), qc.second.blueF(), qc.second.alphaF()));
   }
   for (auto& iter : mTraceLineMap)
   {
      iter.second->SetColoration(aColoration, ucl);
   }
   for (auto& iter : mImmersiveTracelines)
   {
      iter.second->SetColoration(aColoration, ucl);
   }
}

/*************************************************************************************************
 * @brief   Handler, called when the trace line length is changed by the gui.
 * @param   aLength  The length, in seconds, that the tracelines will trail behind for.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::TraceLineLengthHandler(double aLength)
{
   mTraceLineLength = aLength;
}

void RvPlatformHistory::Plugin::TraceLineWidthHandler(unsigned int aWidth)
{
   for (auto& iter : mTraceLineMap)
   {
      iter.second->SetLineWidth(aWidth);
   }
   for (auto& iter : mImmersiveTracelines)
   {
      iter.second->SetLineWidth(aWidth);
   }
}

/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon length is changed by the gui. Changes the length
 *          that wingribbons will trail behind their entity for.
 * @param   aLength  The length, in seconds, that the wingribbon will trail behind for.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonLengthHandler(double aLength)
{
   mWingRibbonLengthSec = aLength;
}

void RvPlatformHistory::Plugin::WingRibbonMainMaxColorHandler(QColor aMainMaxColor)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetMainMaxColor(UtQtColor(aMainMaxColor));
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetMainMaxColor(UtQtColor(aMainMaxColor));
   }
}
void RvPlatformHistory::Plugin::WingRibbonMainMinColorHandler(QColor aMainMinColor)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetMainMinColor(UtQtColor(aMainMinColor));
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetMainMinColor(UtQtColor(aMainMinColor));
   }
}
void RvPlatformHistory::Plugin::WingRibbonEdgeMaxColorHandler(QColor aEdgeMaxColor)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetEdgeMaxColor(UtQtColor(aEdgeMaxColor));
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetEdgeMaxColor(UtQtColor(aEdgeMaxColor));
   }
}
void RvPlatformHistory::Plugin::WingRibbonEdgeMinColorHandler(QColor aEdgeMinColor)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetEdgeMinColor(UtQtColor(aEdgeMinColor));
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetEdgeMinColor(UtQtColor(aEdgeMinColor));
   }
}
void RvPlatformHistory::Plugin::WingRibbonMainMaxFlippedColorHandler(QColor aMainMaxFlippedColor)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetMainMaxFlippedColor(UtQtColor(aMainMaxFlippedColor));
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetMainMaxFlippedColor(UtQtColor(aMainMaxFlippedColor));
   }
}
void RvPlatformHistory::Plugin::WingRibbonMainMinFlippedColorHandler(QColor aMainMinFlippedColor)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetMainMinFlippedColor(UtQtColor(aMainMinFlippedColor));
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetMainMinFlippedColor(UtQtColor(aMainMinFlippedColor));
   }
}
void RvPlatformHistory::Plugin::WingRibbonEdgeMaxFlippedColorHandler(QColor aEdgeMaxFlippedColor)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetEdgeMaxFlippedColor(UtQtColor(aEdgeMaxFlippedColor));
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetEdgeMaxFlippedColor(UtQtColor(aEdgeMaxFlippedColor));
   }
}
void RvPlatformHistory::Plugin::WingRibbonEdgeMinFlippedColorHandler(QColor aEdgeMinFlippedColor)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetEdgeMinFlippedColor(UtQtColor(aEdgeMinFlippedColor));
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetEdgeMinFlippedColor(UtQtColor(aEdgeMinFlippedColor));
   }
}

/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon main state being tracked is changed in gui.
 * @param   stateToTrack   The state to track, that affects what values will be recorded for the
 *                         main of the wing ribbons.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonMainStateBeingTrackedHandler(wkf::TrackableStateValue stateToTrack)
{
   WingRibbonData::ResetCurrentScenarioMainLimits();
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetMainStateBeingTracked(stateToTrack);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetMainStateBeingTracked(stateToTrack);
   }
}

/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon edge state being tracked is changed in gui.
 * @param   stateToTrack   The state to track, that affects what values will be recorded for the
 *                         edge of the wing ribbons.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonEdgeStateBeingTrackedHandler(wkf::TrackableStateValue stateToTrack)
{
   WingRibbonData::ResetCurrentScenarioEdgeLimits();
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetEdgeStateBeingTracked(stateToTrack);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetEdgeStateBeingTracked(stateToTrack);
   }
}

/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon edge limit type is changed by the gui. Changes
 *          how the colors of the wing ribbon's edge area is calculated in response to the recorded state
 *          values.
 * @param   aEdgeLimitType Type of the edge limit that determines how the colors of the wing
 *                         ribbon's edge is calculated in response to the recorded state values.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonEdgeLimitTypeHandler(wkf::InterpolationLimitsMethod aEdgeLimitType)
{
   // if (aEdgeLimitType == eCURRENT_SCENARIO_LIMITS){
   WingRibbonData::ResetCurrentScenarioEdgeLimits();
   //}
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetEdgeLimitType(aEdgeLimitType);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetEdgeLimitType(aEdgeLimitType);
   }
}

/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon main limit type is changed by the gui. Changes
 *          how the colors of the wing ribbon's main area is calculated in response to the recorded state
 *          values.
 * @param   aMainLimitType Type of the main limit that determines how the colors of the wing
 *                         ribbon's main is calculated in response to the recorded state values.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonMainLimitTypeHandler(wkf::InterpolationLimitsMethod aMainLimitType)
{
   WingRibbonData::ResetCurrentScenarioMainLimits();
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetMainLimitType(aMainLimitType);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetMainLimitType(aMainLimitType);
   }
}

/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon hard maximum main limit is change by the gui.
 *          Used in determining what color to display in the wing ribbon's main/center area when the
 *          main limits type is set  to eHARD_LIMITS.
 * @note Only used when the main InterpolationLimitsMethod is set to eHARD_LIMITS.
 * @param   aHardMaxMainLimit The hard maximum main limit.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonHardMaxMainLimitHandler(float aHardMaxMainLimit)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetHardMaxMainLimit(aHardMaxMainLimit);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetHardMinMainLimit(aHardMaxMainLimit);
   }
}
/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon hard minimum main limit is changed by the gui.
 *          Used in determining what color to display in the wing ribbon's main/center area when the
 *          main limits type is set  to eHARD_LIMITS.
 * @note Only used when the main InterpolationLimitsMethod is set to eHARD_LIMITS.
 * @param   aHardMinMainLimit The hard minimum main limit.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonHardMinMainLimitHandler(float aHardMinMainLimit)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetHardMinMainLimit(aHardMinMainLimit);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetHardMinMainLimit(aHardMinMainLimit);
   }
}
/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon hard maximum edge limit is changed by the gui.
 *          Used in determining what color to display in the wing ribbon's edge area when the main
 *          limits type is set  to eHARD_LIMITS.
 * @note Only used when the edge InterpolationLimitsMethod is set to eHARD_LIMITS.
 * @param   aHardMaxEdgeLimit The hard maximum edge limit.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonHardMaxEdgeLimitHandler(float aHardMaxEdgeLimit)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetHardMaxEdgeLimit(aHardMaxEdgeLimit);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetHardMaxEdgeLimit(aHardMaxEdgeLimit);
   }
}
/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon hard minimum edge limit is changed by the gui.
 *          Used in determining what color to display in the wing ribbon's edge area when the main
 *          limits type is set  to eHARD_LIMITS.
 * @note Only used when the edge InterpolationLimitsMethod is set to eHARD_LIMITS.
 * @param   aHardMinEdgeLimit The hard minimum edge limit.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonHardMinEdgeLimitHandler(float aHardMinEdgeLimit)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetHardMinEdgeLimit(aHardMinEdgeLimit);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetHardMinEdgeLimit(aHardMinEdgeLimit);
   }
}

/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon death transparency percent is changed by the
 *          gui. Used in determining how transparent a wing ribbon is at states where the platform
 *          is dead.
 * @param   aDeathTransparencyPercent  The death transparency as a percent between 0-100 for how
 *                                     transparent the wing ribbon becomes when a platform is
 *                                     dead.
 * @date 7/24/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonDeathTransparencyPercentHandler(int aDeathTransparencyPercent)
{
   // convert to float from 0-1 and invert as alpha at 1 is fully opaque, not fully transparent.
   vespa::VaAttachmentWingRibbon::SetTransparencyOnDeath(1.0f - aDeathTransparencyPercent / 100.0f);
}

void RvPlatformHistory::Plugin::WingRibbonColorSchemeHandler()
{
   WingRibbonMainMaxColorHandler(mPrefObjectPtr->GetWingRibbonMainMaxColor());
   WingRibbonMainMinColorHandler(mPrefObjectPtr->GetWingRibbonMainMinColor());
   WingRibbonEdgeMaxColorHandler(mPrefObjectPtr->GetWingRibbonEdgeMaxColor());
   WingRibbonEdgeMinColorHandler(mPrefObjectPtr->GetWingRibbonEdgeMinColor());
   WingRibbonMainMaxFlippedColorHandler(mPrefObjectPtr->GetWingRibbonMainMaxFlippedColor());
   WingRibbonMainMinFlippedColorHandler(mPrefObjectPtr->GetWingRibbonMainMinFlippedColor());
   WingRibbonEdgeMaxFlippedColorHandler(mPrefObjectPtr->GetWingRibbonEdgeMaxFlippedColor());
   WingRibbonEdgeMinFlippedColorHandler(mPrefObjectPtr->GetWingRibbonEdgeMinFlippedColor());
   WingRibbonHardMaxEdgeLimitHandler(mPrefObjectPtr->GetWingRibbonHardMaxEdgeLimit());
   WingRibbonHardMinEdgeLimitHandler(mPrefObjectPtr->GetWingRibbonHardMinEdgeLimit());
   WingRibbonHardMaxMainLimitHandler(mPrefObjectPtr->GetWingRibbonHardMaxMainLimit());
   WingRibbonHardMinMainLimitHandler(mPrefObjectPtr->GetWingRibbonHardMinMainLimit());

   WingRibbonHardEdgeWidthHandler(mPrefObjectPtr->GetWingRibbonHardEdgeWidth());
   WingRibbonSoftEdgeWidthHandler(mPrefObjectPtr->GetWingRibbonSoftEdgeWidth());

   WingRibbonEdgeLimitTypeHandler(mPrefObjectPtr->GetWingRibbonEdgeLimitType());
   WingRibbonMainLimitTypeHandler(mPrefObjectPtr->GetWingRibbonMainLimitType());
   WingRibbonEdgeStateBeingTrackedHandler(mPrefObjectPtr->GetWingRibbonEdgeStateBeingTracked());
   WingRibbonMainStateBeingTrackedHandler(mPrefObjectPtr->GetWingRibbonMainStateBeingTracked());
}

/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon width scale is changed by the gui. Modifies the
 *          width of the wingribbons, with the reference scale of 1.0f being aligned with the
 *          wingtips.
 * @param   aWidthScale The width scale.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonWidthScaleHandler(float aWidthScale)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetWidthScale(aWidthScale);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetWidthScale(aWidthScale);
   }
}

/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon hard edge width is changed by the gui. Changes
 *          the percent of the wing ribbon's area that is taken up by the edge color.
 * @param   aHardEdgeWidth Width of the hard edge, as a percent in the domain [0.0, 100.0].
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonHardEdgeWidthHandler(float aHardEdgeWidth)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetHardEdgeWidth(aHardEdgeWidth / 100);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetHardEdgeWidth(aHardEdgeWidth / 100);
   }
}
/*************************************************************************************************
 * @brief   Handler, called when the wing ribbon soft edge width. Changes the percent of the wing
 *          ribbon's area that is used to interpolate between the main and edge colors (as a form of anti-
 *          aliasing).
 * @param   aSoftEdgeWidth Width of the soft edge, as a percent in the domain [0.0, 100.0].
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::WingRibbonSoftEdgeWidthHandler(float aSoftEdgeWidth)
{
   for (auto& iter : mWingRibbonMap)
   {
      iter.second->SetSoftEdgeWidth(aSoftEdgeWidth / 100);
   }
   for (auto& iter : mImmersiveWingRibbons)
   {
      iter.second->SetSoftEdgeWidth(aSoftEdgeWidth / 100);
   }
}

void RvPlatformHistory::Plugin::BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr)
{
   UtVariant v = aViewerPtr->GetData("type");
   if (!v.IsUnset())
   {
      std::string type = v.GetString();
      if (type == "immersive")
      {
         vespa::VaCameraMotionTethered* tether =
            dynamic_cast<vespa::VaCameraMotionTethered*>(aViewerPtr->GetCamera()->GetCameraMotion());
         if (tether)
         {
            wkf::Platform* plat = dynamic_cast<wkf::Platform*>(tether->GetEntity());

            if (plat)
            {
               if (mImmersiveTracelines.find(aViewerPtr->GetUniqueId()) != mImmersiveTracelines.end())
               {
                  QAction* remTraceline = new QAction("Remove Traceline", aMenu);
                  aMenu->addAction(remTraceline);
                  connect(remTraceline,
                          &QAction::triggered,
                          [aViewerPtr, this]() { mImmersiveTracelines.erase(aViewerPtr->GetUniqueId()); });
               }
               else
               {
                  QAction* addTraceline = new QAction("Add Traceline", aMenu);
                  aMenu->addAction(addTraceline);
                  connect(addTraceline,
                          &QAction::triggered,
                          [aViewerPtr, plat, this]() { AddImmersiveTraceline(*plat, aViewerPtr); });
               }
               if (mImmersiveWingRibbons.find(aViewerPtr->GetUniqueId()) != mImmersiveWingRibbons.end())
               {
                  QAction* remWingRibbon = new QAction("Remove Wing Ribbon", aMenu);
                  aMenu->addAction(remWingRibbon);
                  connect(remWingRibbon,
                          &QAction::triggered,
                          [aViewerPtr, this]() { mImmersiveWingRibbons.erase(aViewerPtr->GetUniqueId()); });
               }
               else
               {
                  QAction* addWingRibbon = new QAction("Add Wing Ribbon", aMenu);
                  aMenu->addAction(addWingRibbon);
                  connect(addWingRibbon,
                          &QAction::triggered,
                          [aViewerPtr, plat, this]() { AddImmersiveWingRibbon(*plat, aViewerPtr); });
               }
            }
         }
      }
   }
}

/*************************************************************************************************
 * @brief   Entity removed callback. Cleans up private memory of the entity's wingribbon/traceline attachments
 * @param   aEntityPtr  Pointer to the entity being removed.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::EntityRemovedCB(vespa::VaEntity* aEntityPtr)
{
   wkf::Platform* platform = dynamic_cast<wkf::Platform*>(aEntityPtr);
   if (platform != nullptr)
   {
      auto index = platform->GetIndex();
      mTraceLineMap.erase(index);
      mWingRibbonMap.erase(index);
      std::set<unsigned int> remset;
      for (auto& it : mImmersiveTracelines)
      {
         if (it.second->GetPlatform()->GetIndex() == index)
         {
            remset.insert(it.first);
         }
      }
      for (auto& rem : remset)
      {
         mImmersiveTracelines.erase(rem);
      }
      remset.clear();
      for (auto& it : mImmersiveWingRibbons)
      {
         if (it.second->GetPlatform()->GetIndex() == index)
         {
            remset.insert(it.first);
         }
      }
      for (auto& rem : remset)
      {
         mImmersiveWingRibbons.erase(rem);
      }
   }
}

void RvPlatformHistory::Plugin::ViewerDestroyedCB(vespa::VaViewer* aViewerPtr)
{
   const auto& tl = mImmersiveTracelines.find(aViewerPtr->GetUniqueId());
   if (tl != mImmersiveTracelines.end())
   {
      mImmersiveTracelines.erase(tl);
   }
   const auto& wr = mImmersiveWingRibbons.find(aViewerPtr->GetUniqueId());
   if (wr != mImmersiveWingRibbons.end())
   {
      mImmersiveWingRibbons.erase(wr);
   }
}

RvPlatformHistory::Plugin::TimeInterval RvPlatformHistory::Plugin::Intersect(const TimeInterval& aFirst,
                                                                             const TimeInterval& aSecond) const
{
   TimeInterval retval(0.0f, 0.0f);
   retval.first  = (aFirst.first > aSecond.first) ? aFirst.first : aSecond.first;
   retval.second = (aFirst.second > aSecond.second) ? aSecond.second : aFirst.second;
   if (retval.first > retval.second)
   {
      retval.first = retval.second = 0.0f;
   }
   return retval;
}

RvPlatformHistory::Plugin::TimeInterval RvPlatformHistory::Plugin::LowDifference(const TimeInterval& aFirst,
                                                                                 const TimeInterval& aSecond) const
{
   if (aFirst.first < aSecond.first)
   {
      return TimeInterval(aFirst.first, (aFirst.second > aSecond.first) ? aSecond.first : aFirst.second);
   }
   return TimeInterval(-1.0f, -1.0f);
}

RvPlatformHistory::Plugin::TimeInterval RvPlatformHistory::Plugin::HighDifference(const TimeInterval& aFirst,
                                                                                  const TimeInterval& aSecond) const
{
   if (aFirst.second > aSecond.second)
   {
      return TimeInterval((aFirst.first < aSecond.second) ? aSecond.second : aFirst.first, aFirst.second);
   }
   return TimeInterval(-1.0f, -1.0f);
}

void RvPlatformHistory::Plugin::AddImmersiveTraceline(wkf::Platform& aPlatform, vespa::VaViewer* aViewerPtr)
{
   mImmersiveTracelines[aViewerPtr->GetUniqueId()] = ut::make_unique<TracelineData>(aPlatform, aViewerPtr);
   auto& ptr                                       = mImmersiveTracelines[aViewerPtr->GetUniqueId()];

   ptr->SetVisible(true);

   ptr->SetFullUpdateRequired(true);
   ptr->SetLineWidth(mPrefObjectPtr->GetTraceLineWidth());
   std::vector<UtColor> ucl;
   if (mPrefObjectPtr->GetTraceLineColoration() == wkf::eTRACE_BEHAVIOR)
   {
      std::map<std::string, QColor> colorMapBehavior = mPrefObjectPtr->GetBehaviorColorMap();
      for (const auto& color : colorMapBehavior)
      {
         ucl.emplace_back(UtColor(color.second.redF(), color.second.greenF(), color.second.blueF(), color.second.alphaF()));
      }
   }
   else
   {
      const std::vector<QColor>& cl = mPrefObjectPtr->GetTraceLineColorationList();
      for (auto qc : cl)
      {
         ucl.emplace_back(UtColor(qc.redF(), qc.greenF(), qc.blueF(), qc.alphaF()));
      }
   }
   ptr->SetColoration(mPrefObjectPtr->GetTraceLineColoration(), ucl);

   auto scenarioData = rvEnv.GetData();
   if (scenarioData != nullptr)
   {
      int  idx          = aPlatform.GetIndex();
      auto platformData = scenarioData->FindPlatform(idx);
      if (platformData != nullptr)
      {
         ptr->AddStateSet(wkf::eTRACE_STATE, ut::make_unique<PlatformStateData>());
         ptr->AddStateSet(wkf::eTRACE_TEAM_COLOR, ut::make_unique<MonoStateData>());
         ptr->AddStateSet(wkf::eTRACE_NAME_HASH, ut::make_unique<MonoStateData>());
         ptr->AddStateSet(wkf::eTRACE_BEHAVIOR, ut::make_unique<BehaviorStateData>(platformData, mStateNames));
      }
   }
}

void RvPlatformHistory::Plugin::AddImmersiveWingRibbon(wkf::Platform& aPlatform, vespa::VaViewer* aViewerPtr)
{
   mImmersiveWingRibbons[aViewerPtr->GetUniqueId()] = ut::make_unique<WingRibbonData>(aPlatform, aViewerPtr);
   auto& ptr                                        = mImmersiveWingRibbons[aViewerPtr->GetUniqueId()];

   ptr->SetVisible(true);
   ptr->SetMainMaxColor(UtQtColor(mPrefObjectPtr->GetWingRibbonMainMaxColor()));
   ptr->SetMainMinColor(UtQtColor(mPrefObjectPtr->GetWingRibbonMainMinColor()));
   ptr->SetEdgeMaxColor(UtQtColor(mPrefObjectPtr->GetWingRibbonEdgeMaxColor()));
   ptr->SetEdgeMinColor(UtQtColor(mPrefObjectPtr->GetWingRibbonEdgeMinColor()));
   ptr->SetMainMaxFlippedColor(UtQtColor(mPrefObjectPtr->GetWingRibbonMainMaxFlippedColor()));
   ptr->SetMainMinFlippedColor(UtQtColor(mPrefObjectPtr->GetWingRibbonMainMinFlippedColor()));
   ptr->SetEdgeMaxFlippedColor(UtQtColor(mPrefObjectPtr->GetWingRibbonEdgeMaxFlippedColor()));
   ptr->SetEdgeMinFlippedColor(UtQtColor(mPrefObjectPtr->GetWingRibbonEdgeMinFlippedColor()));

   ptr->SetHardMaxEdgeLimit(mPrefObjectPtr->GetWingRibbonHardMaxEdgeLimit());
   ptr->SetHardMinEdgeLimit(mPrefObjectPtr->GetWingRibbonHardMinEdgeLimit());
   ptr->SetHardMaxMainLimit(mPrefObjectPtr->GetWingRibbonHardMaxMainLimit());
   ptr->SetHardMinMainLimit(mPrefObjectPtr->GetWingRibbonHardMinMainLimit());

   ptr->SetWidthScale(mPrefObjectPtr->GetWingRibbonWidthScale());
   ptr->SetHardEdgeWidth(mPrefObjectPtr->GetWingRibbonHardEdgeWidth() / 100);
   ptr->SetSoftEdgeWidth(mPrefObjectPtr->GetWingRibbonSoftEdgeWidth() / 100);

   ptr->SetEdgeLimitType(mPrefObjectPtr->GetWingRibbonEdgeLimitType());
   ptr->SetMainLimitType(mPrefObjectPtr->GetWingRibbonMainLimitType());
   ptr->SetEdgeStateBeingTracked(mPrefObjectPtr->GetWingRibbonEdgeStateBeingTracked());
   ptr->SetMainStateBeingTracked(mPrefObjectPtr->GetWingRibbonMainStateBeingTracked());
   ptr->SetFullUpdateRequired(true);
}

void RvPlatformHistory::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

void RvPlatformHistory::Plugin::ClearScenario(bool aFullReset)
{
   mTraceLineMap.clear();
   mWingRibbonRangeLoaded.first  = 0.0;
   mWingRibbonRangeLoaded.second = 0.0;
   mBehaviorsKnown               = false;
}

void RvPlatformHistory::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   {
      TimeInterval rangeOfInterest(static_cast<float>(aData.GetSimTime()) - static_cast<float>(mTraceLineLength),
                                   static_cast<float>(aData.GetSimTime())); // this is the data we want, ideally
      TimeInterval rangeAvailable = aData.GetCacheRange();                  // this is the data that is available
      rangeOfInterest             = Intersect(rangeOfInterest,
                                  rangeAvailable); // this is what we can have based on our desires and data availability

      TimeInterval loAddRange = LowDifference(rangeOfInterest,
                                              mTraceLineRangeLoaded); // this is what we will need to add to make what
                                                                      // we have into what we can get
      TimeInterval hiAddRange = HighDifference(rangeOfInterest,
                                               mTraceLineRangeLoaded); // this is what we will need to add to make what
                                                                       // we have into what we can get

      FindBehaviorStates(aData);

      for (auto& iter : mTraceLineMap)
      {
         if (iter.second->GetFullUpdateRequired())
         {
            iter.second->Update(iter.second->GetColoration(),
                                aData.FindPlatform(iter.first),
                                rangeOfInterest.first,
                                rangeOfInterest.second,
                                TimeInterval(-1.0f, -1.0f),
                                rangeOfInterest);
            iter.second->SetFullUpdateRequired(false);
         }
         else
         {
            iter.second->Update(iter.second->GetColoration(),
                                aData.FindPlatform(iter.first),
                                rangeOfInterest.first,
                                rangeOfInterest.second,
                                loAddRange,
                                hiAddRange);
         }
      }
      for (auto& iter : mImmersiveTracelines)
      {
         if (iter.second->GetFullUpdateRequired())
         {
            iter.second->Update(iter.second->GetColoration(),
                                aData.FindPlatform(iter.second->GetPlatform()->GetIndex()),
                                -1.0f,
                                -1.0f,
                                TimeInterval(-1.0f, -1.0f),
                                rangeOfInterest);
            iter.second->SetFullUpdateRequired(false);
         }
         else
         {
            iter.second->Update(iter.second->GetColoration(),
                                aData.FindPlatform(iter.second->GetPlatform()->GetIndex()),
                                rangeOfInterest.first,
                                rangeOfInterest.second,
                                loAddRange,
                                hiAddRange);
         }
      }
      mTraceLineRangeLoaded = rangeOfInterest;
   }
   {
      TimeInterval rangeOfInterest(static_cast<float>(aData.GetSimTime()) - static_cast<float>(mWingRibbonLengthSec),
                                   static_cast<float>(aData.GetSimTime())); // this is the data we want, ideally
      TimeInterval rangeAvailable = aData.GetCacheRange();                  // this is the data that is available
      rangeOfInterest             = Intersect(rangeOfInterest,
                                  rangeAvailable); // this is what we can have based on our desires and data availability
      TimeInterval loCullRange =
         LowDifference(mWingRibbonRangeLoaded,
                       rangeOfInterest); // this is what we will need to remove to make what we have into what we can get
      TimeInterval hiCullRange = HighDifference(mWingRibbonRangeLoaded, rangeOfInterest);

      TimeInterval loAddRange = LowDifference(rangeOfInterest,
                                              mWingRibbonRangeLoaded); // this is what we will need to add to make what
                                                                       // we have into what we can get
      TimeInterval hiAddRange = HighDifference(rangeOfInterest,
                                               mWingRibbonRangeLoaded); // this is what we will need to add to make what
                                                                        // we have into what we can get
      for (auto& iter : mWingRibbonMap)
      {
         iter.second->ResetCurrentScenarioLimitsIfNeeded();
      }
      for (auto& iter : mWingRibbonMap)
      {
         if (iter.second->GetFullUpdateRequired())
         {
            if (iter.second->IsVisible())
            {
               iter.second->Update(aData.FindPlatform(iter.first), -1.0f, -1.0f, TimeInterval(-1.0f, -1.0f), rangeOfInterest);
               iter.second->SetFullUpdateRequired(false);
            }
         }
         else
         {
            iter.second->Update(aData.FindPlatform(iter.first), loCullRange.second, hiCullRange.first, loAddRange, hiAddRange);
         }
      }
      for (auto& iter : mWingRibbonMap)
      {
         iter.second->SetCurrentScenarioLimitsIfNeeded();
      }

      for (auto& iter : mImmersiveWingRibbons)
      {
         iter.second->ResetCurrentScenarioLimitsIfNeeded();
      }
      for (auto& iter : mImmersiveWingRibbons)
      {
         if (iter.second->GetFullUpdateRequired())
         {
            if (iter.second->IsVisible())
            {
               iter.second->Update(aData.FindPlatform(iter.second->GetPlatform()->GetIndex()),
                                   -1.0f,
                                   -1.0f,
                                   TimeInterval(-1.0f, -1.0f),
                                   rangeOfInterest);
               iter.second->SetFullUpdateRequired(false);
            }
         }
         else
         {
            iter.second->Update(aData.FindPlatform(iter.second->GetPlatform()->GetIndex()),
                                loCullRange.second,
                                hiCullRange.first,
                                loAddRange,
                                hiAddRange);
         }
      }
      for (auto& iter : mImmersiveWingRibbons)
      {
         iter.second->SetCurrentScenarioLimitsIfNeeded();
      }
      mWingRibbonRangeLoaded = rangeOfInterest;
   }
}

void RvPlatformHistory::Plugin::ResetOptionStates()
{
   mTraceLineMap.clear();
   mWingRibbonMap.clear();
}

/*************************************************************************************************
 * @brief   Creates an attachment for the entity of type aId if one does not already exist for it, and sets visibility
 *of the attachments of type aId that are attached to that entity to aSetVisibe
 * @param   aId         The attachment type whose visibility is being changed.
 * @param   aSetVisible True to show, false to hide the attachments.
 * @param   aPlatformPtr  Pointer to the platform to create/find the attachments on.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::Plugin::SetPlatformOptionState(int aId, bool aSetVisible, wkf::Platform* aPlatformPtr)
{
   if (aPlatformPtr)
   {
      unsigned int index = aPlatformPtr->GetIndex();

      if (aId == mTraceLineType)
      {
         std::map<unsigned int, std::unique_ptr<TracelineData>>::iterator jt = mTraceLineMap.find(index);
         if (aSetVisible)
         {
            if (jt != mTraceLineMap.end())
            {
               jt->second->SetVisible(true);
               // show it
            }
            else
            {
               mTraceLineMap[index] =
                  ut::make_unique<TracelineData>(*aPlatformPtr, vaEnv.GetStandardViewer()); // create it
               mTraceLineMap[index]->SetFullUpdateRequired(true);
               TraceLineLengthHandler(mPrefObjectPtr->GetTraceLineLength());
               mTraceLineMap[index]->SetLineWidth(mPrefObjectPtr->GetTraceLineWidth());
               std::vector<UtColor> ucl;
               if (mPrefObjectPtr->GetTraceLineColoration() == wkf::eTRACE_BEHAVIOR)
               {
                  std::map<std::string, QColor> colorMapBehavior = mPrefObjectPtr->GetBehaviorColorMap();
                  for (const auto& color : colorMapBehavior)
                  {
                     ucl.emplace_back(
                        UtColor(color.second.redF(), color.second.greenF(), color.second.blueF(), color.second.alphaF()));
                  }
               }
               else
               {
                  const std::vector<QColor>& cl = mPrefObjectPtr->GetTraceLineColorationList();
                  for (auto qc : cl)
                  {
                     ucl.emplace_back(UtColor(qc.redF(), qc.greenF(), qc.blueF(), qc.alphaF()));
                  }
               }
               mTraceLineMap[index]->SetColoration(mPrefObjectPtr->GetTraceLineColoration(), ucl);
               InitializeStateData(index);
            }
         }
         else
         {
            if (jt != mTraceLineMap.end())
            {
               jt->second->SetVisible(false);
            }
         }
         // turn on trace lines for the selected entity
      }
      else if (aId == mWingRibbonType)
      {
         std::map<unsigned int, std::unique_ptr<WingRibbonData>>::iterator jt = mWingRibbonMap.find(index);
         if (aSetVisible)
         {
            if (jt != mWingRibbonMap.end())
            {
               jt->second->SetVisible(true);
               // show it
            }
            else
            {
               mWingRibbonMap[index] =
                  ut::make_unique<WingRibbonData>(*aPlatformPtr, vaEnv.GetStandardViewer()); // create it
               auto& ptr = mWingRibbonMap[index];

               ptr->SetVisible(true);
               // ptr->SetVisible(mPrefObjectPtr->GetShowWingRibbons());

               ptr->SetMainMaxColor(UtQtColor(mPrefObjectPtr->GetWingRibbonMainMaxColor()));
               ptr->SetMainMinColor(UtQtColor(mPrefObjectPtr->GetWingRibbonMainMinColor()));
               ptr->SetEdgeMaxColor(UtQtColor(mPrefObjectPtr->GetWingRibbonEdgeMaxColor()));
               ptr->SetEdgeMinColor(UtQtColor(mPrefObjectPtr->GetWingRibbonEdgeMinColor()));
               ptr->SetMainMaxFlippedColor(UtQtColor(mPrefObjectPtr->GetWingRibbonMainMaxFlippedColor()));
               ptr->SetMainMinFlippedColor(UtQtColor(mPrefObjectPtr->GetWingRibbonMainMinFlippedColor()));
               ptr->SetEdgeMaxFlippedColor(UtQtColor(mPrefObjectPtr->GetWingRibbonEdgeMaxFlippedColor()));
               ptr->SetEdgeMinFlippedColor(UtQtColor(mPrefObjectPtr->GetWingRibbonEdgeMinFlippedColor()));

               ptr->SetHardMaxEdgeLimit(mPrefObjectPtr->GetWingRibbonHardMaxEdgeLimit());
               ptr->SetHardMinEdgeLimit(mPrefObjectPtr->GetWingRibbonHardMinEdgeLimit());
               ptr->SetHardMaxMainLimit(mPrefObjectPtr->GetWingRibbonHardMaxMainLimit());
               ptr->SetHardMinMainLimit(mPrefObjectPtr->GetWingRibbonHardMinMainLimit());

               ptr->SetWidthScale(mPrefObjectPtr->GetWingRibbonWidthScale());
               ptr->SetHardEdgeWidth(mPrefObjectPtr->GetWingRibbonHardEdgeWidth() / 100);
               ptr->SetSoftEdgeWidth(mPrefObjectPtr->GetWingRibbonSoftEdgeWidth() / 100);

               ptr->SetEdgeLimitType(mPrefObjectPtr->GetWingRibbonEdgeLimitType());
               ptr->SetMainLimitType(mPrefObjectPtr->GetWingRibbonMainLimitType());
               ptr->SetEdgeStateBeingTracked(mPrefObjectPtr->GetWingRibbonEdgeStateBeingTracked());
               ptr->SetMainStateBeingTracked(mPrefObjectPtr->GetWingRibbonMainStateBeingTracked());
               ptr->SetFullUpdateRequired(true);
            }
         }
         else
         {
            if (jt != mWingRibbonMap.end())
            {
               jt->second->SetVisible(false);
            }
         }
      }
   }
}

/*************************************************************************************************
 * @brief   Determines whether or not the attachment type is set to be visible on the entity
 * @param   aId          The type of attachment to determine visibility status on.
 * @param   aPlatformPtr Pointer to the platform that has the attachments we are looking for.
 * @return  True if the attachment type is visible, false if it fails.
 * @date 7/14/2017
 **************************************************************************************************/
Qt::CheckState RvPlatformHistory::Plugin::GetPlatformOptionState(int aId, const wkf::Platform* aPlatformPtr) const
{
   if (aPlatformPtr)
   {
      if (aId == mTraceLineType)
      {
         if (mTraceLineMap.find(aPlatformPtr->GetIndex()) != mTraceLineMap.end())
         {
            return (mTraceLineMap.at(aPlatformPtr->GetIndex())->IsVisible()) ? Qt::Checked : Qt::Unchecked;
         }
      }
      else if (aId == mWingRibbonType)
      {
         if (mWingRibbonMap.find(aPlatformPtr->GetIndex()) != mWingRibbonMap.end())
         {
            return (mWingRibbonMap.at(aPlatformPtr->GetIndex())->IsVisible()) ? Qt::Checked : Qt::Unchecked;
         }
      }
   }
   return Qt::Unchecked;
}

void RvPlatformHistory::Plugin::InitializeStateData(unsigned int aIndex)
{
   auto scenarioData = rvEnv.GetData();
   if (scenarioData != nullptr)
   {
      auto platformData = scenarioData->FindPlatform(aIndex);
      if (platformData != nullptr)
      {
         mTraceLineMap[aIndex]->AddStateSet(wkf::eTRACE_STATE, ut::make_unique<PlatformStateData>());
         mTraceLineMap[aIndex]->AddStateSet(wkf::eTRACE_TEAM_COLOR, ut::make_unique<MonoStateData>());
         mTraceLineMap[aIndex]->AddStateSet(wkf::eTRACE_NAME_HASH, ut::make_unique<MonoStateData>());
         mTraceLineMap[aIndex]->AddStateSet(wkf::eTRACE_BEHAVIOR,
                                            ut::make_unique<BehaviorStateData>(platformData, mStateNames));
      }
   }
}

void RvPlatformHistory::Plugin::FindBehaviorStates(const rv::ResultData& aDataPtr)
{
   std::map<std::string, std::pair<bool, UtColor>> behaviors;

   if (!mBehaviorsKnown)
   {
      // Get all node names and their respective script-defined colors.
      auto db = aDataPtr.GetDb();
      if (db == nullptr || db->GetMaxTime() == 0.0)
      {
         return;
      }

      auto numPlatforms = db->GetPlatformCount();
      for (unsigned int i = 0; i < numPlatforms; ++i)
      {
         auto platform = db->FindPlatform(i);
         if (platform != nullptr)
         {
            platform->BeginRead();
            const auto dataArray = platform->GetArray<rv::MsgBehaviorTree>();

            if (dataArray != nullptr)
            {
               for (auto&& kt : *dataArray)
               {
                  auto tree = static_cast<rv::MsgBehaviorTree*>(kt);
                  for (auto& node : tree->nodes())
                  {
                     // By default, only get leaf nodes.
                     if (node.numChildren() == 0 && behaviors.find(node.nodeName()) == behaviors.end())
                     {
                        behaviors.emplace(node.nodeName(),
                                          std::pair<bool, UtColor>{node.platformHistoryColorValid(),
                                                                   UtColor{node.platformHistoryColor().red(),
                                                                           node.platformHistoryColor().green(),
                                                                           node.platformHistoryColor().blue(),
                                                                           node.platformHistoryColor().alpha()}});
                     }
                  }
               }
            }
            platform->EndRead();
         }
      }

      // Precidence:
      // 1. Saved preference.
      // 2. Script-defined.
      // 3. Name hash.

      if (mPrefWidgetPtr)
      {
         // Try saved preferences first
         auto                          oldColors = mPrefObjectPtr->GetBehaviorColorMap();
         std::map<std::string, QColor> colors;
         for (auto& color : oldColors)
         {
            if (behaviors.find(color.first) != behaviors.end())
            {
               colors.emplace(color.first, color.second);
            }
         }

         for (auto& name : behaviors)
         {
            // Next, look for script defined colors.
            if (name.second.first && colors.find(name.first) == colors.end())
            {
               QColor inputColor{UtQtColor(name.second.second)};
               colors.emplace(name.first, inputColor);
            }
            // If all else failed, assign a color from the node's name's hash.
            if (colors.find(name.first) == colors.end())
            {
               colors.emplace(name.first, wkf::util::GetHashColor(QString::fromStdString(name.first)));
            }
         }
         mBehaviorWidgetPtr->SetBehaviorColors(colors);
         mPrefWidgetPtr->Apply();
         TraceLineBehaviorColorChangeHandler(mPrefObjectPtr->GetTraceLineColoration(), colors);
         mBehaviorsKnown = true;
      }
   }
}
