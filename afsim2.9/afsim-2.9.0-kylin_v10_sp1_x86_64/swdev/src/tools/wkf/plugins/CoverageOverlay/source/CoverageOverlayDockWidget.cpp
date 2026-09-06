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

#include "CoverageOverlayDockWidget.hpp"

#include <QDoubleValidator>

#include "UtCast.hpp"
#include "UtQtGradientMap.hpp"
#include "UtUnits.hpp"
#include "WkfEnvironment.hpp"
#include "WkfUnitsObject.hpp"

namespace CoverageOverlay
{

CoverageOverlayDockWidget::CoverageOverlayDockWidget(const UtQtGradientMap& aGradientMap,
                                                     QWidget*               aParentPtr,
                                                     Qt::WindowFlags        aFlags)
   : QDockWidget(aParentPtr, aFlags)
   , mGradientMap(aGradientMap)
   , mCurrentUnitType(UtUnits::UnitType::_cUNIT_TYPE_COUNT)
{
   mUI.setupUi(this);
   mUI.mGradientLegend->SetVariableName(QString{""});

   connect(mUI.mLoadDataButton, &QPushButton::pressed, this, &CoverageOverlayDockWidget::OnLoadDataPressed);
   connect(mUI.mUnloadDataButton, &QPushButton::pressed, this, &CoverageOverlayDockWidget::OnUnloadDataPressed);
   connect(mUI.mLoadedDataList, &QListWidget::itemSelectionChanged, this, &CoverageOverlayDockWidget::OnCoverageDataSelected);
   connect(mUI.mLoadedDataList, &QListWidget::itemChanged, this, &CoverageOverlayDockWidget::OnItemChanged);
   connect(mUI.mLoadedDataList, &QListWidget::itemDoubleClicked, this, &CoverageOverlayDockWidget::OnItemDoubleClicked);
   connect(mUI.mFieldSelector, &QComboBox::currentTextChanged, this, &CoverageOverlayDockWidget::OnFieldSelection);
   connect(mUI.mOpacitySlider, &QSlider::valueChanged, this, &CoverageOverlayDockWidget::OnOpacityChanged);

   connect(mUI.mAutoScaleCheck, &QCheckBox::stateChanged, this, &CoverageOverlayDockWidget::OnAutoScaleStateChanged);

   QDoubleValidator* minValidator = new QDoubleValidator{this};
   mUI.mMinimumEdit->setValidator(minValidator);
   connect(mUI.mMinimumEdit, &QLineEdit::textEdited, this, &CoverageOverlayDockWidget::OnRangeChanged);

   QDoubleValidator* maxValidator = new QDoubleValidator{this};
   mUI.mMaximumEdit->setValidator(maxValidator);
   connect(mUI.mMaximumEdit, &QLineEdit::textEdited, this, &CoverageOverlayDockWidget::OnRangeChanged);

   connect(mUI.mGradientSelector, &QComboBox::currentTextChanged, this, &CoverageOverlayDockWidget::OnGradientSelection);

   ConnectToUnitsChanged();

   // Populate the gradient selector, and select the default gradient.
   auto grads = mGradientMap.Gradients();
   for (auto& iter : grads)
   {
      mUI.mGradientSelector->addItem(iter.first);
   }
   mUI.mGradientSelector->setCurrentText(UtQtGradientMap::cDEFAULT_GRADIENT_KEY);

   OnCoverageDataSelected();
}

void CoverageOverlayDockWidget::CoverageLoaded(const QString& aFileName)
{
   QListWidgetItem* itemPtr = new QListWidgetItem{aFileName};
   itemPtr->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   mUI.mLoadedDataList->addItem(itemPtr);
   itemPtr->setCheckState(Qt::Checked);
   if (GetSelectedCoverage().isEmpty())
   {
      mUI.mLoadedDataList->setCurrentItem(itemPtr);
   }
}

void CoverageOverlayDockWidget::CoverageUnloaded(const QString& aFileName)
{
   auto items = mUI.mLoadedDataList->findItems(aFileName, Qt::MatchExactly);
   for (auto itemPtr : items)
   {
      delete itemPtr;
   }
}

void CoverageOverlayDockWidget::ClearOptions()
{
   SetOptions(QString{},
              QStringList{},
              QString{},
              QString{},
              QString{},
              UtUnits::UnitType::_cUNIT_TYPE_COUNT,
              127,
              true,
              0.0f,
              1.0f,
              QString{"Default"});
}

void CoverageOverlayDockWidget::SetOptions(const QString&     aCoverageName,
                                           const QStringList& aStringList,
                                           const QString&     aFieldName,
                                           const QString&     aFieldType,
                                           const QString&     aFieldSubtype,
                                           int                aFieldUnitType,
                                           unsigned int       aOpacity,
                                           bool               aAutoScale,
                                           float              aMinValue,
                                           float              aMaxValue,
                                           const QString&     aGradientSelection)
{
   mUI.mCoverageDataGroup->setTitle(aCoverageName);
   mUI.mFieldSelector->clear();
   mUI.mFieldSelector->addItems(aStringList);
   mUI.mFieldSelector->setCurrentText(aFieldName);

   SetOptions(aFieldType, aFieldSubtype, aFieldUnitType, aOpacity, aAutoScale, aMinValue, aMaxValue, aGradientSelection);
}

void CoverageOverlayDockWidget::SetOptions(const QString& aFieldType,
                                           const QString& aFieldSubtype,
                                           int            aFieldUnitType,
                                           unsigned int   aOpacity,
                                           bool           aAutoScale,
                                           float          aMinValue,
                                           float          aMaxValue,
                                           const QString& aGradientSelection)
{
   mUI.mTypeValue->setText(aFieldType);

   mCurrentUnitType   = aFieldUnitType;
   QString unitSuffix = GetUnitSuffixString();
   mUI.mSubtypeValue->setText(aFieldSubtype + unitSuffix);
   mUI.mGradientLegend->SetVariableName(aFieldSubtype + unitSuffix);
   mUI.mOpacitySlider->setValue(aOpacity);

   mUI.mAutoScaleCheck->setChecked(aAutoScale);
   SetValueRange(aMinValue, aMaxValue);

   mUI.mGradientSelector->setCurrentText(aGradientSelection);
}

void CoverageOverlayDockWidget::SetValueRange(float aMinValue, float aMaxValue)
{
   float min  = GetConvertedValue(aMinValue);
   float max  = GetConvertedValue(aMaxValue);
   int   prec = GetUnitPrecision();
   mUI.mMinimumEdit->setText(QString::number(min));
   mUI.mMaximumEdit->setText(QString::number(max));
   OnRangeChanged();
   mUI.mGradientLegend->SetBoundsAndPrecision(min, max, prec);
}

QString CoverageOverlayDockWidget::GetSelectedCoverage() const
{
   if (mUI.mLoadedDataList->currentRow() != -1)
   {
      return mUI.mLoadedDataList->currentItem()->text();
   }
   return QString{""};
}

float CoverageOverlayDockWidget::GetRangeMinimum() const
{
   return mUI.mMinimumEdit->text().toFloat();
}

float CoverageOverlayDockWidget::GetRangeMaximum() const
{
   return mUI.mMaximumEdit->text().toFloat();
}

void CoverageOverlayDockWidget::OnLoadDataPressed()
{
   auto files =
      wkf::getOpenFileNames(this, "Select Coverage Overlay File(s)", "", "Coverage Overlay (*.cvg);; All Files (*.*)");
   if (files.size())
   {
      emit CoveragesSelectedForLoad(files);
   }
}

void CoverageOverlayDockWidget::OnUnloadDataPressed()
{
   auto items = mUI.mLoadedDataList->selectedItems();
   if (items.size())
   {
      emit CoverageSelectedForUnload(items.at(0)->text());
   }
}

void CoverageOverlayDockWidget::OnCoverageDataSelected()
{
   auto items  = mUI.mLoadedDataList->selectedItems();
   bool enable = items.size() == 1;
   if (enable)
   {
      emit CoverageSelected(items.at(0)->text());
   }
   else
   {
      emit CoverageSelected(QString{});
   }
   mUI.mUnloadDataButton->setEnabled(enable);
   mUI.mCoverageDataGroup->setEnabled(enable);
   mUI.mRangeGroup->setEnabled(enable);
   mUI.mDisplayGroup->setEnabled(enable);
}

void CoverageOverlayDockWidget::OnItemChanged(QListWidgetItem* aItemPtr)
{
   if (aItemPtr)
   {
      QString file    = aItemPtr->text();
      bool    checked = aItemPtr->checkState() == Qt::Checked;
      emit    CoverageVisibilityChanged(file, checked);
   }
}

void CoverageOverlayDockWidget::OnItemDoubleClicked(QListWidgetItem* aItemPtr)
{
   if (aItemPtr)
   {
      if (aItemPtr->checkState() == Qt::Checked)
      {
         emit RequestCenterOnCoverage(aItemPtr->text());
      }
   }
}

void CoverageOverlayDockWidget::OnFieldSelection(const QString& aFieldName)
{
   emit FieldChanged(GetSelectedCoverage(), aFieldName);
}

void CoverageOverlayDockWidget::OnOpacityChanged(int aValue)
{
   emit OpacityChanged(GetSelectedCoverage(), aValue);
}

void CoverageOverlayDockWidget::OnAutoScaleStateChanged(int aState)
{
   bool checked = aState == Qt::Checked;
   mUI.mMinimumEdit->setReadOnly(checked);
   mUI.mMaximumEdit->setReadOnly(checked);
   emit AutoScaleStateChanged(GetSelectedCoverage(), checked);
}

void CoverageOverlayDockWidget::OnRangeChanged()
{
   float min    = GetRangeMinimum();
   float max    = GetRangeMaximum();
   float stdMin = GetStandardValue(min);
   float stdMax = GetStandardValue(max);
   if (min < max)
   {
      mUI.mGradientLegend->SetDataBounds(min, max);
      emit RangeChanged(GetSelectedCoverage(), stdMin, stdMax);
   }
   else
   {
      mUI.mGradientLegend->SetDataBounds(max, min);
      emit RangeChanged(GetSelectedCoverage(), stdMax, stdMin);
   }
}

void CoverageOverlayDockWidget::OnGradientSelection(const QString& aGradientName)
{
   if (mGradientMap.Contains(aGradientName))
   {
      mUI.mGradientLegend->GradientBar()->SetGradientStops(mGradientMap.GradientStops(aGradientName));
      emit GradientSelected(GetSelectedCoverage(), aGradientName);
   }
}

void CoverageOverlayDockWidget::OnUnitsUpdate()
{
   // The dock widget does not store the needed values. So this submits a request
   // to the plugin to resend the values by emitting the FieldChanged signal.
   emit FieldChanged(GetSelectedCoverage(), mUI.mFieldSelector->currentText());
}

void CoverageOverlayDockWidget::ConnectToUnitsChanged()
{
   auto* prefsPtr = wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
   if (!prefsPtr)
   {
      return;
   }

   connect(prefsPtr,
           &wkf::UnitsObject::AccelerationUnitChanged,
           [this](UtUnitAcceleration::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::AngleUnitChanged, [this](UtUnitAngle::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr,
           &wkf::UnitsObject::AngularRateUnitChanged,
           [this](UtUnitAngularRate::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr,
           &wkf::UnitsObject::AngularAccelerationUnitChanged,
           [this](UtUnitAngularAcceleration::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::ForceUnitChanged, [this](UtUnitForce::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr,
           &wkf::UnitsObject::FrequencyUnitChanged,
           [this](UtUnitFrequency::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::LengthUnitChanged, [this](UtUnitLength::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::MassUnitChanged, [this](UtUnitMass::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr,
           &wkf::UnitsObject::MassTransferUnitChanged,
           [this](UtUnitMassTransfer::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::PowerUnitChanged, [this](UtUnitPower::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::PowerDBUnitChanged, [this](UtUnitPowerDB::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::PressureUnitChanged, [this](UtUnitPressure::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::RatioUnitChanged, [this](UtUnitRatio::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::SpeedUnitChanged, [this](UtUnitSpeed::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::TimeUnitChanged, [this](UtUnitTime::BaseUnit aUnit) { OnUnitsUpdate(); });
   connect(prefsPtr, &wkf::UnitsObject::TorqueUnitChanged, [this](UtUnitTorque::BaseUnit aUnit) { OnUnitsUpdate(); });
}

QString CoverageOverlayDockWidget::GetUnitSuffixString() const
{
   auto* prefsPtr = wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
   if (!prefsPtr)
   {
      return QString{};
   }
   auto* unitTypePtr = UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(mCurrentUnitType));
   if (!unitTypePtr)
   {
      return QString{};
   }

   QString retval{};
   switch (mCurrentUnitType)
   {
   case UtUnits::cACCELERATION:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetAccelerationUnit())));
      break;
   case UtUnits::cANGLE:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetAngleUnit())));
      break;
   case UtUnits::cANGULAR_RATE:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetAngularRateUnit())));
      break;
   case UtUnits::cANGULAR_ACCELERATION:
      retval =
         QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetAngularAccelerationUnit())));
      break;
   case UtUnits::cFORCE:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetForceUnit())));
      break;
   case UtUnits::cFREQUENCY:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetFrequencyUnit())));
      break;
   case UtUnits::cLENGTH:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetLengthUnit())));
      break;
   case UtUnits::cMASS:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetMassUnit())));
      break;
   case UtUnits::cMASS_TRANSFER:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetMassTransferUnit())));
      break;
   case UtUnits::cPOWER:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetPowerUnit())));
      break;
   case UtUnits::cPOWER_DB:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetPowerDBUnit())));
      break;
   case UtUnits::cPRESSURE:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetPressureUnit())));
      break;
   case UtUnits::cRATIO:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetRatioUnit())));
      break;
   case UtUnits::cSPEED:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetSpeedUnit())));
      break;
   case UtUnits::cTIME:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetTimeUnit())));
      break;
   case UtUnits::cTORQUE:
      retval = QString{" [%1]"}.arg(QString::fromStdString(unitTypePtr->FindUnitName(prefsPtr->GetTorqueUnit())));
      break;
   default:
      // Nothing here.
      break;
   }
   return retval;
}

