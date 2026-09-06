// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "MapCursorStatusWidget.hpp"

#include <sstream>

#include "UtAngle.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"
#include "WkfBullseye.hpp"
#include "WkfEnvironment.hpp"
#include "WkfResourceManager.hpp"
#include "WkfScenario.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfVtkEnvironment.hpp"

Map::CursorStatusWidget::CursorStatusWidget(const vespa::VaViewer& aViewer)
   : QLabel()
   , mViewer(aViewer)
   , mMouseX(-1)
   , mMouseY(-1)
{
   setContentsMargins(20, 0, 20, 0);

   mCallbacks.Add(vespa::VaObserver::UpdateCursorPosition.Connect(&CursorStatusWidget::UpdateCursorPositionCB,
                                                                  this,
                                                                  mViewer.GetUniqueId()));
   mDtedRegistrationId = wkfEnv.GetResourceManager().Register();
}

void Map::CursorStatusWidget::UpdateCursorPositionCB(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY)
{
   mMouseX = aMouseX;
   mMouseY = aMouseY;
   Update();
}

void Map::CursorStatusWidget::Update()
{
   // Make sure we have a valid mouse location.
   if ((mMouseX >= 0) && (mMouseY >= 0))
   {
      // Format the status line
      std::ostringstream oss;
      UtAngle::SetDefaultFormat(UtAngle::FmtDMS + 3);

      vespa::VaPosition curPos;
      mViewer.ScreenToWorld(mMouseX, mMouseY, curPos);

      curPos.SetLatFormat(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
      curPos.SetLonFormat(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());

      double      range   = 0;
      double      bearing = 0;
      std::string bullseyeName;
      if (vaEnv.GetStandardScenario() &&
          vaEnv.GetStandardScenario()->GetRangeBearingFromNearestBullseye(curPos, range, bearing, bullseyeName))
      {
         auto rangeStr =
            wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetFormattedValue(wkf::ValueType::cLENGTH, range).toStdString();
         // bearing needs to be converted from degrees to radians since GetFormattedValue() expects defaults units and
         // radians is the default
         auto bearingStr = wkfEnv.GetPreferenceObject<wkf::UnitsObject>()
                              ->GetFormattedValue(wkf::ValueType::cANGLE, bearing * UtMath::cRAD_PER_DEG)
                              .toStdString();
         oss << rangeStr << " @ " << bearingStr << " from " << bullseyeName << " | ";
      }

      float alt;
      if (mShowTerrain &&
          wkfEnv.GetResourceManager().QueryElevation(mDtedRegistrationId, curPos.GetLat(), curPos.GetLon(), alt))
      {
         std::string formatted =
            wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetFormattedValue(wkf::ValueType::cALTITUDE, alt).toStdString();
         oss << curPos.GetLat() << "   " << curPos.GetLon() << "   " << formatted;
      }
      else
      {
         oss << curPos.GetLat() << "   " << curPos.GetLon();
      }

      setText(oss.str().c_str());
   }
}
