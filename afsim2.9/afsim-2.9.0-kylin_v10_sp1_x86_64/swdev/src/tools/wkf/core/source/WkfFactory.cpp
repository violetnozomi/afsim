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

#include "WkfFactory.hpp"

#include <QMainWindow>

#include "UtQtMemory.hpp"
#include "VaAttachmentAngles.hpp"
#include "VaOverlayMapGrid.hpp"
#include "VaOverlayMapRings.hpp"
#include "VaUtils.hpp"
#include "WkfAboutDialog.hpp"
#include "WkfAttachmentGreatCircleRuler.hpp"
#include "WkfAttachmentLabel.hpp"
#include "WkfBullseye.hpp"
#include "WkfDocumentationDialog.hpp"
#include "WkfMapLayerDockWidget.hpp"
#include "WkfOverlaySpace.hpp"
#include "WkfPermissionDialog.hpp"
#include "WkfPlatform.hpp"
#include "WkfPlatformDataDockWidget.hpp"
#include "WkfPlotDockWidget.hpp"
#include "WkfPointOfInterest.hpp"
#include "WkfPreferencesDialog.hpp"
#include "WkfScenario.hpp"

wkf::Dialog* wkf::Factory::CreateDialog(const std::string& aDialogType, MainWindow* aParent) const
{
   Dialog* dialogPtr = nullptr;

   if (aDialogType == "About")
   {
      dialogPtr = new AboutDialog(aParent);
   }
   else if (aDialogType == "Documentation")
   {
      dialogPtr = new DocumentationDialog(aParent);
   }
   else if (aDialogType == "Preferences")
   {
      dialogPtr = new PreferencesDialog(aParent);
   }
   else if (aDialogType == "Permission")
   {
      dialogPtr = ut::qt::make_qt_ptr<wkf::permission::PermissionDialog>(aParent);
   }

   return dialogPtr;
}

QDockWidget* wkf::Factory::CreateDockWidget(const std::string& aDockWidgetType, QMainWindow* aParent) const
{
   QDockWidget* dockWidgetPtr = nullptr;

   if (aDockWidgetType == "PlottingWidget")
   {
      dockWidgetPtr = new PlotDockWidget(aParent);
   }
   else if (aDockWidgetType == "PlatformData")
   {
      dockWidgetPtr = new PlatformDataDockWidget(aParent);
   }
   else if (aDockWidgetType == "MapLayer")
   {
      dockWidgetPtr = new MapLayerDockWidget(aParent);
   }

   return dockWidgetPtr;
}

QWidget* wkf::Factory::CreateWidget(const std::string& aWidgetType, QMainWindow* aParent) const
{
   // TODO: Can wkf_common be included here via cmake?
   return new QWidget(aParent);
}

vespa::VaEntity* wkf::Factory::UserCreateEntity(const std::string& aEntityType) const
{
   if (aEntityType == "platform")
   {
      return new Platform("");
   }
   else if (aEntityType == "pointOfInterest")
   {
      return new PointOfInterest();
   }
   else if (aEntityType == "bullseye")
   {
      return new Bullseye();
   }
   return nullptr;
}

vespa::VaScenario* wkf::Factory::UserCreateScenario(const std::string& aScenarioType) const
{
   if (aScenarioType == "wkf")
   {
      return new Scenario();
   }
   return nullptr;
}

vespa::VaOverlay* wkf::Factory::UserCreateOverlay(const std::string& aOverlayType,
                                                  const std::string& aOverlayName /* = "" */) const
{
   vespa::VaOverlay* overlayPtr = nullptr;
   if (aOverlayType == "graticule")
   {
      if (!aOverlayName.empty())
      {
         overlayPtr = new vespa::VaOverlayMapGrid(aOverlayName);
      }
      else
      {
         overlayPtr = new vespa::VaOverlayMapGrid();
      }
   }
   else if (aOverlayType == "maprings")
   {
      if (!aOverlayName.empty())
      {
         overlayPtr = new vespa::VaOverlayMapRings(aOverlayName);
      }
      else
      {
         overlayPtr = new vespa::VaOverlayMapRings();
      }
   }
   else if (aOverlayType == "space")
   {
      if (aOverlayName.empty())
      {
         overlayPtr = new wkf::OverlaySpace();
      }
      else
      {
         overlayPtr = new wkf::OverlaySpace(aOverlayName);
      }
   }
   return overlayPtr;
}

vespa::VaAttachment* wkf::Factory::UserCreateAttachment(const std::string& aAttachmentType,
                                                        vespa::VaEntity&   aEntity,
                                                        vespa::VaViewer*   aViewerPtr) const
{
   if (aAttachmentType == "angles")
   {
      return vespa::make_attachment<vespa::VaAttachmentAngles>(aEntity, aViewerPtr);
   }
   else if (aAttachmentType == "great_circle_ruler")
   {
      return vespa::make_attachment<wkf::AttachmentGreatCircleRuler>(aEntity, aViewerPtr);
   }
   else if (aAttachmentType == "label")
   {
      return vespa::make_attachment<wkf::AttachmentLabel>(aEntity, aViewerPtr);
   }
   return nullptr;
}