double CoverageOverlayDockWidget::GetConvertedValue(double aValue) const
{
   auto* prefsPtr = wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
   if (!prefsPtr)
   {
      return aValue;
   }
   auto* unitTypePtr = UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(mCurrentUnitType));
   if (!unitTypePtr)
   {
      return aValue;
   }

   double retval{aValue};
   switch (mCurrentUnitType)
   {
   case UtUnits::cACCELERATION:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetAccelerationUnit());
      break;
   case UtUnits::cANGLE:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetAngleUnit());
      break;
   case UtUnits::cANGULAR_RATE:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetAngularRateUnit());
      break;
   case UtUnits::cANGULAR_ACCELERATION:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetAngularAccelerationUnit());
      break;
   case UtUnits::cFORCE:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetForceUnit());
      break;
   case UtUnits::cFREQUENCY:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetFrequencyUnit());
      break;
   case UtUnits::cLENGTH:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetLengthUnit());
      break;
   case UtUnits::cMASS:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetMassUnit());
      break;
   case UtUnits::cMASS_TRANSFER:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetMassTransferUnit());
      break;
   case UtUnits::cPOWER:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetPowerUnit());
      break;
   case UtUnits::cPOWER_DB:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetPowerDBUnit());
      break;
   case UtUnits::cPRESSURE:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetPressureUnit());
      break;
   case UtUnits::cRATIO:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetRatioUnit());
      break;
   case UtUnits::cSPEED:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetSpeedUnit());
      break;
   case UtUnits::cTIME:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetTimeUnit());
      break;
   case UtUnits::cTORQUE:
      retval = unitTypePtr->ConvertFromStandard(aValue, prefsPtr->GetTorqueUnit());
      break;
   default:
      // Nothing here.
      break;
   }
   return retval;
}

