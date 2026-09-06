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

#ifndef REALCONFIGWIDGET_H
#define REALCONFIGWIDGET_H

#include <vector>

#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>

#include "ConfigWidget.hpp"
#include "UtQtUnitValueEdit.hpp"

namespace wsfg
{

namespace spaceg
{

//! A simple trait-like structure to provide the 'type' of a specialization of RealConfigWidgetT.
template<typename LINE_EDIT>
struct RealConfigWidgetTrait
{
   static QString TypeString();
};

//! A template class to provide unit aware real valued config widgets.
//! The LINE_EDIT type must:
//!  * provide default construction,
//!  * be derived from QWidgt,
//!  * provide a clear() method to clear the text,
//!  * provide a text() method to retrieve the entered text,
//!  * provide a setText() method to set the text.
//! Further, a specialization of RealConfigWidgetTrait must be created
//! for the same LINE_EDIT type.
template<typename LINE_EDIT>
class RealConfigWidgetT : public ConfigWidget
{
public:
   explicit RealConfigWidgetT(QWidget* aParentPtr = nullptr)
      : ConfigWidget(aParentPtr)
      , mPropertyName{}
      , mLabelPtr{new QLabel{this}}
      , mValuePtr{new LINE_EDIT{this}}
   {
      QHBoxLayout* layoutPtr = new QHBoxLayout{};
      layoutPtr->addWidget(mLabelPtr, 1);
      layoutPtr->addWidget(mValuePtr, 2);
      setLayout(layoutPtr);
   }

   ~RealConfigWidgetT() override {}

   //! Configure this widget.
   //!
   //! Configure this widget according to the provided @p aConfiguration. The following
   //! keys in @p aConfiguration effect this object:
   //!  * "name"    - The name of the real value represented by this widget.
   //!  * "display" - The displayed label for the value represented by this widget.
   void Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override
   {
      if (ValidateObject(aConfiguration))
      {
         mPropertyName = aConfiguration.constFind(cNAME)->toString();
         mLabelPtr->setText(aConfiguration.constFind(cDISPLAY)->toString());
      }
   }

   //! Get the values entered into this widget.
   //!
   //! This will return a JSON object with a single field under a key equal to the
   //! configured "name" of this object. The value will be the text entered into
   //! the line edit in this widget. A text value is used to allow for the units
   //! to be carried through.
   QJsonObject GetValues() const override
   {
      QJsonObject retval{};
      if (mValuePtr->IsValid() && !mValuePtr->text().isEmpty())
      {
         retval[mPropertyName] = mValuePtr->text();
      }
      return retval;
   }

   //! Set the values displayed by this widget.
   //!
   //! This will only modify the displayed value if there is a field in @p aData
   //! with a key matching the configured "name" of this object. If there is such
   //! a key, this will return true. Otherwise, no changes to the displayed data
   //! will occur, and this will return false.
   bool SetValues(const QJsonObject& aData) override
   {
      bool retval{false};
      auto iter = aData.constFind(mPropertyName);
      if (iter != aData.constEnd())
      {
         mValuePtr->setText(iter->toString());
         retval = true;
      }
      return retval;
   }

   void ClearValues() override { mValuePtr->clear(); }

   static QString Type() { return RealConfigWidgetTrait<LINE_EDIT>::TypeString(); }

private:
   static bool ValidateObject(const QJsonObject& aConfiguration)
   {
      return aConfiguration.constFind(cTYPE)->toString() == Type() && aConfiguration.constFind(cNAME)->isString() &&
             aConfiguration.constFind(cDISPLAY)->isString();
   }

   QString    mPropertyName;
   QLabel*    mLabelPtr;
   LINE_EDIT* mValuePtr;
};

//! An adapter type that will allow for unitless real-valued config widgets.
class UnitlessRealLineEdit : public QLineEdit
{
public:
   explicit UnitlessRealLineEdit(QWidget* aParentPtr = nullptr)
      : QLineEdit(aParentPtr)
   {
      setValidator(new QDoubleValidator{});
   }

   bool IsValid() const { return true; }
};

// Specializations to cover the types used by Astrolabe.
template<>
struct RealConfigWidgetTrait<UtQtLengthValueEdit>
{
   static QString TypeString() { return QString{"length"}; };
};

template<>
struct RealConfigWidgetTrait<UtQtTimeValueEdit>
{
   static QString TypeString() { return QString{"time"}; };
};

template<>
struct RealConfigWidgetTrait<UtQtSpeedValueEdit>
{
   static QString TypeString() { return QString{"speed"}; };
};

template<>
struct RealConfigWidgetTrait<UtQtAngleValueEdit>
{
   static QString TypeString() { return QString{"angle"}; };
};

template<>
struct RealConfigWidgetTrait<UtQtAngularRateValueEdit>
{
   static QString TypeString() { return QString{"angle_rate"}; }
};

template<>
struct RealConfigWidgetTrait<UnitlessRealLineEdit>
{
   static QString TypeString() { return QString{"real"}; }
};

using LengthConfigWidget    = RealConfigWidgetT<UtQtLengthValueEdit>;
using TimeConfigWidget      = RealConfigWidgetT<UtQtTimeValueEdit>;
using SpeedConfigWidget     = RealConfigWidgetT<UtQtSpeedValueEdit>;
using AngleConfigWidget     = RealConfigWidgetT<UtQtAngleValueEdit>;
using AngleRateConfigWidget = RealConfigWidgetT<UtQtAngularRateValueEdit>;
using RealConfigWidget      = RealConfigWidgetT<UnitlessRealLineEdit>;

} // namespace spaceg

} // namespace wsfg

#endif // REALCONFIGWIDGET_H
