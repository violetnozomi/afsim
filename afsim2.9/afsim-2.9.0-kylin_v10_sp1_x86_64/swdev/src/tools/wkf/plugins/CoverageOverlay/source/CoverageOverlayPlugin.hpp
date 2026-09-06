// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef COVERAGEOVERLAYPLUGIN_HPP
#define COVERAGEOVERLAYPLUGIN_HPP

#include <map>

#include "CoverageDataManager.hpp"
#include "CoverageOverlayDockWidget.hpp"
#include "HeatmapOverlay.hpp"
#include "UtQtGradientMap.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfPlugin.hpp"

namespace CoverageOverlay
{

//! This plugin pesents a dockable tool that allows the user to load and display coverage overlay files.
class Plugin : public wkf::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

private:
   void InitializeGradients();
   void OnCoverageLoaded(const QString& aFileName);
   void OnCoverageUnloaded(const QString& aFileName);
   void OnCoverageVisibilityChanged(const QString& aFileName, bool aVisible);
   void OnRequestCenterOnCoverage(const QString& aFileName);
   void OnCoverageSelected(const QString& aFileName);
   void OnFieldChanged(const QString& aFileName, const QString& aFieldName);
   void OnOpacityChanged(const QString& aFileName, int aValue);
   void OnAutoScaleStateChanged(const QString& aFileName, bool aChecked);
   void OnRangeChanged(const QString& aFileName, float aMinValue, float aMaxValue);
   void OnGradientSelected(const QString& aFileName, const QString& aGradientName);

   void MapProjectionChanged(unsigned int aViewerID, const UtoMapProjection* aProjection);

   CoverageDataManager                mManager{};
   std::map<QString, HeatmapOverlay*> mOverlays{};
   CoverageOverlayDockWidget*         mDockWidgetPtr{nullptr};
   UtQtGradientMap                    mGradientMap{};
   vespa::VaCallbackHolder            mCallbacks{};
};

} // namespace CoverageOverlay

#endif // COVERAGEOVERLAYPLUGIN_HPP
