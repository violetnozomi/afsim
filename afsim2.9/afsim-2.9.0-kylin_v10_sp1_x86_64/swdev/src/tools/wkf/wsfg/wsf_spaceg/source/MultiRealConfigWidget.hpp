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

#ifndef MULTIREALCONFIGWIDGET_H
#define MULTIREALCONFIGWIDGET_H

#include <QComboBox>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>

#include "ConfigWidget.hpp"
#include "UtQtUnitValueEdit.hpp"

namespace wsfg
{

namespace spaceg
{

template<typename LINE_EDIT>
struct MultiRealConfigWidgetTrait
{
   static QString TypeString();
};

//! A ConfigWidget subclass that represents a single, possibly unit-bearing, real value,
//! which can be provided in a number of equivalent, but mutually exclusive, forms. The
//! resulting widget contains a combo box for selecting the form of the input and an input
//! box for entering the real value. The LINE_EDIT type here has the same requirements as
//! in the RealConfigWidget.
template<typename LINE_EDIT>
class MultiRealConfigWidgetT : public ConfigWidget
{
public:
   explicit MultiRealConfigWidgetT(QWidget* aParentPtr = nullptr)
      : ConfigWidget(aParentPtr)
      , mLabelsPtr{new QComboBox{this}}
      , mValuePtr{new LINE_EDIT{this}}
   {
      QHBoxLayout* layoutPtr = new QHBoxLayout{};
      layoutPtr->addWidget(mLabelsPtr);
      layoutPtr->addWidget(mValuePtr);
      setLayout(layoutPtr);
   }

   ~MultiRealConfigWidgetT() override {}

   //! Configure this widget.
   //!
   //! Configure this widget according to the provided @p aConfiguration. The following
   //! keys in @p aConfiguration effect this object:
   //!  * "name"    - an array giving the set of possible input values;
   //!  * "display" - an array giving the text to display for each option in the combo box.
   //! The set of "name" and "display" values must be the same length, and each "name" pairs
   //! with the "display" at the same relative position in the array. The result of this
   //! c
   void Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration) override
   {
      if (ValidateObject(aConfiguration))
      {
         QJsonArray names   = aConfiguration.value(cNAME).toArray();
         QJsonArray display = aConfiguration.value(cDISPLAY).toArray();
         for (int i = 0; i < display.size(); ++i)
         {
            mLabelsPtr->addItem(display[i].toString(), names[i].toVariant());
         }
      }
   }

   //! Get the value entered into this widget.
   //!
   //! This will return a JSON object with a single field under the key equal to the
   //! "name" for the selected input option. The value for this field will be the
   //! string entered into the input, rather than a numeric value so that the
   //! entered units can be carried along with the value.
   QJsonObject GetValues() const override
   {
      QJsonObject retval{};
      if (!mValuePtr->text().isEmpty())
      {
         retval[CurrentProperty()] = mValuePtr->text();
      }
      return retval;
   }

   //! Set the value displayed by this widget.
   //!
   //! This will set the state of this widget to match the provided @p aData.
   //! This will search @p aData for each of the configured "name" values
   //! and will set the combo box and the text to match the property that is
   //! first found. If no such property is found, then this will return false.
   //!
   //! \param aData - the data to use to set this widget's properties.
   //! \returns     - true if succesfully set; false otherwise.
   bool SetValues(const QJsonObject& aData) override
   {
      bool retval{false};
      for (int i = 0; i < mLabelsPtr->count(); ++i)
      {
         QString candidate = mLabelsPtr->itemData(i).toString();
         auto    iter      = aData.constFind(candidate);
         if (iter != aData.constEnd())
         {
            mValuePtr->setText(iter->toString());
            mLabelsPtr->setCurrentIndex(i);
            retval = true;
            break;
         }
      }
      return retval;
   }

   void ClearValues() override
   {
      mValuePtr->clear();
      mLabelsPtr->setCurrentIndex(0);
   }

   static QString Type() { return MultiRealConfigWidgetTrait<LINE_EDIT>::TypeString(); }

private:
   QString CurrentProperty() const { return mLabelsPtr->currentData().toString(); }

   static bool ValidateObject(const QJsonObject& aConfiguration)
   {
      auto name    = aConfiguration.constFind(cNAME);
      auto display = aConfiguration.constFind(cDISPLAY);
      return aConfiguration.constFind(cTYPE)->toString() == Type() && name->isArray() && display->isArray() &&
             name->toArray().size() == display->toArray().size();
   }

   QComboBox* mLabelsPtr;
   LINE_EDIT* mValuePtr;
};

// Specializations for what we need in Astrolabe.
template<>
struct MultiRealConfigWidgetTrait<UtQtLengthValueEdit>
{
   static QString TypeString() { return QString{"multilength"}; }
};

using MultiLengthConfigWidget = MultiRealConfigWidgetT<UtQtLengthValueEdit>;

} // namespace spaceg

} // namespace wsfg

#endif // MULTIREALCONFIGWIDGET_H
