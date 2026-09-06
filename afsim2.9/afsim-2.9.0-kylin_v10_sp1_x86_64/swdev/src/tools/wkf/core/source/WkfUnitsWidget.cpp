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
#include "WkfUnitsWidget.hpp"

#include "UtTime.hpp"
#include "UtUnitTypes.hpp"
#include "WkfUnitsObject.hpp"

wkf::UnitsWidget::UnitsWidget(QWidget* parent)
   : PrefWidgetT<UnitsObject>(parent)
{
   mUI.setupUi(this);

   std::map<int, std::string>::const_iterator iter;
   for (iter = UtUnitAcceleration::mUnitToString.begin(); iter != UtUnitAcceleration::mUnitToString.end(); ++iter)
   {
      mUI.accelerationComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitAngle::mUnitToString.begin(); iter != UtUnitAngle::mUnitToString.end(); ++iter)
   {
      mUI.angleComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitAngularRate::mUnitToString.begin(); iter != UtUnitAngularRate::mUnitToString.end(); ++iter)
   {
      mUI.angularRateComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitAngularAcceleration::mUnitToString.begin(); iter != UtUnitAngularAcceleration::mUnitToString.end();
        ++iter)
   {
      mUI.angularAccelerationComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitTime::mUnitToString.begin(); iter != UtUnitTime::mUnitToString.end(); ++iter)
   {
      mUI.durationComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitForce::mUnitToString.begin(); iter != UtUnitForce::mUnitToString.end(); ++iter)
   {
      mUI.forceComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitFrequency::mUnitToString.begin(); iter != UtUnitFrequency::mUnitToString.end(); ++iter)
   {
      mUI.frequencyComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitLength::mUnitToString.begin(); iter != UtUnitLength::mUnitToString.end(); ++iter)
   {
      mUI.highAltitudeComboBox->addItem(iter->second.c_str(), iter->first);
      mUI.lowAltitudeComboBox->addItem(iter->second.c_str(), iter->first);
      mUI.lengthComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitMass::mUnitToString.begin(); iter != UtUnitMass::mUnitToString.end(); ++iter)
   {
      mUI.massComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitMassTransfer::mUnitToString.begin(); iter != UtUnitMassTransfer::mUnitToString.end(); ++iter)
   {
      mUI.massTransferComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitPower::mUnitToString.begin(); iter != UtUnitPower::mUnitToString.end(); ++iter)
   {
      mUI.powerComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitPowerDB::mUnitToString.begin(); iter != UtUnitPowerDB::mUnitToString.end(); ++iter)
   {
      mUI.powerDBComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitPressure::mUnitToString.begin(); iter != UtUnitPressure::mUnitToString.end(); ++iter)
   {
      mUI.pressureComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitRatio::mUnitToString.begin(); iter != UtUnitRatio::mUnitToString.end(); ++iter)
   {
      mUI.ratioComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitSpeed::mUnitToString.begin(); iter != UtUnitSpeed::mUnitToString.end(); ++iter)
   {
      mUI.speedComboBox->addItem(iter->second.c_str(), iter->first);
   }

   for (iter = UtUnitTorque::mUnitToString.begin(); iter != UtUnitTorque::mUnitToString.end(); ++iter)
   {
      mUI.torqueComboBox->addItem(iter->second.c_str(), iter->first);
   }

   mUI.latLonComboBox->addItem("DD.ffffffff", UtAngle::FmtD + 8);
   mUI.latLonComboBox->addItem("DD.ffffff", UtAngle::FmtD + 6);
   mUI.latLonComboBox->addItem("DD:MM.ffffff", UtAngle::FmtDM + 6);
   mUI.latLonComboBox->addItem("DD:MM.ffff", UtAngle::FmtDM + 4);
   mUI.latLonComboBox->addItem("DD:MM:SS.ffff", UtAngle::FmtDMS + 4);
   mUI.latLonComboBox->addItem("DD:MM:SS.ff", UtAngle::FmtDMS + 2);
   mUI.latLonComboBox->addItem("DD:MM:SS", UtAngle::FmtDMS);

   mUI.timeComboBox->addItem("ss", UtTime::FmtS);
   mUI.timeComboBox->addItem("ss.ff", UtTime::FmtS + 2);
   mUI.timeComboBox->addItem("hh:mm:ss", UtTime::FmtHMS);
   mUI.timeComboBox->addItem("hh:mm:ss.ff", UtTime::FmtHMS + 2);
   mUI.timeComboBox->addItem("dd.ff", UtTime::FmtDays + 3);

   mUI.dateComboBox->addItem("ISO 8601", UnitsData::DATE_ISO);
   mUI.dateComboBox->addItem("MMM d yyyy hh:mm:ss", UnitsData::DATE_LONG);
   mUI.dateComboBox->addItem("MM/dd/yy hh:mm:ss", UnitsData::DATE_SHORT);

   // When a user changes the unit type, the precision should change to the default for that particular unit
   connect(mUI.accelerationComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.accelerationSpinBox->setValue(UnitsData::mAccelerationPrecisionMap.at(
                 (UtUnitAcceleration::BaseUnit)mUI.accelerationComboBox->currentData().toInt()));
           });
   connect(mUI.lowAltitudeComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.lowAltitudeSpinBox->setValue(UnitsData::mAltitudePrecisionMap.at(
                 (UtUnitLength::BaseUnit)mUI.lowAltitudeComboBox->currentData().toInt()));
           });
   connect(mUI.highAltitudeComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.highAltitudeSpinBox->setValue(UnitsData::mAltitudePrecisionMap.at(
                 (UtUnitLength::BaseUnit)mUI.highAltitudeComboBox->currentData().toInt()));
           });
   connect(mUI.angleComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.angleSpinBox->setValue(
                 UnitsData::mAnglePrecisionMap.at((UtUnitAngle::BaseUnit)mUI.angleComboBox->currentData().toInt()));
           });
   connect(mUI.angularRateComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.angularRateSpinBox->setValue(UnitsData::mAngularRatePrecisionMap.at(
                 (UtUnitAngularRate::BaseUnit)mUI.angularRateComboBox->currentData().toInt()));
           });
   connect(mUI.angularAccelerationComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.angularAccelerationSpinBox->setValue(UnitsData::mAngularAccelerationPrecisionMap.at(
                 (UtUnitAngularAcceleration::BaseUnit)mUI.angularAccelerationComboBox->currentData().toInt()));
           });
   connect(mUI.durationComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.durationSpinBox->setValue(
                 UnitsData::mTimePrecisionMap.at((UtUnitTime::BaseUnit)mUI.durationComboBox->currentData().toInt()));
           });
   connect(mUI.forceComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.forceSpinBox->setValue(
                 UnitsData::mForcePrecisionMap.at((UtUnitForce::BaseUnit)mUI.forceComboBox->currentData().toInt()));
           });
   connect(mUI.frequencyComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.frequencySpinBox->setValue(UnitsData::mFrequencyPrecisionMap.at(
                 (UtUnitFrequency::BaseUnit)mUI.frequencyComboBox->currentData().toInt()));
           });
   connect(mUI.lengthComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.lengthSpinBox->setValue(
                 UnitsData::mLengthPrecisionMap.at((UtUnitLength::BaseUnit)mUI.lengthComboBox->currentData().toInt()));
           });
   connect(mUI.massComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.massSpinBox->setValue(
                 UnitsData::mMassPrecisionMap.at((UtUnitMass::BaseUnit)mUI.massComboBox->currentData().toInt()));
           });
   connect(mUI.massTransferComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.massTransferSpinBox->setValue(UnitsData::mMassTransferPrecisionMap.at(
                 (UtUnitMassTransfer::BaseUnit)mUI.massTransferComboBox->currentData().toInt()));
           });
   connect(mUI.powerComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.powerSpinBox->setValue(
                 UnitsData::mPowerPrecisionMap.at((UtUnitPower::BaseUnit)mUI.powerComboBox->currentData().toInt()));
           });
   connect(mUI.powerDBComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.powerDBSpinBox->setValue(
                 UnitsData::mPowerDBPrecisionMap.at((UtUnitPowerDB::BaseUnit)mUI.powerDBComboBox->currentData().toInt()));
           });
   connect(mUI.pressureComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.pressureSpinBox->setValue(UnitsData::mPressurePrecisionMap.at(
                 (UtUnitPressure::BaseUnit)mUI.pressureComboBox->currentData().toInt()));
           });
   connect(mUI.ratioComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.ratioSpinBox->setValue(
                 UnitsData::mRatioPrecisionMap.at((UtUnitRatio::BaseUnit)mUI.ratioComboBox->currentData().toInt()));
           });
   connect(mUI.speedComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.speedSpinBox->setValue(
                 UnitsData::mSpeedPrecisionMap.at((UtUnitSpeed::BaseUnit)mUI.speedComboBox->currentData().toInt()));
           });
   connect(mUI.torqueComboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           this,
           [&](int aIndex)
           {
              mUI.torqueSpinBox->setValue(
                 UnitsData::mTorquePrecisionMap.at((UtUnitTorque::BaseUnit)mUI.torqueComboBox->currentData().toInt()));
           });
}

