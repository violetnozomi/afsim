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

#include "RvFactory.hpp"

#include <QMainWindow>

#include "RvAboutDialog.hpp"
#include "RvDocumentationDialog.hpp"
#include "RvPlatformDataDockWidget.hpp"
#include "RvPlottingWidget.hpp"
#include "RvPreferencesDialog.hpp"
#include "annotation/WkfPointOfInterestPropertiesWidget.hpp"
#include "annotation/WkfRingPropertiesWidget.hpp"
#include "visibility/WkfVisibilityDockWidget.hpp"
#include "wsf_draw/WkfOverlayWsfDraw.hpp"

#ifdef CreateDialog
#undef CreateDialog
#endif

wkf::Dialog* rv::Factory::CreateDialog(const std::string& aDialogType, wkf::MainWindow* aParent /*= nullptr */) const
{
   if (aDialogType == "About")
   {
      return new rv::AboutDialog(aParent);
   }
   else if (aDialogType == "Preferences")
   {
      return new PreferencesDialog(aParent);
   }
   else if (aDialogType == "Documentation")
   {
      return new rv::DocumentationDialog(aParent);
   }
   return wkf::Factory::CreateDialog(aDialogType, aParent);
}

QDockWidget* rv::Factory::CreateDockWidget(const std::string& aDockWidgetType, QMainWindow* aParent /*= 0*/) const
{
   if (aDockWidgetType == "PlatformData")
   {
      return new PlatformDataDockWidget(aParent);
   }
   else if (aDockWidgetType == "PlottingWidget")
   {
      return new PlottingWidget(aParent);
   }
   else if (aDockWidgetType == "VisibilityWidget")
   {
      return new wkf::VisibilityDockWidget(aParent);
   }

   return wkf::Factory::CreateDockWidget(aDockWidgetType, aParent);
}

QWidget* rv::Factory::CreateWidget(const std::string& aWidgetType, QMainWindow* aParent) const
{
   if (aWidgetType == "PointOfInterestProperties")
   {
      return new Annotation::PointOfInterestPropertiesWidget(aParent);
   }
   else if (aWidgetType == "RangeRingProperties")
   {
      return new Annotation::RingPropertiesWidget(aParent);
   }
   return wkf::Factory::CreateWidget(aWidgetType, aParent);
}

void rv::Factory::UserOpenFile(const QStringList& aFileNames) const
{
   if (aFileNames[0].endsWith(".aer", Qt::CaseInsensitive))
   {
      rvEnv.OpenEventRecording(aFileNames[0]);
   }
}

vespa::VaOverlay* rv::Factory::UserCreateOverlay(const std::string& aOverlayType,
                                                 const std::string& aOverlayName /*= ""*/) const
{
   if (aOverlayType == "wsfdraw")
   {
      if (!aOverlayName.empty())
      {
         return new wkf::OverlayWsfDraw(aOverlayName);
      }
      else
      {
         return new wkf::OverlayWsfDraw();
      }
   }
   else
   {
      return wkf::Factory::UserCreateOverlay(aOverlayType, aOverlayName);
   }
}
