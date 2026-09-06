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
#include "MapPrefWidget.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFont>
#include <QFormLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QTreeWidgetItem>

#include "MapPrefObject.hpp"
#include "UtQtColorButton.hpp"
#include "UtQtFontButton.hpp"
#include "UtQtMemory.hpp"
#include "WkfEnvironment.hpp"
#include "WkfSharedMapPreferencesObject.hpp"

Map::PrefWidget::PrefWidget()
   : wkf::PrefWidgetT<PrefObject>()
   , mDefaultTerrainMapIndex(0)
   , mDefaultChartIndex(0)
{
   connect(wkfEnv.GetPreferenceObject<wkf::SharedMapPreferencesObject>(),
           &wkf::SharedMapPreferencesObject::SharedMapsChanged,
           this,
           &PrefWidget::SharedMapsChanged);

   mUi.setupUi(this);

   mUi.vectorTree->setSortingEnabled(false);

   mUi.vectorTree->setColumnCount(2);
   mUi.vectorTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
   mUi.vectorTree->clear();

   mMap        = new QTreeWidgetItem(mUi.vectorTree->invisibleRootItem(), QStringList("Map"));
   mMapChooser = new QComboBox(this);
   mUi.vectorTree->setItemWidget(mMap, 1, mMapChooser);

   QTreeWidgetItem* mapSettings = new QTreeWidgetItem(mUi.vectorTree->invisibleRootItem(), QStringList("Map Options"));
   mIntensity                   = new QTreeWidgetItem(mapSettings, QStringList("Brightness"));
   mIntensitySlider             = new QSlider(Qt::Horizontal, this);
   mIntensitySlider->setMinimum(-50);
   mIntensitySlider->setMaximum(50);
   mIntensitySlider->setValue(0);
   mUi.vectorTree->setItemWidget(mIntensity, 1, mIntensitySlider);

   mContrast       = new QTreeWidgetItem(mapSettings, QStringList("Contrast"));
   mContrastSlider = new QSlider(Qt::Horizontal, this);
   mContrastSlider->setMinimum(-50);
   mContrastSlider->setMaximum(100);
   mContrastSlider->setValue(50);
   mUi.vectorTree->setItemWidget(mContrast, 1, mContrastSlider);

   mSaturation       = new QTreeWidgetItem(mapSettings, QStringList("Saturation"));
   mSaturationSlider = new QSlider(Qt::Horizontal, this);
   mSaturationSlider->setMinimum(-50);
   mSaturationSlider->setMaximum(150);
   mSaturationSlider->setValue(0);
   mUi.vectorTree->setItemWidget(mSaturation, 1, mSaturationSlider);

   mapSettings->setExpanded(true);

   QTreeWidgetItem* modelOptions = new QTreeWidgetItem(mUi.vectorTree->invisibleRootItem(), QStringList("Model Options"));
   mTeamColor                    = new QTreeWidgetItem(modelOptions, QStringList("Display Team Color"));
   mTeamColor->setCheckState(0, Qt::Unchecked);
   mTrueScale = new QTreeWidgetItem(modelOptions, QStringList("True Scale"));
   mTrueScale->setCheckState(0, Qt::Unchecked);
   modelOptions->setExpanded(true);
   QTreeWidgetItem* modelScale = new QTreeWidgetItem(modelOptions, QStringList("Model Scale"));
   mModelScaleSlider           = new QSlider(Qt::Horizontal, this);
   mModelScaleSlider->setMinimum(50);
   mModelScaleSlider->setMaximum(500);
   mModelScaleSlider->setValue(100);
   mUi.vectorTree->setItemWidget(modelScale, 1, mModelScaleSlider);
   QTreeWidgetItem* labelSize = new QTreeWidgetItem(modelOptions, QStringList("Label Font"));
   mLabelButton               = new UtQtFontButton(this);
   mLabelButton->SetWindowTitle("Choose Platform Label Font");
   mUi.vectorTree->setItemWidget(labelSize, 1, mLabelButton);

   QTreeWidgetItem* overlays = new QTreeWidgetItem(mUi.vectorTree->invisibleRootItem(), QStringList("Overlays"));
   mGrid                     = new QTreeWidgetItem(overlays, QStringList("Grid Lines"));
   mGrid->setCheckState(0, Qt::Unchecked);
   SetColor(mGrid, Qt::red);
   QTreeWidgetItem* gridDensity = new QTreeWidgetItem(mGrid, QStringList("Density"));
   mGridDensity                 = new QSlider(Qt::Horizontal, this);
   mGridDensity->setMinimum(-3);
   mGridDensity->setMaximum(3);
   mGridDensity->setValue(0);
   mUi.vectorTree->setItemWidget(gridDensity, 1, mGridDensity);

   mElevationLines = new QTreeWidgetItem(overlays, QStringList("Elevation Lines"));
   mElevationLines->setCheckState(0, Qt::Unchecked);

   mElevationSpacing = new QTreeWidgetItem(mElevationLines, QStringList("Spacing (m)"));
   SetValue(mElevationSpacing, 400, 10, 1000);
   mElevationMin = new QTreeWidgetItem(mElevationLines, QStringList("Min (m)"));
   SetValue(mElevationMin, -413, -413, 8850);
   mElevationMax = new QTreeWidgetItem(mElevationLines, QStringList("Max (m)"));
   SetValue(mElevationMax, 8850, -413, 8850);
   mElevationSource = new QTreeWidgetItem(mElevationLines, QStringList("Source"));
   SetFilename(mElevationSource, "");

   mCoast = new QTreeWidgetItem(overlays, QStringList("Coast Lines"));
   mCoast->setCheckState(0, Qt::Unchecked);
   SetColor(mCoast, Qt::red);
   mCountry = new QTreeWidgetItem(overlays, QStringList("Country Borders"));
   mCountry->setCheckState(0, Qt::Unchecked);
   SetColor(mCountry, Qt::red);
   mInternal = new QTreeWidgetItem(overlays, QStringList("US Internal Borders"));
   mInternal->setCheckState(0, Qt::Unchecked);
   SetColor(mInternal, Qt::red);
   mLakes = new QTreeWidgetItem(overlays, QStringList("Lakes and Rivers"));
   mLakes->setCheckState(0, Qt::Unchecked);
   SetColor(mLakes, Qt::red);
   mRings = new QTreeWidgetItem(overlays, QStringList("Screen-Centered Range Rings"));
   mRings->setCheckState(0, Qt::Unchecked);
   SetColor(mRings, Qt::red);
   mTerminator = new QTreeWidgetItem(overlays, QStringList("Terminator"));
   mTerminator->setCheckState(0, Qt::Unchecked);
   SetColor(mTerminator, Qt::yellow);
   QTreeWidgetItem* terminatorWidth = new QTreeWidgetItem(mTerminator, QStringList("Line Width"));
   mTerminatorWidthSlider           = ut::qt::make_qt_ptr<QSlider>(Qt::Horizontal, this);
   mTerminatorWidthSlider->setMinimum(1);
   mTerminatorWidthSlider->setMaximum(5);
   mTerminatorWidthSlider->setValue(2);
   mUi.vectorTree->setItemWidget(terminatorWidth, 1, mTerminatorWidthSlider);

   auto PopulateCelestialOptions = [&](QTreeWidgetItem*&           aPoint,
                                       QSlider*&                   aScale,
                                       QRadioButton*&              aDefaultIcon,
                                       QRadioButton*&              aCustomIcon,
                                       wkf::ImageSelectionWidget*& aImageSelector,
                                       const QString&              aName)
   {
      aPoint = new QTreeWidgetItem(overlays, QStringList(aName));
      aPoint->setCheckState(0, Qt::Unchecked);
      SetColor(aPoint, Qt::gray);
      QTreeWidgetItem* scale = new QTreeWidgetItem(aPoint, QStringList("Scale"));
      aScale                 = ut::qt::make_qt_ptr<QSlider>(Qt::Horizontal, this);
      aScale->setMinimum(1);
      aScale->setMaximum(20);
      aScale->setValue(10);
      mUi.vectorTree->setItemWidget(scale, 1, aScale);
      QTreeWidgetItem* radioItem  = new QTreeWidgetItem();
      QFrame*          groupFrame = ut::qt::make_qt_ptr<QFrame>(mUi.vectorTree);
      aDefaultIcon                = ut::qt::make_qt_ptr<QRadioButton>("Default Icon", groupFrame);
      aDefaultIcon->setChecked(true);
      aCustomIcon    = ut::qt::make_qt_ptr<QRadioButton>("Custom Icon", groupFrame);
      aImageSelector = ut::qt::make_qt_ptr<wkf::ImageSelectionWidget>(groupFrame);
      aImageSelector->setEnabled(false);
      connect(aCustomIcon, &QRadioButton::toggled, aImageSelector, &wkf::ImageSelectionWidget::SetEnabledUpdate);
      QVBoxLayout* verticalBox = new QVBoxLayout;
      verticalBox->setContentsMargins(0, 0, 0, 0);
      verticalBox->addWidget(aDefaultIcon);
      verticalBox->addWidget(aCustomIcon);
      verticalBox->addWidget(aImageSelector);
      groupFrame->setLayout(verticalBox);
      aPoint->addChild(radioItem);
      mUi.vectorTree->setItemWidget(radioItem, 0, groupFrame);
      mUi.vectorTree->setFirstItemColumnSpanned(radioItem, true);
   };

   PopulateCelestialOptions(mSubSolarPoint, mSubSolarScale, mDefaultSunIcon, mCustomSunIcon, mSunImageSelector, "Sub-Solar Point");
   PopulateCelestialOptions(mSubLunarPoint, mSubLunarScale, mDefaultMoonIcon, mCustomMoonIcon, mMoonImageSelector, "Sub-Lunar Point");

   mSolarPath = new QTreeWidgetItem(overlays, QStringList("Solar Path"));
   mSolarPath->setCheckState(0, Qt::Unchecked);
   SetColor(mSolarPath, Qt::yellow);
   QTreeWidgetItem* solarPathWidth = new QTreeWidgetItem(mSolarPath, QStringList("Line Width"));
   mSolarPathWidthSlider           = new QSlider(Qt::Horizontal, this);
   mSolarPathWidthSlider->setMinimum(1);
   mSolarPathWidthSlider->setMaximum(5);
   mSolarPathWidthSlider->setValue(2);
   mUi.vectorTree->setItemWidget(solarPathWidth, 1, mSolarPathWidthSlider);

   mScale = new QTreeWidgetItem(overlays, QStringList("Scale"));
   mScale->setCheckState(0, Qt::Unchecked);
   mCompass = new QTreeWidgetItem(overlays, QStringList("Compass"));
   mCompass->setCheckState(0, Qt::Unchecked);
   mLighting = new QTreeWidgetItem(mapSettings, QStringList("Lighting"));
   mLighting->setCheckState(0, Qt::Unchecked);
   mECI = new QTreeWidgetItem(mapSettings, QStringList("ECI Camera"));
   mECI->setCheckState(0, Qt::Unchecked);
   mCenterOnStart = new QTreeWidgetItem(mapSettings, QStringList("Center On Start"));
   mCenterOnStart->setCheckState(0, Qt::Unchecked);
   mShowAltitudeAtCursor = new QTreeWidgetItem(mapSettings, QStringList("Show Terrain Altitude At Cursor"));
   mShowAltitudeAtCursor->setCheckState(0, Qt::Unchecked);
   mZoomOnCursor = new QTreeWidgetItem(mapSettings, QStringList("Zoom on Mouse Cursor"));
   mZoomOnCursor->setCheckState(0, Qt::Checked);
   overlays->setExpanded(true);
}