void wkf::UnitsWidget::WritePreferenceData(UnitsData& aPrefData)
{
   aPrefData.mAcceleration.mUnit = (UtUnitAcceleration::BaseUnit)mUI.accelerationComboBox->currentData().toInt();
   aPrefData.mHighAltitude.mUnit = (UtUnitLength::BaseUnit)mUI.highAltitudeComboBox->currentData().toInt();
   aPrefData.mLowAltitude.mUnit  = (UtUnitLength::BaseUnit)mUI.lowAltitudeComboBox->currentData().toInt();
   aPrefData.mAngle.mUnit        = (UtUnitAngle::BaseUnit)mUI.angleComboBox->currentData().toInt();
   aPrefData.mAngularRate.mUnit  = (UtUnitAngularRate::BaseUnit)mUI.angularRateComboBox->currentData().toInt();
   aPrefData.mAngularAcceleration.mUnit =
      (UtUnitAngularAcceleration::BaseUnit)mUI.angularAccelerationComboBox->currentData().toInt();
   aPrefData.mForce.mUnit        = (UtUnitForce::BaseUnit)mUI.forceComboBox->currentData().toInt();
   aPrefData.mFrequency.mUnit    = (UtUnitFrequency::BaseUnit)mUI.frequencyComboBox->currentData().toInt();
   aPrefData.mLatLonFormat       = (UtAngle::Fmt)mUI.latLonComboBox->currentData().toInt();
   aPrefData.mLength.mUnit       = (UtUnitLength::BaseUnit)mUI.lengthComboBox->currentData().toInt();
   aPrefData.mMass.mUnit         = (UtUnitMass::BaseUnit)mUI.massComboBox->currentData().toInt();
   aPrefData.mMassTransfer.mUnit = (UtUnitMassTransfer::BaseUnit)mUI.massTransferComboBox->currentData().toInt();
   aPrefData.mPower.mUnit        = (UtUnitPower::BaseUnit)mUI.powerComboBox->currentData().toInt();
   aPrefData.mPowerDB.mUnit      = (UtUnitPowerDB::BaseUnit)mUI.powerDBComboBox->currentData().toInt();
   aPrefData.mPressure.mUnit     = (UtUnitPressure::BaseUnit)mUI.pressureComboBox->currentData().toInt();
   aPrefData.mRatio.mUnit        = (UtUnitRatio::BaseUnit)mUI.ratioComboBox->currentData().toInt();
   aPrefData.mSpeed.mUnit        = (UtUnitSpeed::BaseUnit)mUI.speedComboBox->currentData().toInt();
   aPrefData.mTime.mUnit         = (UtUnitTime::BaseUnit)mUI.durationComboBox->currentData().toInt();
   aPrefData.mTimeFormat         = (UtTime::Fmt)mUI.timeComboBox->currentData().toInt();
   aPrefData.mDateFormat         = static_cast<UnitsData::DateFormat>(mUI.dateComboBox->currentData().toInt());
   aPrefData.mTorque.mUnit       = (UtUnitTorque::BaseUnit)mUI.torqueComboBox->currentData().toInt();

   aPrefData.mAcceleration.mPrecision        = static_cast<unsigned int>(mUI.accelerationSpinBox->value());
   aPrefData.mHighAltitude.mPrecision        = static_cast<unsigned int>(mUI.highAltitudeSpinBox->value());
   aPrefData.mLowAltitude.mPrecision         = static_cast<unsigned int>(mUI.lowAltitudeSpinBox->value());
   aPrefData.mAngle.mPrecision               = static_cast<unsigned int>(mUI.angleSpinBox->value());
   aPrefData.mAngularRate.mPrecision         = static_cast<unsigned int>(mUI.angularRateSpinBox->value());
   aPrefData.mAngularAcceleration.mPrecision = static_cast<unsigned int>(mUI.angularAccelerationSpinBox->value());
   aPrefData.mForce.mPrecision               = static_cast<unsigned int>(mUI.forceSpinBox->value());
   aPrefData.mFrequency.mPrecision           = static_cast<unsigned int>(mUI.frequencySpinBox->value());
   aPrefData.mLength.mPrecision              = static_cast<unsigned int>(mUI.lengthSpinBox->value());
   aPrefData.mMass.mPrecision                = static_cast<unsigned int>(mUI.massSpinBox->value());
   aPrefData.mMassTransfer.mPrecision        = static_cast<unsigned int>(mUI.massTransferSpinBox->value());
   aPrefData.mPower.mPrecision               = static_cast<unsigned int>(mUI.powerSpinBox->value());
   aPrefData.mPowerDB.mPrecision             = static_cast<unsigned int>(mUI.powerDBSpinBox->value());
   aPrefData.mPressure.mPrecision            = static_cast<unsigned int>(mUI.pressureSpinBox->value());
   aPrefData.mRatio.mPrecision               = static_cast<unsigned int>(mUI.ratioSpinBox->value());
   aPrefData.mSpeed.mPrecision               = static_cast<unsigned int>(mUI.speedSpinBox->value());
   aPrefData.mTime.mPrecision                = static_cast<unsigned int>(mUI.durationSpinBox->value());
}

