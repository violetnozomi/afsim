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
#include "MapPrefObject.hpp"

#include <QSettings>

#include "MapOverlayScale.hpp"
#include "UtColor.hpp"
#include "UtPath.hpp"
#include "UtRunEnvManager.hpp"
#include "UtoCmeTerrain.hpp"
#include "VaEphemeris.hpp"
#include "VaObserver.hpp"
#include "VaOverlayCompass.hpp"
#include "VaOverlayElevationLines.hpp"
#include "VaOverlayMapGrid.hpp"
#include "VaOverlayMapRings.hpp"
#include "VaOverlayShapeFile.hpp"
#include "VaOverlayWdb.hpp"
#include "WkfEnvironment.hpp"
#include "WkfOverlaySpace.hpp"
#include "WkfSharedMapPreferencesObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"

Map::PrefObject::PrefObject(QObject* parent)
   : wkf::PrefObjectT<PrefData>(parent, "MapDisplay")
{
}

void Map::PrefObject::Apply()
{
   PrefData& pd = mCurrentPrefs;

   vespa::VaViewer* viewer = vaEnv.GetStandardViewer();

   if (viewer && viewer->IsInitialized())
   {
      vespa::VaObserver::SetIntensity[viewer->GetUniqueId()](static_cast<float>(pd.mIntensity));
      vespa::VaObserver::SetSaturation[viewer->GetUniqueId()](static_cast<float>(pd.mSaturation));
      vespa::VaObserver::SetContrast[viewer->GetUniqueId()](static_cast<float>(pd.mContrast));

      if (pd.mGridLines)
      {
         if (nullptr == mMapGridPtr)
         {
            mMapGridPtr =
               dynamic_cast<vespa::VaOverlayMapGrid*>(vaEnv.GetFactory()->CreateOverlay("graticule", "graticule"));
            viewer->AddOverlay(mMapGridPtr);
         }
         float color[4] = {static_cast<float>(pd.mGridColor.redF()),
                           static_cast<float>(pd.mGridColor.greenF()),
                           static_cast<float>(pd.mGridColor.blueF()),
                           static_cast<float>(pd.mGridColor.alphaF())};
         float density  = pd.mGridDensity;
         mMapGridPtr->SetColor(color);
         mMapGridPtr->SetDensityFactor(density);
         mMapGridPtr->SetVisible(true);
      }
      else if (nullptr != mMapGridPtr)
      {
         mMapGridPtr->SetVisible(false);
      }
      if (pd.mElevationLines)
      {
         if (nullptr == mMapElevationLinesPtr)
         {
            mMapElevationLinesPtr = dynamic_cast<vespa::VaOverlayElevationLines*>(
               vaEnv.GetFactory()->CreateOverlay("elevationlines", "elevationlines"));
            viewer->AddOverlay(mMapElevationLinesPtr);
         }

         mMapElevationLinesPtr->SetDatasetFileName(pd.mElevationSource.toStdString());
         mMapElevationLinesPtr->SetElevationStep(pd.mElevationSpacing);
         mMapElevationLinesPtr->SetElevationColorRange(pd.mElevationMin, pd.mElevationMax);
         mMapElevationLinesPtr->SetVisible(true);
      }
      else if (nullptr != mMapElevationLinesPtr)
      {
         mMapElevationLinesPtr->SetVisible(false);
      }
      if (pd.mCoastLines)
      {
         if (nullptr == mCoastPtr)
         {
            mCoastPtr = dynamic_cast<vespa::VaOverlayShapeFile*>(vaEnv.GetFactory()->CreateOverlay("shapefile", "coast"));
            viewer->AddOverlay(mCoastPtr);
            std::string coasturl = UtRunEnvManager::GetCmeRelBase() + "/maps/layers/ne_50m_coastline";
            mCoastPtr->LoadFile(coasturl);
         }
         float color[4] = {static_cast<float>(pd.mCoastColor.redF()),
                           static_cast<float>(pd.mCoastColor.greenF()),
                           static_cast<float>(pd.mCoastColor.blueF()),
                           static_cast<float>(pd.mCoastColor.alphaF())};
         mCoastPtr->SetColor(color);
         mCoastPtr->SetWidth(1.0f);
         mCoastPtr->SetVisible(true);
      }
      else if (mCoastPtr)
      {
         mCoastPtr->SetVisible(false);
      }
      if (pd.mCountryBorders)
      {
         if (nullptr == mCountryPtr)
         {
            mCountryPtr =
               dynamic_cast<vespa::VaOverlayShapeFile*>(vaEnv.GetFactory()->CreateOverlay("shapefile", "country"));
            viewer->AddOverlay(mCountryPtr);
            std::string countryurl = UtRunEnvManager::GetCmeRelBase() + "/maps/layers/pol";
            mCountryPtr->LoadFile(countryurl);
         }
         float color[4] = {static_cast<float>(pd.mCountryColor.redF()),
                           static_cast<float>(pd.mCountryColor.greenF()),
                           static_cast<float>(pd.mCountryColor.blueF()),
                           static_cast<float>(pd.mCountryColor.alphaF())};
         mCountryPtr->SetColor(color);
         mCountryPtr->SetWidth(1.0f);
         mCountryPtr->SetVisible(true);
      }
      else if (mCountryPtr)
      {
         mCountryPtr->SetVisible(false);
      }
      if (pd.mInternalBorders)
      {
         if (nullptr == mInternalPtr)
         {
            mInternalPtr =
               dynamic_cast<vespa::VaOverlayShapeFile*>(vaEnv.GetFactory()->CreateOverlay("shapefile", "internal"));
            viewer->AddOverlay(mInternalPtr);
            std::string internalurl = UtRunEnvManager::GetCmeRelBase() + "/maps/layers/us";
            mInternalPtr->LoadFile(internalurl);
         }
         float color[4] = {static_cast<float>(pd.mInternalColor.redF()),
                           static_cast<float>(pd.mInternalColor.greenF()),
                           static_cast<float>(pd.mInternalColor.blueF()),
                           static_cast<float>(pd.mInternalColor.alphaF())};
         mInternalPtr->SetColor(color);
         mInternalPtr->SetWidth(1.0f);
         mInternalPtr->SetVisible(true);
      }
      else if (mInternalPtr)
      {
         mInternalPtr->SetVisible(false);
      }

      if (pd.mLakesAndRivers)
      {
         if (nullptr == mWdbPtr)
         {
            mWdbPtr            = dynamic_cast<vespa::VaOverlayWdb*>(vaEnv.GetFactory()->CreateOverlay("wdb", "wdb"));
            std::string wdburl = UtRunEnvManager::GetCmeRelBase() + "/maps/wdb/";
            mWdbPtr->SetUrl(wdburl);
            viewer->AddOverlay(mWdbPtr);
         }
         float color[4] = {static_cast<float>(pd.mLakesColor.redF()),
                           static_cast<float>(pd.mLakesColor.greenF()),
                           static_cast<float>(pd.mLakesColor.blueF()),
                           static_cast<float>(pd.mLakesColor.alphaF())};
         mWdbPtr->SetColor("Lakes_and_Rivers", color);
         mWdbPtr->ShowLayer("Lakes_and_Rivers");
      }
      else if (nullptr != mWdbPtr)
      {
         mWdbPtr->HideLayer("Lakes_and_Rivers");
      }

      if (pd.mGlobalRangeRings)
      {
         if (nullptr == mMapRingsPtr)
         {
            mMapRingsPtr =
               dynamic_cast<vespa::VaOverlayMapRings*>(vaEnv.GetFactory()->CreateOverlay("maprings", "maprings"));
            viewer->AddOverlay(mMapRingsPtr);
         }
         float color[4] = {static_cast<float>(pd.mRingColor.redF()),
                           static_cast<float>(pd.mRingColor.greenF()),
                           static_cast<float>(pd.mRingColor.blueF()),
                           static_cast<float>(pd.mRingColor.alphaF())};
         mMapRingsPtr->SetColor(color);
         mMapRingsPtr->SetVisible(true);
      }
      else if (nullptr != mMapRingsPtr)
      {
         mMapRingsPtr->SetVisible(false);
      }
      if (pd.mTerminator)
      {
         if (nullptr == mSpaceOverlayPtr)
         {
            mSpaceOverlayPtr = dynamic_cast<wkf::OverlaySpace*>(vaEnv.GetFactory()->CreateOverlay("space", "space"));
            viewer->AddOverlay(mSpaceOverlayPtr);
         }
         float color[4] = {static_cast<float>(pd.mTerminatorColor.redF()),
                           static_cast<float>(pd.mTerminatorColor.greenF()),
                           static_cast<float>(pd.mTerminatorColor.blueF()),
                           static_cast<float>(pd.mTerminatorColor.alphaF())};
         mSpaceOverlayPtr->SetTerminatorColor(color);
         mSpaceOverlayPtr->SetTerminatorWidth(pd.mTerminatorWidth);
         mSpaceOverlayPtr->ShowTerminator(true);
         mSpaceOverlayPtr->SetUpdatePeriod(pd.mSolarUpdatePeriod);
      }
      else if (nullptr != mSpaceOverlayPtr)
      {
         mSpaceOverlayPtr->ShowTerminator(false);
      }
      if (pd.mSubSolarPoint)
      {
         if (nullptr == mSpaceOverlayPtr)
         {
            mSpaceOverlayPtr = dynamic_cast<wkf::OverlaySpace*>(vaEnv.GetFactory()->CreateOverlay("space", "space"));
            viewer->AddOverlay(mSpaceOverlayPtr);
         }
         float color[4] = {static_cast<float>(pd.mSubSolarColor.redF()),
                           static_cast<float>(pd.mSubSolarColor.greenF()),
                           static_cast<float>(pd.mSubSolarColor.blueF()),
                           static_cast<float>(pd.mSubSolarColor.alphaF())};
         mSpaceOverlayPtr->SetSubSolarPointColor(color);
         mSpaceOverlayPtr->SetSubSolarPointScale(pd.mSubSolarScale);
         if (pd.mSubSolarIconType == PrefData::DEFAULT)
         {
            mSpaceOverlayPtr->SetSubSolarPointIcon("");
         }
         else
         {
            mSpaceOverlayPtr->SetSubSolarPointIcon(pd.mSubSolarIconPath);
         }
         mSpaceOverlayPtr->ShowSubSolarPoint(true);
         mSpaceOverlayPtr->SetUpdatePeriod(pd.mSolarUpdatePeriod);
      }
      else if (nullptr != mSpaceOverlayPtr)
      {
         mSpaceOverlayPtr->ShowSubSolarPoint(false);
      }
      if (pd.mSubLunarPoint)
      {
         if (nullptr == mSpaceOverlayPtr)
         {
            mSpaceOverlayPtr = dynamic_cast<wkf::OverlaySpace*>(vaEnv.GetFactory()->CreateOverlay("space", "space"));
            viewer->AddOverlay(mSpaceOverlayPtr);
         }
         float color[4] = {static_cast<float>(pd.mSubLunarColor.redF()),
                           static_cast<float>(pd.mSubLunarColor.greenF()),
                           static_cast<float>(pd.mSubLunarColor.blueF()),
                           static_cast<float>(pd.mSubLunarColor.alphaF())};
         mSpaceOverlayPtr->SetSubLunarPointColor(color);
         mSpaceOverlayPtr->SetSubLunarPointScale(pd.mSubLunarScale);
         if (pd.mSubLunarIconType == PrefData::DEFAULT)
         {
            mSpaceOverlayPtr->SetSubLunarPointIcon("");
         }
         else
         {
            mSpaceOverlayPtr->SetSubLunarPointIcon(pd.mSubLunarIconPath);
         }
         mSpaceOverlayPtr->ShowSubLunarPoint(true);
      }
      else if (nullptr != mSpaceOverlayPtr)
      {
         mSpaceOverlayPtr->ShowSubLunarPoint(false);
      }
      if (pd.mSolarPath)
      {
         if (nullptr == mSpaceOverlayPtr)
         {
            mSpaceOverlayPtr = dynamic_cast<wkf::OverlaySpace*>(vaEnv.GetFactory()->CreateOverlay("space", "space"));
            viewer->AddOverlay(mSpaceOverlayPtr);
         }
         float color[4] = {static_cast<float>(pd.mSolarPathColor.redF()),
                           static_cast<float>(pd.mSolarPathColor.greenF()),
                           static_cast<float>(pd.mSolarPathColor.blueF()),
                           static_cast<float>(pd.mSolarPathColor.alphaF())};
         mSpaceOverlayPtr->SetSolarPathColor(color);
         mSpaceOverlayPtr->SetSolarPathWidth(pd.mSolarPathWidth);
         mSpaceOverlayPtr->ShowSolarPath(true);
         mSpaceOverlayPtr->SetUpdatePeriod(pd.mSolarUpdatePeriod);
      }
      else if (nullptr != mSpaceOverlayPtr)
      {
         mSpaceOverlayPtr->ShowSolarPath(false);
      }
      if (pd.mScale)
      {
         if (nullptr == mScalePtr)
         {
            mScalePtr = new OverlayScale("scale");
            viewer->AddOverlay(mScalePtr);
         }
         mScalePtr->SetVisible(true);
      }
      else if (nullptr != mScalePtr)
      {
         mScalePtr->SetVisible(false);
      }
      if (pd.mCompass)
      {
         if (nullptr == mCompassPtr)
         {
            mCompassPtr = dynamic_cast<vespa::VaOverlayCompass*>(vaEnv.GetFactory()->CreateOverlay("compass", "compass"));
            viewer->AddOverlay(mCompassPtr);
         }
         mCompassPtr->SetVisible(true);
      }
      else if (nullptr != mCompassPtr)
      {
         mCompassPtr->SetVisible(false);
      }

      ApplySharedMaps();

      viewer->SuppressTeamColor(!pd.mTeamColor);
      viewer->SetTrueScale(pd.mTrueScale);
      viewer->SetIconScale(pd.mTrueScale ? 1.0 : pd.mModelScale);
      viewer->EnableLighting(pd.mLighting);
      vespa::VaObserver::ECI_ModeChanged(pd.mECI);
      viewer->SetData("CenterOnStart", pd.mCenterOnStart);
      vespa::VaObserver::RedrawWindow();
   }
   else
   {
      mCallbacks.Add(vespa::VaObserver::ViewerInitialized.Connect(&Map::PrefObject::ViewerInitializedCB, this));
   }
}

