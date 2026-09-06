// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "MapCursorStatusUpdater.hpp"

#include <sstream>

#include "VaObserver.hpp"
#include "VaPosition.hpp"
#include "WkfEnvironment.hpp"
#include "WkfScenario.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfVtkEnvironment.hpp"

namespace
{
const int cINVALID_POSITION = -1;
}

Map::CursorUpdater::CursorUpdater(const QString& aName, const vespa::VaViewer& aViewer)
   : wkf::Updater(aName, "")
   , mViewer(aViewer)
{
   mCallbacks.Add(vespa::VaObserver::UpdateCursorPosition.Connect(&CursorUpdater::ReadData, this, mViewer.GetUniqueId()));
}

// Position
QString Map::PositionUpdater::GetValueString() const
{
   std::ostringstream oss;
   oss << mLat << " " << mLon;
   return QString::fromStdString(oss.str());
}

void Map::PositionUpdater::ReadData(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY)
{
   vespa::VaPosition curPos;
   mViewer.ScreenToWorld(aMouseX, aMouseY, curPos);
   curPos.SetLatFormat(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
   curPos.SetLonFormat(wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetLatLonFormatting());
   mLat = curPos.GetLat();
   mLon = curPos.GetLon();
}

Map::ElevationUpdater::ElevationUpdater(const vespa::VaViewer& aViewer, size_t aDtedId)
   : CursorUpdater("Elevation", aViewer)
   , mDtedId(aDtedId)
   , mMouseX(cINVALID_POSITION)
   , mMouseY(cINVALID_POSITION)
{
   QObject::connect(&mTimer,
                    &QTimer::timeout,
                    [this]()
                    {
                       if (mMouseX != cINVALID_POSITION && mMouseY != cINVALID_POSITION)
                       {
                          ReadData(nullptr, mMouseX, mMouseY);
                       }
                    });
   mTimer.start(500);
}

// Elevation
QString Map::ElevationUpdater::GetValueString() const
{
   if (mElevationExists)
   {
      return wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetFormattedValue(wkf::ValueType::cALTITUDE, mElevation);
   }
   return "no data";
}

void Map::ElevationUpdater::ReadData(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY)
{
   vespa::VaPosition curPos;
   mViewer.ScreenToWorld(aMouseX, aMouseY, curPos);
   mElevationExists = wkfEnv.GetResourceManager().QueryElevation(mDtedId, curPos.GetLat(), curPos.GetLon(), mElevation);
   mMouseX          = aMouseX;
   mMouseY          = aMouseY;
}

// Bullseye Base
void Map::BullseyeUpdater::ReadData(vespa::VaViewer* aViewerPtr, int aMouseX, int aMouseY)
{
   vespa::VaPosition curPos;
   mViewer.ScreenToWorld(aMouseX, aMouseY, curPos);
   mBullseyeExists =
      (vaEnv.GetStandardScenario() &&
       vaEnv.GetStandardScenario()->GetRangeBearingFromNearestBullseye(curPos, mRange, mBearing, mNearestBullseye));
}

// Bullseye Bearing
QString Map::BullseyeBearingUpdater::GetValueString() const
{
   if (mBullseyeExists)
   {
      return wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetFormattedValue(wkf::ValueType::cANGLE,
                                                                               mBearing * UtMath::cRAD_PER_DEG);
   }
   return "no bullseye";
}

// Bullseye Range
QString Map::BullseyeRangeUpdater::GetValueString() const
{
   if (mBullseyeExists)
   {
      return wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetFormattedValue(wkf::ValueType::cLENGTH, mRange);
   }
   return "no bullseye";
}

// Nearest Bullseye
QString Map::BullseyeNearestUpdater::GetValueString() const
{
   if (mBullseyeExists)
   {
      return QString::fromStdString(mNearestBullseye);
   }
   return "no bullseye";
}
