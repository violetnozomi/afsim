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
#ifndef WKFUNITTYPES_HPP
#define WKFUNITTYPES_HPP

#include "wkf_export.h"

#include <sstream>

#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>

#include "UtLatPos.hpp"
#include "UtLonPos.hpp"
#include "UtMemory.hpp"
#include "UtTime.hpp"
#include "UtUnitTypes.hpp"
#include "UtUnits.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlatformDataTreeItem.hpp"
#include "WkfUnitsObject.hpp"

namespace wkf
{
class UnitInterface;

// UNIT BASE
class WKF_EXPORT UnitBase : public QObject
{
   friend class UnitInterface;

public:
   UnitBase()           = default;
   ~UnitBase() override = default;

   // Sets the value of the number to be displays, the value is assumed to be in the default units (ie: length is in m)
   void SetValue(double aValue);
   // Indicated whether or not the Popup display is supported
   virtual bool PopupSupported() const { return false; }
   // SetPrecision sets the number of significant digits or digits after the decimal to display, depending on
   // aPrecisionType aPrecisionType maps to the precision argument taken by QString::number() Documentation for
   // QString::number(): https://doc.qt.io/qt-5/qstring.html#argument-formats Defaults to Significant Figures
   void SetPrecision(unsigned int aPrecision, char aPrecisionType = 'g')
   {
      mPrecision     = aPrecision;
      mPrecisionType = aPrecisionType;
   }
   // This sets the QListWidget for the Popup
   // This class does NOT take ownership of the ListWidget and will NOT delete it
   void SetListWidget(QListWidget* aListWidget) { mListWidget = aListWidget; }
   // Removes the List widget so that is no longer updated by this class
   void RemoveListWidget() { mListWidget = nullptr; }
   // Virtual function to update the display of this class
   virtual void UpdateDisplay() { SetUnitText(QString::number(mValue, mPrecisionType, mPrecision)); }
   // Sets the title of the pop up box
   void SetPopUpTitle(const QString& aPopUpTitle) { mPopUpTitle = aPopUpTitle; }

protected:
   virtual void InitializeSubscriptions() {}

   // Returns a formatted String with units label
   QString GetString(double aValue, const std::string& aUnitString);

   // Derivations of this class are not friended with UnitInterface, therefore we need this wrapper in the base class to
   // call SetUnitText()
   void SetUnitText(const QString& aText);

   int    mPrecision{6};
   char   mPrecisionType{'g'};
   double mValue{0.0};

   QString        mPopUpTitle{};
   QListWidget*   mListWidget{nullptr};
   UnitInterface* mUnitInterface{nullptr};

private:
   void SetInterface(UnitInterface* aUnitInterface) { mUnitInterface = aUnitInterface; }
   bool mInitialized{false};
};

// UNIT INTERFACE for QWidgets
class WKF_EXPORT UnitInterface
{
   friend class UnitBase;

public:
   UnitInterface(std::unique_ptr<UnitBase> aUnit);
   virtual ~UnitInterface() = default;

   // Pass through functions so that the object to not need to get the UnitBase under the hood to interact with this class.
   void SetValue(double aValue) { mUnit->SetValue(aValue); }
   bool PopupSupported() { return mUnit->PopupSupported(); }
   void SetPrecision(unsigned int aPrecision, char aPrecisionType = 'g')
   {
      mUnit->SetPrecision(aPrecision, aPrecisionType);
   }
   void SetListWidget(QListWidget* aListWidget) { mUnit->SetListWidget(aListWidget); }
   void RemoveListWidget() { mUnit->RemoveListWidget(); }
   void UpdateDisplay() { mUnit->UpdateDisplay(); }

protected:
   // Want this to be protected so users are forced to use SetValue() to set the QString that is displayed.
   //  However this leads to the need of a friend classes between UnitInterface and UnitBase.
   virtual void SetUnitText(const QString& aText) = 0;

   std::unique_ptr<UnitBase> mUnit;
};

template<class UNIT_TYPE, UnitSignal<UNIT_TYPE> UNIT_SIGNAL, typename UNIT_TYPE::BaseUnit... Units>
class UnitBaseT : public UnitBase
{
protected:
   void InitializeSubscriptions() override
   {
      connect(wkfEnv.GetPreferenceObject<UnitsObject>(), UNIT_SIGNAL, this, [this]() { UpdateDisplay(); });
   }