double CoverageOverlayDockWidget::GetStandardValue(double aValue) const
{
   auto* prefsPtr = wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
   if (!prefsPtr)
   {
      return aValue;
   }
   auto* unitTypePtr = UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(mCurrentUnitType));
   if (!unitTypePtr)
   {
      return aValue;
   }

   double retval{aValue};
   switch (mCurrentUnitType)
   {
   case UtUnits::cACCELERATION:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetAccelerationUnit());
      break;
   case UtUnits::cANGLE:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetAngleUnit());
      break;
   case UtUnits::cANGULAR_RATE:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetAngularRateUnit());
      break;
   case UtUnits::cANGULAR_ACCELERATION:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetAngularAccelerationUnit());
      break;
   case UtUnits::cFORCE:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetForceUnit());
      break;
   case UtUnits::cFREQUENCY:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetFrequencyUnit());
      break;
   case UtUnits::cLENGTH:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetLengthUnit());
      break;
   case UtUnits::cMASS:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetMassUnit());
      break;
   case UtUnits::cMASS_TRANSFER:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetMassTransferUnit());
      break;
   case UtUnits::cPOWER:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetPowerUnit());
      break;
   case UtUnits::cPOWER_DB:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetPowerDBUnit());
      break;
   case UtUnits::cPRESSURE:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetPressureUnit());
      break;
   case UtUnits::cRATIO:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetRatioUnit());
      break;
   case UtUnits::cSPEED:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetSpeedUnit());
      break;
   case UtUnits::cTIME:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetTimeUnit());
      break;
   case UtUnits::cTORQUE:
      retval = unitTypePtr->ConvertToStandard(aValue, prefsPtr->GetTorqueUnit());
      break;
   default:
      // Nothing here.
      break;
   }
   return retval;
}

