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

#include "CoverageOverlayPlugin.hpp"

#include "UtMemory.hpp"
#include "VaBoundingBox.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(CoverageOverlay::Plugin,
                          "Coverage Overlay",
                          "The Coverage Overlay plugin adds an option to the View menu to open a dockable window to "
                          "view coverage data in the map window.",
                          "all")

namespace CoverageOverlay
{

Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : wkf::Plugin(aPluginName, aUniqueId)
{
   InitializeGradients();

   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   mDockWidgetPtr                 = new CoverageOverlayDockWidget{mGradientMap, mainWindowPtr};
   mainWindowPtr->addDockWidget(Qt::RightDockWidgetArea, mDockWidgetPtr);
   mDockWidgetPtr->hide();

   connect(mDockWidgetPtr,
           &CoverageOverlayDockWidget::CoveragesSelectedForLoad,
           &mManager,
           &CoverageDataManager::LoadCoverageFiles);
   connect(mDockWidgetPtr,
           &CoverageOverlayDockWidget::CoverageSelectedForUnload,
           &mManager,
           &CoverageDataManager::UnloadCoverageData);

   connect(mDockWidgetPtr, &CoverageOverlayDockWidget::CoverageVisibilityChanged, this, &Plugin::OnCoverageVisibilityChanged);
   connect(mDockWidgetPtr, &CoverageOverlayDockWidget::RequestCenterOnCoverage, this, &Plugin::OnRequestCenterOnCoverage);
   connect(mDockWidgetPtr, &CoverageOverlayDockWidget::CoverageSelected, this, &Plugin::OnCoverageSelected);
   connect(mDockWidgetPtr, &CoverageOverlayDockWidget::FieldChanged, this, &Plugin::OnFieldChanged);
   connect(mDockWidgetPtr, &CoverageOverlayDockWidget::OpacityChanged, this, &Plugin::OnOpacityChanged);
   connect(mDockWidgetPtr, &CoverageOverlayDockWidget::AutoScaleStateChanged, this, &Plugin::OnAutoScaleStateChanged);
   connect(mDockWidgetPtr, &CoverageOverlayDockWidget::RangeChanged, this, &Plugin::OnRangeChanged);
   connect(mDockWidgetPtr, &CoverageOverlayDockWidget::GradientSelected, this, &Plugin::OnGradientSelected);
   connect(&mManager, &CoverageDataManager::CoverageLoaded, this, &Plugin::OnCoverageLoaded);
   connect(&mManager, &CoverageDataManager::CoverageUnloaded, this, &Plugin::OnCoverageUnloaded);

   mCallbacks.Add(wkf::Observer::MapProjectionChanged.Connect(&Plugin::MapProjectionChanged, this));
}

void Plugin::InitializeGradients()
{
   // Loop over predefined gradients and remove the transparent stop
   auto gradList = mGradientMap.Gradients();
   for (auto& grad : gradList)
   {
      if (grad.second.front().second == Qt::transparent)
      {
         grad.second.pop_front();
         grad.second.front().first = 0.0;
         mGradientMap.SetGradientStops(grad.first, grad.second);
      }
      else if (grad.second.back().second == Qt::transparent)
      {
         grad.second.pop_back();
         grad.second.back().first = 1.0;
         mGradientMap.SetGradientStops(grad.first, grad.second);
      }
   }
}

void Plugin::OnCoverageLoaded(const QString& aFileName)
{
   auto iter = mOverlays.find(aFileName);
   if (iter == mOverlays.end())
   {
      auto fieldsPtr = mManager.GetCoverageData(aFileName);
      if (fieldsPtr)
      {
         wkf::Viewer* viewerPtr = vaEnv.GetStandardViewer();
         if (viewerPtr)
         {
            auto overlayPtr = new HeatmapOverlay{*fieldsPtr, mGradientMap};
            mOverlays.emplace(aFileName, overlayPtr);
            viewerPtr->AddOverlay(overlayPtr);
            // Make sure that the new overlay has an approriate range of values.
            overlayPtr->SetAutoFieldRange(0);
         }
         else
         {
            // No viewer, so we unload the data.
            mManager.UnloadCoverageData(aFileName);
         }
      }
      // else
      //    The data manager announced a file that does not seem to exist.
      //    We can ignore this case.
   }
   // else
   //    The manager has announced a file for which there is already an overlay.
   //    No action needs to be taken.

   mDockWidgetPtr->CoverageLoaded(aFileName);
}

void Plugin::OnCoverageUnloaded(const QString& aFileName)
{
   auto iter = mOverlays.find(aFileName);
   if (iter != mOverlays.end())
   {
      // Remove the overlay from the viewer
      wkf::Viewer* viewerPtr = vaEnv.GetStandardViewer();
      if (viewerPtr)
      {
         viewerPtr->RemoveOverlay(iter->second->GetName());
         mOverlays.erase(iter);
      }
   }
   mDockWidgetPtr->CoverageUnloaded(aFileName);
}

void Plugin::OnCoverageVisibilityChanged(const QString& aFileName, bool aVisible)
{
   auto iter = mOverlays.find(aFileName);
   if (iter != mOverlays.end())
   {
      iter->second->SetVisible(aVisible);
   }
}

void Plugin::OnRequestCenterOnCoverage(const QString& aFileName)
{
   wkf::Viewer* viewerPtr = vaEnv.GetStandardViewer();
   if (viewerPtr)
   {
      auto fieldsPtr = mManager.GetCoverageData(aFileName);
      if (fieldsPtr)
      {
         viewerPtr->CenterOnBoundingBox(fieldsPtr->GetBoundingBox());
      }
   }
}

void Plugin::OnCoverageSelected(const QString& aFileName)
{
   auto iter = mOverlays.find(aFileName);
   if (iter != mOverlays.end())
   {
      auto        names = iter->second->GetFieldNames();
      QStringList argNames{};
      for (auto& name : names)
      {
         argNames.push_back(QString::fromStdString(name));
      }

      auto field = iter->second->GetCurrentField();
      if (!iter->second->HasFieldRange(field))
      {
         iter->second->SetAutoFieldRange(field);
      }
      bool  autoRange;
      float rangeMin;
      float rangeMax;
      iter->second->GetFieldRange(field, rangeMin, rangeMax, autoRange);
      mDockWidgetPtr->SetOptions(QString::fromStdString(iter->second->GetHeatmapName()),
                                 argNames,
                                 QString::fromStdString(iter->second->GetCurrentFieldName()),
                                 QString::fromStdString(iter->second->GetCurrentFieldType()),
                                 QString::fromStdString(iter->second->GetCurrentFieldSubtype()),
                                 iter->second->GetCurrentFieldUnitType(),
                                 iter->second->GetOpacity(),
                                 autoRange,
                                 rangeMin,
                                 rangeMax,
                                 iter->second->GetGradientSelection());
   }
   else
   {
      mDockWidgetPtr->ClearOptions();
   }
}

void Plugin::OnFieldChanged(const QString& aFileName, const QString& aFieldName)
{
   auto iter = mOverlays.find(aFileName);
   if (iter != mOverlays.end())
   {
      iter->second->SetCurrentField(aFieldName.toStdString());

      auto field = iter->second->GetCurrentField();
      if (!iter->second->HasFieldRange(field))
      {
         iter->second->SetAutoFieldRange(field);
      }
      bool  autoRange;
      float rangeMin;
      float rangeMax;
      iter->second->GetFieldRange(field, rangeMin, rangeMax, autoRange);
      mDockWidgetPtr->SetOptions(QString::fromStdString(iter->second->GetCurrentFieldType()),
                                 QString::fromStdString(iter->second->GetCurrentFieldSubtype()),
                                 iter->second->GetCurrentFieldUnitType(),
                                 iter->second->GetOpacity(),
                                 autoRange,
                                 rangeMin,
                                 rangeMax,
                                 iter->second->GetGradientSelection());
   }
}

void Plugin::OnOpacityChanged(const QString& aFileName, int aValue)
{
   auto iter = mOverlays.find(aFileName);
   if (iter != mOverlays.end())
   {
      iter->second->SetOpacity(aValue);
   }
}

void Plugin::OnAutoScaleStateChanged(const QString& aFileName, bool aChecked)
{
   auto iter = mOverlays.find(aFileName);
   if (iter != mOverlays.end())
   {
      if (aChecked)
      {
         iter->second->SetAutoFieldRange(iter->second->GetCurrentField());
         bool  isAuto;
         float minVal;
         float maxVal;
         iter->second->GetFieldRange(iter->second->GetCurrentField(), minVal, maxVal, isAuto);
         mDockWidgetPtr->SetValueRange(minVal, maxVal);
      }
      else
      {
         bool  isAuto;
         float minVal;
         float maxVal;
         iter->second->GetFieldRange(iter->second->GetCurrentField(), minVal, maxVal, isAuto);
         iter->second->SetFieldRange(iter->second->GetCurrentField(), minVal, maxVal, aChecked);
      }
   }
}

void Plugin::OnRangeChanged(const QString& aFileName, float aMinValue, float aMaxValue)
{
   auto iter = mOverlays.find(aFileName);
   if (iter != mOverlays.end())
   {
      bool  isAuto;
      float minVal;
      float maxVal;
      iter->second->GetFieldRange(iter->second->GetCurrentField(), minVal, maxVal, isAuto);
      iter->second->SetFieldRange(iter->second->GetCurrentField(), aMinValue, aMaxValue, isAuto);
   }
}

void Plugin::OnGradientSelected(const QString& aFileName, const QString& aGradientName)
{
   auto iter = mOverlays.find(aFileName);
   if (iter != mOverlays.end())
   {
      iter->second->SetGradientSelection(aGradientName);
   }
}

void Plugin::MapProjectionChanged(unsigned int aViewerID, const UtoMapProjection* aProjection)
{
   for (auto& overlay : mOverlays)
   {
      overlay.second->RegenerateGeometry();
   }
}

} // namespace CoverageOverlay