   bool PopupSupported() const override
   {
      auto units = {Units...};
      return (units.size() > 0);
   }

   virtual QString GetFormattedUnit() const
   {
      auto unitsObject = wkfEnv.GetPreferenceObject<UnitsObject>();
      return unitsObject->GetFormattedValue(static_cast<ValueType>(UNIT_TYPE::cUNIT_TYPE_ID), mValue);
   }

   void UpdateDisplay() override
   {
      SetUnitText(GetFormattedUnit());
      // The list widget will simply display everything with a default number of digits, instead of extrapolating the
      // precision preference
      UtUnitaryValue<UNIT_TYPE> value(mValue);
      if (mListWidget)
      {
         mListWidget->clear();
         if (!mPopUpTitle.isEmpty())
         {
            mListWidget->addItem(mPopUpTitle);
         }
         for (auto unit : {Units...})
         {
            value.ConvertToUnit(unit);
            mListWidget->addItem(GetString(value.GetAsUnit(), value.GetUnitName()));
         }
      }
   }
};

// Altitude doesn't follow template of other unit types because the unit preference depends on the value, i.e. low alt/high alt
class Altitude :
   public UnitBaseT<UtUnitLength, nullptr, UtUnitLength::cFEET, UtUnitLength::cMILES, UtUnitLength::cNAUTICAL_MILES, UtUnitLength::cMETERS, UtUnitLength::cKILOMETERS>
{
   void InitializeSubscriptions() override
   {
      connect(wkfEnv.GetPreferenceObject<UnitsObject>(), &UnitsObject::LowAltitudeUnitChanged, this, &Altitude::UpdateDisplay);
      connect(wkfEnv.GetPreferenceObject<UnitsObject>(), &UnitsObject::HighAltitudeUnitChanged, this, &Altitude::UpdateDisplay);
   }

   QString GetFormattedUnit() const override
   {
      return wkfEnv.GetPreferenceObject<UnitsObject>()->GetFormattedValue(ValueType::cALTITUDE, mValue);
   }
};

template<class FORMAT_TYPE, FormatPreference FORMAT_PREF, FormatSignal FORMAT_SIGNAL>
class FormattedValueT : public UnitBase
{
protected:
   void InitializeSubscriptions() override
   {
      connect(wkfEnv.GetPreferenceObject<UnitsObject>(), FORMAT_SIGNAL, this, [this]() { UpdateDisplay(); });
   }

