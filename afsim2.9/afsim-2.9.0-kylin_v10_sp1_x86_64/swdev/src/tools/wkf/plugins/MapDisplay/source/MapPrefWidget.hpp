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

#ifndef MAPPREFWIDGET_HPP
#define MAPPREFWIDGET_HPP

class QComboBox;
class QSlider;
class QTreeWidgetItem;

#include <QRadioButton>

#include "MapPrefObject.hpp"
#include "UtQtFontButton.hpp"
#include "WkfPrefWidget.hpp"
#include "selectors/WkfImageSelectorWidget.hpp"
#include "ui_MapPrefWidget.h"

namespace Map
{
class PrefWidget : public wkf::PrefWidgetT<PrefObject>
{
   Q_OBJECT

public:
   PrefWidget();
   ~PrefWidget() override = default;

   QString GetCategoryHint() const override { return "Map"; }
   bool    GetCategoryHintPriority() const override { return true; }

private:
   void SharedMapsChanged();

   enum UserRole
   {
      cMAPLIST = 1,
      cMAP     = 2,
      cMAPFILE = 3
   };
   QColor GetColor(QTreeWidgetItem* aItem);
   void   SetColor(QTreeWidgetItem* aItem, const QColor& aColor);

   int  GetValue(QTreeWidgetItem* aItem);
   void SetValue(QTreeWidgetItem* aItem, const int aValue, const int aMin, const int aMax);

   QString GetFilename(QTreeWidgetItem* aItem);
   void    SetFilename(QTreeWidgetItem* aItem, const QString& aFilename);
   void    SelectFile(QTreeWidgetItem* aItem, const QString& aCaption, const QString& aFilter);

   void ReadPreferenceData(const PrefData& aPrefData) override;
   void WritePreferenceData(PrefData& aPrefData) override;

   void ChooseLabelFont();

   int mDefaultTerrainMapIndex;
   int mDefaultChartIndex;

   Ui::MapPrefWidget          mUi;
   QTreeWidgetItem*           mMap;
   QComboBox*                 mMapChooser;
   QTreeWidgetItem*           mGrid;
   QSlider*                   mGridDensity;
   QTreeWidgetItem*           mElevationLines;
   QTreeWidgetItem*           mElevationSpacing;
   QTreeWidgetItem*           mElevationMin;
   QTreeWidgetItem*           mElevationMax;
   QTreeWidgetItem*           mElevationSource;
   QTreeWidgetItem*           mCoast;
   QTreeWidgetItem*           mCountry;
   QTreeWidgetItem*           mInternal;
   QTreeWidgetItem*           mLakes;
   QTreeWidgetItem*           mRings;
   QTreeWidgetItem*           mScale;
   QTreeWidgetItem*           mCompass;
   QTreeWidgetItem*           mTerminator;
   QSlider*                   mTerminatorWidthSlider;
   QTreeWidgetItem*           mSubSolarPoint;
   QSlider*                   mSubSolarScale;
   QRadioButton*              mDefaultSunIcon;
   QRadioButton*              mCustomSunIcon;
   wkf::ImageSelectionWidget* mSunImageSelector;
   QTreeWidgetItem*           mSubLunarPoint;
   QSlider*                   mSubLunarScale;
   QRadioButton*              mDefaultMoonIcon;
   QRadioButton*              mCustomMoonIcon;
   wkf::ImageSelectionWidget* mMoonImageSelector;
   QTreeWidgetItem*           mSolarPath;
   QSlider*                   mSolarPathWidthSlider;
   QTreeWidgetItem*           mIntensity;
   QSlider*                   mIntensitySlider;
   QTreeWidgetItem*           mSaturation;
   QSlider*                   mSaturationSlider;
   QTreeWidgetItem*           mContrast;
   QSlider*                   mContrastSlider;
   QTreeWidgetItem*           mTeamColor;
   QTreeWidgetItem*           mTrueScale;
   QSlider*                   mModelScaleSlider;
   UtQtFontButton*            mLabelButton;
   QTreeWidgetItem*           mLighting;
   QTreeWidgetItem*           mECI;
   QTreeWidgetItem*           mCenterOnStart;
   QTreeWidgetItem*           mShowAltitudeAtCursor;
   QTreeWidgetItem*           mZoomOnCursor;
};
} // namespace Map
#endif
