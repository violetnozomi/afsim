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
#ifndef MAPPREFOBJECT_HPP
#define MAPPREFOBJECT_HPP

#include <set>

#include <QColor>

#include "VaCallbackHolder.hpp"
#include "WkfPrefObject.hpp"

namespace vespa
{
class VaOverlayCompass;
class VaOverlayElevationLines;
class VaOverlayMapGrid;
class VaOverlayMapRings;
class VaOverlayScale;
class VaOverlayShapeFile;
class VaOverlayWdb;
class VaViewer;
} // namespace vespa

namespace wkf
{
class OverlaySpace;
}

namespace Map
{
class OverlayScale;

struct PrefData
{
   enum AstroIconType
   {
      DEFAULT,
      CUSTOM
   };

   std::string mActiveMap{"Blue Marble"};

   float        mIntensity{0.f};
   float        mSaturation{0.f};
   float        mContrast{0.f};
   bool         mGridLines{true};
   QColor       mGridColor{170, 170, 170, 255};
   float        mGridDensity{1.0f};
   bool         mElevationLines{false};
   int          mElevationSpacing{400};
   int          mElevationMin{-413};
   int          mElevationMax{8850};
   QString      mElevationSource{};
   bool         mGlobalRangeRings{false};
   QColor       mRingColor{0, 0, 0, 255};
   bool         mCoastLines{true};
   QColor       mCoastColor{37, 224, 255, 255};
   bool         mCountryBorders{true};
   QColor       mCountryColor{64, 192, 64, 255};
   bool         mInternalBorders{true};
   QColor       mInternalColor{48, 144, 48, 255};
   bool         mLakesAndRivers{true};
   QColor       mLakesColor{37, 224, 255, 255};
   bool         mScale{false};
   bool         mCompass{false};
   bool         mTeamColor{true};
   bool         mTrueScale{false};
   float        mModelScale{1.0f};
   bool         mLighting{false};
   bool         mECI{false};
   bool         mCenterOnStart{true};
   QFont        mFont;
   bool         mShowTerrainAltitudeAtCursor{true};
   bool         mZoomOnCursor{true};
   bool         mTerminator{false};
   QColor       mTerminatorColor{200, 200, 0, 255};
   unsigned int mTerminatorWidth{2};
   bool         mSolarPath{false};
   QColor       mSolarPathColor{255, 255, 128, 255};
   unsigned int mSolarPathWidth{2};
   double       mSolarUpdatePeriod{60.0};

   bool          mSubSolarPoint{false};
   QColor        mSubSolarColor{255, 255, 0, 255};
   double        mSubSolarScale{0.6};
   QString       mSubSolarIconPath;
   AstroIconType mSubSolarIconType = DEFAULT;
   bool          mSubLunarPoint{false};
   QColor        mSubLunarColor{192, 192, 192, 255};
   double        mSubLunarScale{0.6};
   QString       mSubLunarIconPath;
   AstroIconType mSubLunarIconType = DEFAULT;
};

class PrefObject : public wkf::PrefObjectT<PrefData>
{
public:
   static constexpr const char* cNAME = "MapPrefObject";
   PrefObject(QObject* parent = nullptr);
   ~PrefObject() override = default;

   const QFont& GetLabelFont() const;
   float        GetModelScale() const;
   bool         GetShowTerrainAltitudeAtCursor() const;
   bool         GetZoomOnCursor() const;

private:
   void Apply() override;

   void     ApplySharedMaps();
   PrefData ReadSettings(QSettings& aSettings) const override;
   void     SaveSettingsP(QSettings& aSettings) const override;
   void     ViewerInitializedCB(vespa::VaViewer* aViewerPtr);

   std::string mActiveMaps;

   vespa::VaOverlayCompass*        mCompassPtr{nullptr};
   vespa::VaOverlayMapGrid*        mMapGridPtr{nullptr};
   vespa::VaOverlayElevationLines* mMapElevationLinesPtr{nullptr};
   vespa::VaOverlayMapRings*       mMapRingsPtr{nullptr};
   OverlayScale*                   mScalePtr{nullptr};
   vespa::VaOverlayShapeFile*      mCountryPtr{nullptr};
   vespa::VaOverlayShapeFile*      mInternalPtr{nullptr};
   vespa::VaOverlayShapeFile*      mCoastPtr{nullptr};
   vespa::VaOverlayWdb*            mWdbPtr{nullptr};
   vespa::VaCallbackHolder         mCallbacks;
   wkf::OverlaySpace*              mSpaceOverlayPtr{nullptr};
};
} // namespace Map

Q_DECLARE_METATYPE(Map::PrefData)
#endif