   void UpdateDisplay() override
   {
      std::ostringstream oss;
      oss << FORMAT_TYPE(mValue, (wkfEnv.GetPreferenceObject<UnitsObject>()->*FORMAT_PREF)());
      ;
      SetUnitText(QString::fromStdString(oss.str()));
   }
};

#define DEFINE_UNIT_ALIAS(UNIT_TYPE, ...) \
   using UNIT_TYPE = UnitBaseT<UtUnit##UNIT_TYPE, &UnitsObject::UNIT_TYPE##UnitChanged, ##__VA_ARGS__>;

DEFINE_UNIT_ALIAS(Acceleration, UtUnitAcceleration::cG, UtUnitAcceleration::cMETERS_PER_SECOND2)
DEFINE_UNIT_ALIAS(Angle, UtUnitAngle::cRADIANS, UtUnitAngle::cDEGREES, UtUnitAngle::cMILS)
DEFINE_UNIT_ALIAS(AngularRate, UtUnitAngularRate::cRADIANS_PER_SECOND, UtUnitAngularRate::cRPM)
DEFINE_UNIT_ALIAS(AngularAcceleration,
                  UtUnitAngularAcceleration::cRADIANS_PER_SECOND2,
                  UtUnitAngularAcceleration::cDEGREES_PER_SECOND2)
DEFINE_UNIT_ALIAS(Force, UtUnitForce::cNEWTONS, UtUnitForce::cKGF, UtUnitForce::cLBF)
DEFINE_UNIT_ALIAS(Frequency, UtUnitFrequency::cHZ, UtUnitFrequency::cKHZ, UtUnitFrequency::cMHZ, UtUnitFrequency::cGHZ)
DEFINE_UNIT_ALIAS(Length,
                  UtUnitLength::cFEET,
                  UtUnitLength::cMILES,
                  UtUnitLength::cNAUTICAL_MILES,
                  UtUnitLength::cMETERS,
                  UtUnitLength::cKILOMETERS)
DEFINE_UNIT_ALIAS(Mass, UtUnitMass::cKILOGRAMS, UtUnitMass::cPOUNDS, UtUnitMass::cTONS)
DEFINE_UNIT_ALIAS(MassTransfer, UtUnitMassTransfer::cKILOGRAMS_PER_SECOND)
DEFINE_UNIT_ALIAS(Power, UtUnitPower::cWATTS, UtUnitPower::cKILOWATTS, UtUnitPower::cMEGAWATTS, UtUnitPower::cGIGAWATTS)
DEFINE_UNIT_ALIAS(PowerDB, UtUnitPowerDB::cDBW, UtUnitPowerDB::cDBM, UtUnitPowerDB::cWATTS)
DEFINE_UNIT_ALIAS(Pressure, UtUnitPressure::cKILOPASCALS, UtUnitPressure::cPSI)
DEFINE_UNIT_ALIAS(Ratio, UtUnitRatio::cDB, UtUnitRatio::cABSOLUTE)
DEFINE_UNIT_ALIAS(Speed,
                  UtUnitSpeed::cMETERS_PER_SECOND,
                  UtUnitSpeed::cKILOMETERS_PER_HOUR,
                  UtUnitSpeed::cFEET_PER_SECOND,
                  UtUnitSpeed::cFEET_PER_MINUTE,
                  UtUnitSpeed::cMILES_PER_HOUR,
                  UtUnitSpeed::cKNOTS)
DEFINE_UNIT_ALIAS(Torque, UtUnitTorque::cNEWTON_METERS)

using Latitude  = FormattedValueT<UtLatPos, &UnitsObject::GetLatLonFormatting, &UnitsObject::LatLonFormatChanged>;
using Longitude = FormattedValueT<UtLonPos, &UnitsObject::GetLatLonFormatting, &UnitsObject::LatLonFormatChanged>;
using Time      = FormattedValueT<UtTime, &UnitsObject::GetTimeFormatting, &UnitsObject::TimeFormatChanged>;
using Unitless  = UnitBase;

// QLABEL
class WKF_EXPORT UnitLabel : public UnitInterface, public QLabel
{
public:
   UnitLabel(std::unique_ptr<UnitBase> aUnit, QWidget* aParent = nullptr, const QString& aPopUpTitle = "")
      : UnitInterface(std::move(aUnit))
      , QLabel("", aParent)
   {
      mUnit->SetPopUpTitle(aPopUpTitle);
   }

   ~UnitLabel() override = default;

protected:
   void SetUnitText(const QString& aText) override { QLabel::setText(aText); }
};

// QLABEL TEMPLATE
template<class T>
class UnitLabelT : public UnitLabel
{
   static_assert(std::is_base_of<UnitBase, T>::value, "Template argument to UnitLabelT must be a subclass of UnitBase");

public:
   UnitLabelT(QWidget* aParent = nullptr, const QString& aPopupTitle = QString())
      : UnitLabel(ut::make_unique<T>(), aParent, aPopupTitle)
   {
   }
};

// SPECIALIZED QLABELS
using AccelerationLabel        = UnitLabelT<Acceleration>;
using AltitudeLabel            = UnitLabelT<Altitude>;
using AngleLabel               = UnitLabelT<Angle>;
using AngularLabel             = UnitLabelT<AngularRate>;
using AngularAccelerationLabel = UnitLabelT<AngularAcceleration>;
using ForceLabel               = UnitLabelT<Force>;
using FrequencyLabel           = UnitLabelT<Frequency>;
using LatitudeLabel            = UnitLabelT<Latitude>;
using LengthLabel              = UnitLabelT<Length>;
using LongitudeLabel           = UnitLabelT<Longitude>;
using MassLabel                = UnitLabelT<Mass>;
using MassTransferLabel        = UnitLabelT<MassTransfer>;
using PowerLabel               = UnitLabelT<Power>;
using PowerDBLabel             = UnitLabelT<PowerDB>;
using PressureLabel            = UnitLabelT<Pressure>;
using RatioLabel               = UnitLabelT<Ratio>;
using SpeedLabel               = UnitLabelT<Speed>;
using TimeLabel                = UnitLabelT<Time>;
using TorqueLabel              = UnitLabelT<Torque>;
using UnitlessLabel            = UnitLabelT<Unitless>;

// QLINEEDIT
// NOTE: QLineEdit is included for completeness, it is not recommend to use QLineEdit to
//       display data as it will lead to an outdated and ugly UI appearance. However it is
//       possible to use QLineEdit to display data, so it is included.  We default ReadOnly
//       to true, because we do not validate input into the UnitLineEdit
class WKF_EXPORT UnitLineEdit : public UnitInterface, public QLineEdit
{
public:
   UnitLineEdit(std::unique_ptr<UnitBase> aUnit, QWidget* aParent = nullptr, const QString& aPopUpTitle = "")
      : UnitInterface(std::move(aUnit))
      , QLineEdit("", aParent)
   {
      mUnit->SetPopUpTitle(aPopUpTitle);
      setReadOnly(true); // Don't support writing to the UnitLineEdit
   }