void Map::PrefObject::ApplySharedMaps()
{
   PrefData& pd = mCurrentPrefs;

   auto* viewerPtr = vaEnv.GetStandardViewer();
   if (viewerPtr)
   {
      //   mActiveMaps = wkfEnv.GetPreferenceObject<wkf::SharedMapPreferencesObject>()->SetMap(pd.mActiveMap, viewer, mActiveMaps);

      viewerPtr->SuppressTeamColor(!pd.mTeamColor);
      viewerPtr->SetTrueScale(pd.mTrueScale);
      viewerPtr->SetIconScale(pd.mTrueScale ? 1.0 : pd.mModelScale);
      viewerPtr->EnableLighting(pd.mLighting);
      vespa::VaObserver::ECI_ModeChanged(pd.mECI);
   }
   // nothing to do for mCenterOnStart
   emit PreferencesChanged(QVariant::fromValue(pd));
}

Map::PrefData Map::PrefObject::ReadSettings(QSettings& aSettings) const
{
   PrefData        prefs;
   const PrefData& defaults = mDefaultPrefs;

   aSettings.beginGroup("MapOptions");
   prefs.mIntensity         = aSettings.value("intensity", defaults.mIntensity).toFloat();
   prefs.mSaturation        = aSettings.value("saturation", defaults.mSaturation).toFloat();
   prefs.mContrast          = aSettings.value("contrast", defaults.mContrast).toFloat();
   prefs.mGridLines         = aSettings.value("gridLines", defaults.mGridLines).toBool();
   prefs.mGridColor         = aSettings.value("gridColor", defaults.mGridColor).value<QColor>();
   prefs.mGridDensity       = aSettings.value("gridDensity", defaults.mGridDensity).toFloat();
   prefs.mElevationLines    = aSettings.value("elevationLines", defaults.mElevationLines).toBool();
   prefs.mElevationSpacing  = aSettings.value("elevationSpacing", defaults.mElevationSpacing).toInt();
   prefs.mElevationMin      = aSettings.value("elevationMin", defaults.mElevationMin).toInt();
   prefs.mElevationMax      = aSettings.value("elevationMax", defaults.mElevationMax).toInt();
   prefs.mElevationSource   = aSettings.value("elevationSource", defaults.mElevationSource).toString();
   prefs.mCoastLines        = aSettings.value("coastLines", defaults.mCoastLines).toBool();
   prefs.mCoastColor        = aSettings.value("coastColor", defaults.mCoastColor).value<QColor>();
   prefs.mCountryBorders    = aSettings.value("countryBorders", defaults.mCountryBorders).toBool();
   prefs.mCountryColor      = aSettings.value("countryColor", defaults.mCountryColor).value<QColor>();
   prefs.mInternalBorders   = aSettings.value("internalBorders", defaults.mInternalBorders).toBool();
   prefs.mInternalColor     = aSettings.value("internalColor", defaults.mInternalColor).value<QColor>();
   prefs.mLakesAndRivers    = aSettings.value("lakesRivers", defaults.mLakesAndRivers).toBool();
   prefs.mLakesColor        = aSettings.value("lakesColor", defaults.mLakesColor).value<QColor>();
   prefs.mGlobalRangeRings  = aSettings.value("globalRangeRing", defaults.mGlobalRangeRings).toBool();
   prefs.mRingColor         = aSettings.value("globalRingColor", defaults.mRingColor).value<QColor>();
   prefs.mScale             = aSettings.value("scale", defaults.mScale).toBool();
   prefs.mCompass           = aSettings.value("compass", defaults.mCompass).toBool();
   prefs.mTerminator        = aSettings.value("terminator", defaults.mTerminator).toBool();
   prefs.mTerminatorColor   = aSettings.value("terminatorColor", defaults.mTerminatorColor).value<QColor>();
   prefs.mTerminatorWidth   = aSettings.value("terminatorWidth", defaults.mTerminatorWidth).toUInt();
   prefs.mSubSolarPoint     = aSettings.value("SubSolarPoint", defaults.mSubSolarPoint).toBool();
   prefs.mSubSolarColor     = aSettings.value("SubSolarColor", defaults.mSubSolarColor).value<QColor>();
   prefs.mSubSolarScale     = aSettings.value("SubSolarScale", defaults.mSubSolarScale).toDouble();
   prefs.mSubSolarIconPath  = aSettings.value("SubSolarIcon", defaults.mSubSolarIconPath).toString();
   prefs.mSubSolarIconType  = prefs.mSubSolarIconPath.isEmpty() ? PrefData::DEFAULT : PrefData::CUSTOM;
   prefs.mSubLunarPoint     = aSettings.value("SubLunarPoint", defaults.mSubLunarPoint).toBool();
   prefs.mSubLunarColor     = aSettings.value("SubLunarColor", defaults.mSubLunarColor).value<QColor>();
   prefs.mSubLunarScale     = aSettings.value("SubLunarScale", defaults.mSubLunarScale).toDouble();
   prefs.mSubLunarIconPath  = aSettings.value("SubLunarIcon", defaults.mSubLunarIconPath).toString();
   prefs.mSubLunarIconType  = prefs.mSubLunarIconPath.isEmpty() ? PrefData::DEFAULT : PrefData::CUSTOM;
   prefs.mSolarPath         = aSettings.value("SolarPath", defaults.mSolarPath).toBool();
   prefs.mSolarPathColor    = aSettings.value("SolarPathColor", defaults.mSolarPathColor).value<QColor>();
   prefs.mSolarPathWidth    = aSettings.value("SolarPathWidth", defaults.mSolarPathWidth).toUInt();
   prefs.mSolarUpdatePeriod = aSettings.value("SolarUpdatePeriod", defaults.mSolarUpdatePeriod).toDouble();

   prefs.mActiveMap = aSettings.value("activeMap", defaults.mActiveMap.c_str()).toString().toStdString();

   prefs.mLighting      = aSettings.value("lighting", defaults.mLighting).toBool();
   prefs.mECI           = aSettings.value("eci", defaults.mECI).toBool();
   prefs.mCenterOnStart = aSettings.value("centerOnStart", defaults.mCenterOnStart).toBool();
   prefs.mShowTerrainAltitudeAtCursor =
      aSettings.value("showTerrainAltitudeAtCursor", defaults.mShowTerrainAltitudeAtCursor).toBool();
   prefs.mZoomOnCursor = aSettings.value("zoomOnCursor", defaults.mZoomOnCursor).toBool();
   aSettings.endGroup();
   aSettings.beginGroup("ModelOptions");
   prefs.mTeamColor  = aSettings.value("teamcolor", defaults.mTeamColor).toBool();
   prefs.mTrueScale  = aSettings.value("trueScale", defaults.mTrueScale).toBool();
   prefs.mModelScale = aSettings.value("modelScale", defaults.mModelScale).toFloat();
   prefs.mFont       = aSettings.value("font", defaults.mFont).value<QFont>();
   aSettings.endGroup();

   return prefs;
}