int CoverageOverlayDockWidget::GetUnitPrecision() const
{
   auto* prefsPtr = wkfEnv.GetPreferenceObject<wkf::UnitsObject>();
   if (!prefsPtr)
   {
      return 6;
   }
   auto* unitTypePtr = UtUnits::GetUnitType(static_cast<UtUnits::UnitType>(mCurrentUnitType));
   if (!unitTypePtr)
   {
      return 6;
   }

   double retval{6};
   switch (mCurrentUnitType)
   {
   case UtUnits::cACCELERATION:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cACCELERATION));
      break;
   case UtUnits::cANGLE:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cANGLE));
      break;
   case UtUnits::cANGULAR_RATE:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cANGULAR_RATE));
      break;
   case UtUnits::cANGULAR_ACCELERATION:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cANGULAR_ACCELERATION));
      break;
   case UtUnits::cFORCE:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cFORCE));
      break;
   case UtUnits::cFREQUENCY:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cFREQUENCY));
      break;
   case UtUnits::cLENGTH:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cLENGTH));
      break;
   case UtUnits::cMASS:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cMASS));
      break;
   case UtUnits::cMASS_TRANSFER:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cMASS_TRANSFER));
      break;
   case UtUnits::cPOWER:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cPOWER));
      break;
   case UtUnits::cPOWER_DB:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cPOWER_DB));
      break;
   case UtUnits::cPRESSURE:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cPRESSURE));
      break;
   case UtUnits::cRATIO:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cRATIO));
      break;
   case UtUnits::cSPEED:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cSPEED));
      break;
   case UtUnits::cTIME:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cTIME));
      break;
   case UtUnits::cTORQUE:
      retval = ut::cast_to_int(prefsPtr->GetUnitPrecision(wkf::ValueType::cTORQUE));
      break;
   default:
      // Nothing here.
      break;
   }
   return retval;
}

} // namespace CoverageOverlay