   ~UnitLineEdit() override = default;

protected:
   void SetUnitText(const QString& aText) override { QLineEdit::setText(aText); }

   // Hide this function from users to force them to use the SetValue() call instead.
   void setText(const QString& text){};
};

// QLINEEDIT TEMPLATE
template<class T>
class UnitLineEditT : public UnitLineEdit
{
   static_assert(std::is_base_of<UnitBase, T>::value, "Template argument to UnitLineEditT must be a subclass of UnitBase");

public:
   UnitLineEditT(QWidget* aParent = nullptr, const QString& aPopupTitle = "")
      : UnitLineEdit(ut::make_unique<T>(), aParent, aPopupTitle)
   {
   }
};

// SPECIALIZED QLINEEDIT
using AccelerationLineEdit        = UnitLineEditT<Acceleration>;
using AltitudeLineEdit            = UnitLineEditT<Altitude>;
using AngleLineEdit               = UnitLineEditT<Angle>;
using AngularLineEdit             = UnitLineEditT<AngularRate>;
using AngularAccelerationLineEdit = UnitLineEditT<AngularAcceleration>;
using ForceLineEdit               = UnitLineEditT<Force>;
using FrequencyLineEdit           = UnitLineEditT<Frequency>;
using LatitudeLineEdit            = UnitLineEditT<Latitude>;
using LengthLineEdit              = UnitLineEditT<Length>;
using LongitudeLineEdit           = UnitLineEditT<Longitude>;
using MassLineEdit                = UnitLineEditT<Mass>;
using MassTransferLineEdit        = UnitLineEditT<MassTransfer>;
using PowerLineEdit               = UnitLineEditT<Power>;
using PowerDBLineEdit             = UnitLineEditT<PowerDB>;
using PressureLineEdit            = UnitLineEditT<Pressure>;
using RatioLineEdit               = UnitLineEditT<Ratio>;
using SpeedLineEdit               = UnitLineEditT<Speed>;
using TimeLineEdit                = UnitLineEditT<Time>;
using UnitlessLineEdit            = UnitLineEditT<Unitless>;
using TorqueLineEdit              = UnitLineEditT<Torque>;

// QLISTWIDGETITEM
class WKF_EXPORT UnitListWidgetItem : public UnitInterface, public QListWidgetItem
{
public:
   UnitListWidgetItem(std::unique_ptr<UnitBase> aUnit, const QString& aPopUpTitle = "")
      : UnitInterface(std::move(aUnit))
      , QListWidgetItem("")
   {
      mUnit->SetPopUpTitle(aPopUpTitle);
   }

   ~UnitListWidgetItem() override = default;

protected:
   void SetUnitText(const QString& aText) override { QListWidgetItem::setText(aText); }