void Map::PrefWidget::WritePreferenceData(PrefData& aPrefData)
{
   aPrefData.mIntensity        = static_cast<float>(mIntensitySlider->value()) / 50.0f;
   aPrefData.mSaturation       = static_cast<float>(mSaturationSlider->value()) / 50.0f;
   aPrefData.mContrast         = static_cast<float>(mContrastSlider->value()) / 50.0f;
   aPrefData.mGridLines        = (mGrid->checkState(0) == Qt::Checked);
   aPrefData.mGridColor        = GetColor(mGrid);
   aPrefData.mGridDensity      = pow(2, mGridDensity->value());
   aPrefData.mElevationLines   = (mElevationLines->checkState(0) == Qt::Checked);
   aPrefData.mElevationSpacing = GetValue(mElevationSpacing);
   aPrefData.mElevationMin     = GetValue(mElevationMin);
   aPrefData.mElevationMax     = GetValue(mElevationMax);
   aPrefData.mElevationSource  = GetFilename(mElevationSource);
   aPrefData.mCoastLines       = (mCoast->checkState(0) == Qt::Checked);
   aPrefData.mCoastColor       = GetColor(mCoast);
   aPrefData.mCountryBorders   = (mCountry->checkState(0) == Qt::Checked);
   aPrefData.mCountryColor     = GetColor(mCountry);
   aPrefData.mInternalBorders  = (mInternal->checkState(0) == Qt::Checked);
   aPrefData.mInternalColor    = GetColor(mInternal);
   aPrefData.mLakesAndRivers   = (mLakes->checkState(0) == Qt::Checked);
   aPrefData.mLakesColor       = GetColor(mLakes);
   aPrefData.mGlobalRangeRings = (mRings->checkState(0) == Qt::Checked);
   aPrefData.mRingColor        = GetColor(mRings);
   aPrefData.mTerminator       = (mTerminator->checkState(0) == Qt::Checked);
   aPrefData.mTerminatorColor  = GetColor(mTerminator);
   aPrefData.mTerminatorWidth  = mTerminatorWidthSlider->value();

   aPrefData.mSubSolarPoint = (mSubSolarPoint->checkState(0) == Qt::Checked);
   aPrefData.mSubSolarColor = GetColor(mSubSolarPoint);
   aPrefData.mSubSolarScale = mSubSolarScale->value() * 0.1;
   if ((!mDefaultSunIcon->isChecked()) && (mSunImageSelector->IsValid()))
   {
      aPrefData.mSubSolarIconType = PrefData::CUSTOM;
      aPrefData.mSubSolarIconPath = mSunImageSelector->GetText();
   }
   else
   {
      aPrefData.mSubSolarIconType = PrefData::DEFAULT;
      aPrefData.mSubSolarIconPath = "";
   }
   aPrefData.mSubLunarPoint = (mSubLunarPoint->checkState(0) == Qt::Checked);
   aPrefData.mSubLunarColor = GetColor(mSubLunarPoint);
   aPrefData.mSubLunarScale = mSubLunarScale->value() * 0.1;
   if ((!mDefaultMoonIcon->isChecked()) && (mMoonImageSelector->IsValid()))
   {
      aPrefData.mSubLunarIconType = PrefData::CUSTOM;
      aPrefData.mSubLunarIconPath = mMoonImageSelector->GetText();
   }
   else
   {
      aPrefData.mSubLunarIconType = PrefData::DEFAULT;
      aPrefData.mSubLunarIconPath = "";
   }

   aPrefData.mSolarPath                   = (mSolarPath->checkState(0) == Qt::Checked);
   aPrefData.mSolarPathColor              = GetColor(mSolarPath);
   aPrefData.mSolarPathWidth              = mSolarPathWidthSlider->value();
   aPrefData.mScale                       = (mScale->checkState(0) == Qt::Checked);
   aPrefData.mCompass                     = (mCompass->checkState(0) == Qt::Checked);
   aPrefData.mActiveMap                   = mMapChooser->currentText().toStdString();
   aPrefData.mTeamColor                   = (mTeamColor->checkState(0) == Qt::Checked);
   aPrefData.mTrueScale                   = (mTrueScale->checkState(0) == Qt::Checked);
   aPrefData.mModelScale                  = (mModelScaleSlider->value() / 100.0f);
   aPrefData.mFont                        = mLabelButton->GetFont();
   aPrefData.mLighting                    = (mLighting->checkState(0) == Qt::Checked);
   aPrefData.mECI                         = (mECI->checkState(0) == Qt::Checked);
   aPrefData.mCenterOnStart               = (mCenterOnStart->checkState(0) == Qt::Checked);
   aPrefData.mShowTerrainAltitudeAtCursor = (mShowAltitudeAtCursor->checkState(0) == Qt::Checked);
   aPrefData.mZoomOnCursor                = (mZoomOnCursor->checkState(0) == Qt::Checked);
}