void Map::PrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.beginGroup("MapOptions");
   aSettings.setValue("intensity", mCurrentPrefs.mIntensity);
   aSettings.setValue("saturation", mCurrentPrefs.mSaturation);
   aSettings.setValue("contrast", mCurrentPrefs.mContrast);
   aSettings.setValue("gridLines", mCurrentPrefs.mGridLines);
   aSettings.setValue("gridColor", mCurrentPrefs.mGridColor);
   aSettings.setValue("gridDensity", mCurrentPrefs.mGridDensity);
   aSettings.setValue("elevationLines", mCurrentPrefs.mElevationLines);
   aSettings.setValue("elevationSpacing", mCurrentPrefs.mElevationSpacing);
   aSettings.setValue("elevationMin", mCurrentPrefs.mElevationMin);
   aSettings.setValue("elevationMax", mCurrentPrefs.mElevationMax);
   aSettings.setValue("elevationSource", mCurrentPrefs.mElevationSource);
   aSettings.setValue("coastLines", mCurrentPrefs.mCoastLines);
   aSettings.setValue("coastColor", mCurrentPrefs.mCoastColor);
   aSettings.setValue("countryBorders", mCurrentPrefs.mCountryBorders);
   aSettings.setValue("countryColor", mCurrentPrefs.mCountryColor);
   aSettings.setValue("internalBorders", mCurrentPrefs.mInternalBorders);
   aSettings.setValue("internalColor", mCurrentPrefs.mInternalColor);
   aSettings.setValue("lakesRivers", mCurrentPrefs.mLakesAndRivers);
   aSettings.setValue("lakesColor", mCurrentPrefs.mLakesColor);
   aSettings.setValue("globalRangeRing", mCurrentPrefs.mGlobalRangeRings);
   aSettings.setValue("globalRingColor", mCurrentPrefs.mRingColor);
   aSettings.setValue("scale", mCurrentPrefs.mScale);
   aSettings.setValue("compass", mCurrentPrefs.mCompass);
   aSettings.setValue("terminator", mCurrentPrefs.mTerminator);
   aSettings.setValue("terminatorColor", mCurrentPrefs.mTerminatorColor);
   aSettings.setValue("terminatorWidth", mCurrentPrefs.mTerminatorWidth);
   aSettings.setValue("SubSolarPoint", mCurrentPrefs.mSubSolarPoint);
   aSettings.setValue("SubSolarColor", mCurrentPrefs.mSubSolarColor);
   aSettings.setValue("SubSolarScale", mCurrentPrefs.mSubSolarScale);
   if (mCurrentPrefs.mSubSolarIconType == PrefData::DEFAULT)
   {
      aSettings.setValue("SubSolarIcon", "");
   }
   else
   {
      aSettings.setValue("SubSolarIcon", mCurrentPrefs.mSubSolarIconPath);
   }
   aSettings.setValue("SubLunarPoint", mCurrentPrefs.mSubLunarPoint);
   aSettings.setValue("SubLunarColor", mCurrentPrefs.mSubLunarColor);
   aSettings.setValue("SubLunarScale", mCurrentPrefs.mSubLunarScale);
   if (mCurrentPrefs.mSubLunarIconType == PrefData::DEFAULT)
   {
      aSettings.setValue("SubLunarIcon", "");
   }
   else
   {
      aSettings.setValue("SubLunarIcon", mCurrentPrefs.mSubLunarIconPath);
   }
   aSettings.setValue("SolarPath", mCurrentPrefs.mSolarPath);
   aSettings.setValue("SolarPathColor", mCurrentPrefs.mSolarPathColor);
   aSettings.setValue("SolarPathWidth", mCurrentPrefs.mSolarPathWidth);
   aSettings.setValue("SolarUpdatePeriod", mCurrentPrefs.mSolarUpdatePeriod);

   aSettings.setValue("activeMap", mCurrentPrefs.mActiveMap.c_str());
   aSettings.setValue("lighting", mCurrentPrefs.mLighting);
   aSettings.setValue("eci", mCurrentPrefs.mECI);
   aSettings.setValue("centerOnStart", mCurrentPrefs.mCenterOnStart);
   aSettings.setValue("showTerrainAltitudeAtCursor", mCurrentPrefs.mShowTerrainAltitudeAtCursor);
   aSettings.setValue("zoomOnCursor", mCurrentPrefs.mZoomOnCursor);
   aSettings.endGroup();

   aSettings.beginGroup("ModelOptions");
   aSettings.setValue("teamcolor", mCurrentPrefs.mTeamColor);
   aSettings.setValue("trueScale", mCurrentPrefs.mTrueScale);
   aSettings.setValue("modelScale", mCurrentPrefs.mModelScale);
   aSettings.setValue("font", mCurrentPrefs.mFont);
   aSettings.endGroup();
}

void Map::PrefObject::ViewerInitializedCB(vespa::VaViewer* aViewerPtr)
{
   if (aViewerPtr == vaEnv.GetStandardViewer())
   {
      Apply();
   }
}

const QFont& Map::PrefObject::GetLabelFont() const
{
   return mCurrentPrefs.mFont;
}

float Map::PrefObject::GetModelScale() const
{
   return mCurrentPrefs.mModelScale;
}

bool Map::PrefObject::GetShowTerrainAltitudeAtCursor() const
{
   return mCurrentPrefs.mShowTerrainAltitudeAtCursor;
}

bool Map::PrefObject::GetZoomOnCursor() const
{
   return mCurrentPrefs.mZoomOnCursor;
}