   // Hide this function from users to force them to use the SetValue() call instead.
   void setText(const QString& text){};
};

// QLISTWIDGETITEM TEMPLATE
template<class T>
class UnitListWidgetItemT : public UnitListWidgetItem
{
   static_assert(std::is_base_of<UnitBase, T>::value,
                 "Template argument to UnitListWidgetItemT must be a subclass of UnitBase");

public:
   UnitListWidgetItemT(const QString& aPopupTitle)
      : UnitListWidgetItem(ut::make_unique<T>(), aPopupTitle)
   {
   }
};

// SPECIALIZED QLISTWIDGETITEM
using AccelerationListWidgetItem        = UnitListWidgetItemT<Acceleration>;
using AltitudeListWidgetItem            = UnitListWidgetItemT<Altitude>;
using AngleListWidgetItem               = UnitListWidgetItemT<Angle>;
using AngularListWidgetItem             = UnitListWidgetItemT<AngularRate>;
using AngularAccelerationListWidgetItem = UnitListWidgetItemT<AngularAcceleration>;
using ForceListWidgetItem               = UnitListWidgetItemT<Force>;
using FrequencyListWidgetItem           = UnitListWidgetItemT<Frequency>;
using LatitudeListWidgetItem            = UnitListWidgetItemT<Latitude>;
using LengthListWidgetItem              = UnitListWidgetItemT<Length>;
using LongitudeListWidgetItem           = UnitListWidgetItemT<Longitude>;
using MassListWidgetItem                = UnitListWidgetItemT<Mass>;
using MassTransferListWidgetItem        = UnitListWidgetItemT<MassTransfer>;
using PowerListWidgetItem               = UnitListWidgetItemT<Power>;
using PowerDBListWidgetItem             = UnitListWidgetItemT<PowerDB>;
using PressureListWidgetItem            = UnitListWidgetItemT<Pressure>;
using RatioListWidgetItem               = UnitListWidgetItemT<Ratio>;
using SpeedListWidgetItem               = UnitListWidgetItemT<Speed>;
using TimeListWidgetItem                = UnitListWidgetItemT<Time>;
using UnitlessListWidgetItem            = UnitListWidgetItemT<Unitless>;
using TorqueListWidgetItem              = UnitListWidgetItemT<Torque>;

// TREEWIDGETITEM
class WKF_EXPORT UnitTreeWidgetItem : public UnitInterface, public PlatformDataTreeItem
{
public:
   UnitTreeWidgetItem(QTreeWidgetItem* aParent, const QString& aName, std::unique_ptr<UnitBase> aUnit)
      : UnitInterface(std::move(aUnit))
      , PlatformDataTreeItem(aParent)
   {
      QTreeWidgetItem::setText(eNAME_FIELD, aName);
      mUnit->SetPopUpTitle(aName);
      SetPlottable();
   }

   ~UnitTreeWidgetItem() override = default;

protected:
   enum Fields
   {
      eNAME_FIELD  = 0,
      eVALUE_FIELD = 1
   };

   void SetUnitText(const QString& aText) override { QTreeWidgetItem::setText(eVALUE_FIELD, aText); }