void Map::PrefWidget::ReadPreferenceData(const PrefData& aPrefData)
{
   // Indicate which map is currently selected...

   mIntensitySlider->setValue(aPrefData.mIntensity * 50.f);
   mSaturationSlider->setValue(aPrefData.mSaturation * 50.f);
   mContrastSlider->setValue(aPrefData.mContrast * 50.0f);

   mGrid->setCheckState(0, aPrefData.mGridLines ? Qt::Checked : Qt::Unchecked);
   SetColor(mGrid, aPrefData.mGridColor);
   mGridDensity->setValue(static_cast<int>(log2(aPrefData.mGridDensity)));
   mElevationLines->setCheckState(0, aPrefData.mElevationLines ? Qt::Checked : Qt::Unchecked);
   SetValue(mElevationSpacing, aPrefData.mElevationSpacing, 10, 1000);
   SetValue(mElevationMin, aPrefData.mElevationMin, -413, 8850);
   SetValue(mElevationMax, aPrefData.mElevationMax, -413, 8850);
   SetFilename(mElevationSource, aPrefData.mElevationSource);
   mRings->setCheckState(0, aPrefData.mGlobalRangeRings ? Qt::Checked : Qt::Unchecked);
   SetColor(mRings, aPrefData.mRingColor);
   mCoast->setCheckState(0, aPrefData.mCoastLines ? Qt::Checked : Qt::Unchecked);
   SetColor(mCoast, aPrefData.mCoastColor);
   mCountry->setCheckState(0, aPrefData.mCountryBorders ? Qt::Checked : Qt::Unchecked);
   SetColor(mCountry, aPrefData.mCountryColor);
   mInternal->setCheckState(0, aPrefData.mInternalBorders ? Qt::Checked : Qt::Unchecked);
   SetColor(mInternal, aPrefData.mInternalColor);
   mLakes->setCheckState(0, aPrefData.mLakesAndRivers ? Qt::Checked : Qt::Unchecked);
   SetColor(mLakes, aPrefData.mLakesColor);
   mRings->setCheckState(0, aPrefData.mGlobalRangeRings ? Qt::Checked : Qt::Unchecked);
   SetColor(mRings, aPrefData.mRingColor);
   mTerminator->setCheckState(0, aPrefData.mTerminator ? Qt::Checked : Qt::Unchecked);
   SetColor(mTerminator, aPrefData.mTerminatorColor);
   mTerminatorWidthSlider->setValue(aPrefData.mTerminatorWidth);
   mSubSolarPoint->setCheckState(0, aPrefData.mSubSolarPoint ? Qt::Checked : Qt::Unchecked);
   SetColor(mSubSolarPoint, aPrefData.mSubSolarColor);
   mSubSolarScale->setValue(aPrefData.mSubSolarScale * 10);
   if (aPrefData.mSubSolarIconType == PrefData::DEFAULT)
   {
      mDefaultSunIcon->setChecked(true);
      mSunImageSelector->SetText("");
   }
   else
   {
      mCustomSunIcon->setChecked(true);
      mSunImageSelector->SetText(aPrefData.mSubSolarIconPath);
   }
   mSubLunarPoint->setCheckState(0, aPrefData.mSubLunarPoint ? Qt::Checked : Qt::Unchecked);
   SetColor(mSubLunarPoint, aPrefData.mSubLunarColor);
   mSubLunarScale->setValue(aPrefData.mSubLunarScale * 10);
   if (aPrefData.mSubLunarIconType == PrefData::DEFAULT)
   {
      mDefaultMoonIcon->setChecked(true);
      mMoonImageSelector->SetText("");
   }
   else
   {
      mCustomMoonIcon->setChecked(true);
      mMoonImageSelector->SetText(aPrefData.mSubLunarIconPath);
   }
   mSolarPath->setCheckState(0, aPrefData.mSolarPath ? Qt::Checked : Qt::Unchecked);
   SetColor(mSolarPath, aPrefData.mSolarPathColor);
   mSolarPathWidthSlider->setValue(aPrefData.mSolarPathWidth);
   mScale->setCheckState(0, aPrefData.mScale ? Qt::Checked : Qt::Unchecked);
   mCompass->setCheckState(0, aPrefData.mCompass ? Qt::Checked : Qt::Unchecked);
   mLighting->setCheckState(0, aPrefData.mLighting ? Qt::Checked : Qt::Unchecked);
   mECI->setCheckState(0, aPrefData.mECI ? Qt::Checked : Qt::Unchecked);
   mCenterOnStart->setCheckState(0, aPrefData.mCenterOnStart ? Qt::Checked : Qt::Unchecked);
   mShowAltitudeAtCursor->setCheckState(0, aPrefData.mShowTerrainAltitudeAtCursor ? Qt::Checked : Qt::Unchecked);
   mZoomOnCursor->setCheckState(0, aPrefData.mZoomOnCursor ? Qt::Checked : Qt::Unchecked);

   // Enable or disable intensity and saturation sliders based on map selected

   mMapChooser->clear();
   for (auto& it : wkfEnv.GetPreferenceObject<wkf::SharedMapPreferencesObject>()->GetMaps())
   {
      QString name = it.first.c_str();
      mMapChooser->addItem(name);
   }
   mMapChooser->setCurrentText(aPrefData.mActiveMap.c_str());

   mTeamColor->setCheckState(0, aPrefData.mTeamColor ? Qt::Checked : Qt::Unchecked);
   mTrueScale->setCheckState(0, aPrefData.mTrueScale ? Qt::Checked : Qt::Unchecked);
   mModelScaleSlider->setValue(aPrefData.mModelScale * 100);
   mLabelButton->SetFont(aPrefData.mFont);
}