void wkf::UnitsWidget::showEvent(QShowEvent*)
{
   // This is necessary because the pref widget isn't constructed until after settings are loaded into pref object
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
}

void wkf::UnitsWidget::ReadPreferenceData(const UnitsData& aPrefData)
{
   int index = mUI.accelerationComboBox->findData(aPrefData.mAcceleration.mUnit);
   mUI.accelerationComboBox->setCurrentIndex(index);

   index = mUI.highAltitudeComboBox->findData(aPrefData.mHighAltitude.mUnit);
   mUI.highAltitudeComboBox->setCurrentIndex(index);

   index = mUI.lowAltitudeComboBox->findData(aPrefData.mLowAltitude.mUnit);
   mUI.lowAltitudeComboBox->setCurrentIndex(index);

   index = mUI.angleComboBox->findData(aPrefData.mAngle.mUnit);
   mUI.angleComboBox->setCurrentIndex(index);

   index = mUI.angularRateComboBox->findData(aPrefData.mAngularRate.mUnit);
   mUI.angularRateComboBox->setCurrentIndex(index);

   index = mUI.angularAccelerationComboBox->findData(aPrefData.mAngularAcceleration.mUnit);
   mUI.angularAccelerationComboBox->setCurrentIndex(index);

   index = mUI.durationComboBox->findData(aPrefData.mTime.mUnit);
   mUI.durationComboBox->setCurrentIndex(index);

   index = mUI.forceComboBox->findData(aPrefData.mForce.mUnit);
   mUI.forceComboBox->setCurrentIndex(index);

   index = mUI.frequencyComboBox->findData(aPrefData.mFrequency.mUnit);
   mUI.frequencyComboBox->setCurrentIndex(index);

   index = mUI.lengthComboBox->findData(aPrefData.mLength.mUnit);
   mUI.lengthComboBox->setCurrentIndex(index);

   index = mUI.massComboBox->findData(aPrefData.mMass.mUnit);
   mUI.massComboBox->setCurrentIndex(index);

   index = mUI.massTransferComboBox->findData(aPrefData.mMassTransfer.mUnit);
   mUI.massTransferComboBox->setCurrentIndex(index);

   index = mUI.powerComboBox->findData(aPrefData.mPower.mUnit);
   mUI.powerComboBox->setCurrentIndex(index);

   index = mUI.powerDBComboBox->findData(aPrefData.mPowerDB.mUnit);
   mUI.powerDBComboBox->setCurrentIndex(index);

   index = mUI.pressureComboBox->findData(aPrefData.mPressure.mUnit);
   mUI.pressureComboBox->setCurrentIndex(index);

   index = mUI.ratioComboBox->findData(aPrefData.mRatio.mUnit);
   mUI.ratioComboBox->setCurrentIndex(index);

   index = mUI.speedComboBox->findData(aPrefData.mSpeed.mUnit);
   mUI.speedComboBox->setCurrentIndex(index);

   index = mUI.latLonComboBox->findData(aPrefData.mLatLonFormat);
   mUI.latLonComboBox->setCurrentIndex(index);

   index = mUI.timeComboBox->findData(aPrefData.mTimeFormat);
   mUI.timeComboBox->setCurrentIndex(index);

   index = mUI.dateComboBox->findData(aPrefData.mDateFormat);
   mUI.dateComboBox->setCurrentIndex(index);

   index = mUI.torqueComboBox->findData(aPrefData.mTorque.mUnit);
   mUI.torqueComboBox->setCurrentIndex(index);

   mUI.accelerationSpinBox->setValue(aPrefData.mAcceleration.mPrecision);
   mUI.highAltitudeSpinBox->setValue(aPrefData.mHighAltitude.mPrecision);
   mUI.lowAltitudeSpinBox->setValue(aPrefData.mLowAltitude.mPrecision);
   mUI.angleSpinBox->setValue(aPrefData.mAngle.mPrecision);
   mUI.angularRateSpinBox->setValue(aPrefData.mAngularRate.mPrecision);
   mUI.angularAccelerationSpinBox->setValue(aPrefData.mAngularAcceleration.mPrecision);
   mUI.durationSpinBox->setValue(aPrefData.mTime.mPrecision);
   mUI.forceSpinBox->setValue(aPrefData.mForce.mPrecision);
   mUI.frequencySpinBox->setValue(aPrefData.mFrequency.mPrecision);
   mUI.lengthSpinBox->setValue(aPrefData.mLength.mPrecision);
   mUI.massSpinBox->setValue(aPrefData.mMass.mPrecision);
   mUI.massTransferSpinBox->setValue(aPrefData.mMassTransfer.mPrecision);
   mUI.powerSpinBox->setValue(aPrefData.mPower.mPrecision);
   mUI.powerDBSpinBox->setValue(aPrefData.mPowerDB.mPrecision);
   mUI.pressureSpinBox->setValue(aPrefData.mPressure.mPrecision);
   mUI.ratioSpinBox->setValue(aPrefData.mRatio.mPrecision);
   mUI.speedSpinBox->setValue(aPrefData.mSpeed.mPrecision);
   mUI.torqueSpinBox->setValue(aPrefData.mTorque.mPrecision);
}