   // Hide this function from users to force them to use the SetValue() call instead.
   void setText(int column, const QString& text){};
};

// TREEWIDGETITEM TEMPLATE
template<class T>
class UnitTreeWidgetItemT : public UnitTreeWidgetItem
{
   static_assert(std::is_base_of<UnitBase, T>::value,
                 "Template argument to UnitTreeWidgetItemT must be a subclass of UnitBase");

public:
   UnitTreeWidgetItemT(QTreeWidgetItem* aParent, const QString& aLabelName)
      : UnitTreeWidgetItem(aParent, aLabelName, ut::make_unique<T>())
   {
   }
};

// SPECIALIZED TREEWIDGETITEMs
using AccelerationTreeWidgetItem        = UnitTreeWidgetItemT<Acceleration>;
using AltitudeTreeWidgetItem            = UnitTreeWidgetItemT<Altitude>;
using AngleTreeWidgetItem               = UnitTreeWidgetItemT<Angle>;
using AngularRateTreeWidgetItem         = UnitTreeWidgetItemT<AngularRate>;
using AngularAccelerationTreeWidgetItem = UnitTreeWidgetItemT<AngularAcceleration>;
using ForceTreeWidgetItem               = UnitTreeWidgetItemT<Force>;
using FrequencyTreeWidgetItem           = UnitTreeWidgetItemT<Frequency>;
using LatitudeTreeWidgetItem            = UnitTreeWidgetItemT<Latitude>;
using LengthTreeWidgetItem              = UnitTreeWidgetItemT<Length>;
using LongitudeTreeWidgetItem           = UnitTreeWidgetItemT<Longitude>;
using MassTreeWidgetItem                = UnitTreeWidgetItemT<Mass>;
using MassTransferTreeWidgetItem        = UnitTreeWidgetItemT<MassTransfer>;
using PowerTreeWidgetItem               = UnitTreeWidgetItemT<Power>;
using PowerDBTreeWidgetItem             = UnitTreeWidgetItemT<PowerDB>;
using PressureTreeWidgetItem            = UnitTreeWidgetItemT<Pressure>;
using RatioTreeWidgetItem               = UnitTreeWidgetItemT<Ratio>;
using SpeedTreeWidgetItem               = UnitTreeWidgetItemT<Speed>;
using TimeTreeWidgetItem                = UnitTreeWidgetItemT<Time>;
using UnitlessTreeWidgetItem            = UnitTreeWidgetItemT<Unitless>;
using TorqueTreeWidgetItem              = UnitTreeWidgetItemT<Torque>;

// TABLEWIDGETITEM
class WKF_EXPORT UnitTableWidgetItem : public UnitInterface, public QTableWidgetItem
{
public:
   UnitTableWidgetItem(std::unique_ptr<UnitBase> aUnit, const QString& aPopUpTitle = "")
      : UnitInterface(std::move(aUnit))
      , QTableWidgetItem("")
   {
      mUnit->SetPopUpTitle(aPopUpTitle);
   }

   ~UnitTableWidgetItem() override = default;

protected:
   void SetUnitText(const QString& aText) override { QTableWidgetItem::setText(aText); }

   // Hide this function from users to force them to use the SetValue() call instead.
   void setText(const QString& text){};
};

// TABLEWIDGETITEM TEMPLATE
template<class T>
class UnitTableWidgetItemT : public UnitTableWidgetItem
{
   static_assert(std::is_base_of<UnitBase, T>::value,
                 "Template argument to UnitTableWidgetItemT must be a subclass of UnitBase");

public:
   UnitTableWidgetItemT(const QString& aPopupTitle)
      : UnitTableWidgetItem(ut::make_unique<T>(), aPopupTitle)
   {
   }
};

// SPECIALIZED TABLEWIDGETITEMs
using AccelerationTableWidgetItem        = UnitTableWidgetItemT<Acceleration>;
using AltitudeTableWidgetItem            = UnitTableWidgetItemT<Altitude>;
using AngleTableWidgetItem               = UnitTableWidgetItemT<Angle>;
using AngularRateTableWidgetItem         = UnitTableWidgetItemT<AngularRate>;
using AngularAccelerationTableWidgetItem = UnitTableWidgetItemT<AngularAcceleration>;
using ForceTableWidgetItem               = UnitTableWidgetItemT<Force>;
using FrequencyTableWidgetItem           = UnitTableWidgetItemT<Frequency>;
using LatitudeTableWidgetItem            = UnitTableWidgetItemT<Latitude>;
using LengthTableWidgetItem              = UnitTableWidgetItemT<Length>;
using LongitudeTableWidgetItem           = UnitTableWidgetItemT<Longitude>;
using MassTableWidgetItem                = UnitTableWidgetItemT<Mass>;
using MassTransferTableWidgetItem        = UnitTableWidgetItemT<MassTransfer>;
using PowerTableWidgetItem               = UnitTableWidgetItemT<Power>;
using PowerDBTableWidgetItem             = UnitTableWidgetItemT<PowerDB>;
using PressureTableWidgetItem            = UnitTableWidgetItemT<Pressure>;
using RatioTableWidgetItem               = UnitTableWidgetItemT<Ratio>;
using SpeedTableWidgetItem               = UnitTableWidgetItemT<Speed>;
using TimeTableWidgetItem                = UnitTableWidgetItemT<Time>;
using UnitlessTableWidgetItem            = UnitTableWidgetItemT<Unitless>;
using TorqueTableWidgetItem              = UnitTableWidgetItemT<Torque>;
} // namespace wkf
#endif