void Map::PrefWidget::SharedMapsChanged()
{
   QString ctext = mMapChooser->currentText();
   mMapChooser->clear();
   for (auto& it : wkfEnv.GetPreferenceObject<wkf::SharedMapPreferencesObject>()->GetMaps())
   {
      QString name = QString::fromStdString(it.first);
      mMapChooser->addItem(name);
   }
   mMapChooser->setCurrentText(ctext);
}

QColor Map::PrefWidget::GetColor(QTreeWidgetItem* aItem)
{
   UtQtColorButton* colorButton = dynamic_cast<UtQtColorButton*>(mUi.vectorTree->itemWidget(aItem, 1));
   if (colorButton)
   {
      return colorButton->GetColor();
   }
   else
   {
      return Qt::red;
   }
}

void Map::PrefWidget::SetColor(QTreeWidgetItem* aItem, const QColor& aColor)
{
   UtQtColorButton* colorButton = dynamic_cast<UtQtColorButton*>(mUi.vectorTree->itemWidget(aItem, 1));
   if (!colorButton)
   {
      colorButton = new UtQtColorButton(this);
      mUi.vectorTree->setItemWidget(aItem, 1, colorButton);
   }
   if (colorButton)
   {
      colorButton->SetColor(aColor);
   }
}

int Map::PrefWidget::GetValue(QTreeWidgetItem* aItem)
{
   QSpinBox* spinBox = dynamic_cast<QSpinBox*>(mUi.vectorTree->itemWidget(aItem, 1));
   if (spinBox)
   {
      return spinBox->value();
   }
   else
   {
      return 0;
   }
}

void Map::PrefWidget::SetValue(QTreeWidgetItem* aItem, const int aValue, const int aMin, const int aMax)
{
   QSpinBox* spinBox = dynamic_cast<QSpinBox*>(mUi.vectorTree->itemWidget(aItem, 1));
   if (!spinBox)
   {
      spinBox = new QSpinBox(this);
      mUi.vectorTree->setItemWidget(aItem, 1, spinBox);
   }
   if (spinBox)
   {
      spinBox->setValue(aValue);
      spinBox->setRange(aMin, aMax);
   }
}

void Map::PrefWidget::SetFilename(QTreeWidgetItem* aItem, const QString& aFilename)
{
   QPushButton* pb = dynamic_cast<QPushButton*>(mUi.vectorTree->itemWidget(aItem, 1));
   if (!pb)
   {
      pb = new QPushButton(this);
      mUi.vectorTree->setItemWidget(aItem, 1, pb);
      connect(pb, &QPushButton::clicked, [this, aItem]() { SelectFile(aItem, "Choose TMS Source", "tms.xml"); });
   }
   if (pb)
   {
      pb->setText(pb->fontMetrics().elidedText(aFilename, Qt::ElideMiddle, 140));
      aItem->setData(1, Qt::UserRole, aFilename);
   }
}

void Map::PrefWidget::SelectFile(QTreeWidgetItem* aItem, const QString& aCaption, const QString& aFilter)
{
   QString oldFilename = aItem->data(1, Qt::UserRole).toString();
   QString newFilename = wkf::getOpenFileName(this, aCaption, oldFilename, aFilter);

   if (!newFilename.isEmpty())
   {
      QPushButton* pb = dynamic_cast<QPushButton*>(mUi.vectorTree->itemWidget(aItem, 1));
      if (pb)
      {
         pb->setText(pb->fontMetrics().elidedText(newFilename, Qt::ElideMiddle, 140));
         aItem->setData(1, Qt::UserRole, newFilename);
      }
   }
}

QString Map::PrefWidget::GetFilename(QTreeWidgetItem* aItem)
{
   return aItem->data(1, Qt::UserRole).toString();
}
