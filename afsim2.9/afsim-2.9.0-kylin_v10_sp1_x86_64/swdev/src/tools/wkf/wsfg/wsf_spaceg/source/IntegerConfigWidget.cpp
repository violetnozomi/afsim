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

#include "IntegerConfigWidget.hpp"

#include <QHBoxLayout>
#include <QIntValidator>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>

namespace
{

const QString cINTEGERTYPE("integer");
const QString cUNSIGNED("unsigned");

} // namespace

namespace wsfg
{

namespace spaceg
{

IntegerConfigWidget::IntegerConfigWidget(QWidget* aParentPtr)
   : ConfigWidget(aParentPtr)
   , mPropertyName{}
   , mLabelPtr{new QLabel{}}
   , mValuePtr{new QLineEdit{}}
{
   QHBoxLayout* layoutPtr = new QHBoxLayout();
   layoutPtr->addWidget(mLabelPtr, 1);
   layoutPtr->addWidget(mValuePtr, 2);
   setLayout(layoutPtr);
}

//! Configure this object.
//!
//! Configure this widget according to the provided @p aConfiguration. The following
//! fields in @p aConfiguration effect this object:
//!  * "name"     - The name of the integer property represented by this widget;
//!  * "display"  - The text appearing in the label for this widget;
//!  * "unsigned" - Indicates if the integer collected must be non-negative.
void IntegerConfigWidget::Configure(const ConfigWidgetFactory& /* UNUSED */, const QJsonObject& aConfiguration)
{
   if (ValidateObject(aConfiguration))
   {
      mPropertyName = aConfiguration.constFind(cNAME)->toString();
      mLabelPtr->setText(aConfiguration.constFind(cDISPLAY)->toString());
      auto validatorPtr = new QIntValidator{};
      if (aConfiguration.constFind(cUNSIGNED)->toBool(false))
      {
         validatorPtr->setBottom(0);
      }
      mValuePtr->setValidator(validatorPtr);
   }
}

//! Get the value entered into this widget.
//!
//! This will return a JSON object with a single field under the key equal to the
//! configured "name" of this object, and a value that is a string containing an
//! integer (signed or unsigned according to the "unsigned" configuration parameter).
QJsonObject IntegerConfigWidget::GetValues() const
{
   QJsonObject retval{};
   if (!mValuePtr->text().isEmpty())
   {
      retval[mPropertyName] = mValuePtr->text();
   }
   return retval;
}

//! Set the value displayed by this widget.
//!
//! This will set the displayed value to match the value provided in @p aData.
//! If there was no field in @p aData matching the "name" of this object then
//! the displayed value will not be modified and this will return false.
//!
//! \param aData - the value to set in this widget.
//! \returns     - true if the value was set; false otherwise.
bool IntegerConfigWidget::SetValues(const QJsonObject& aData)
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

void IntegerConfigWidget::ClearValues()
{
   mValuePtr->clear();
}

QString IntegerConfigWidget::Type()
{
   return cINTEGERTYPE;
}

bool IntegerConfigWidget::ValidateObject(const QJsonObject& aConfiguration)
{
   return aConfiguration.constFind(cTYPE)->toString() == cINTEGERTYPE && aConfiguration.constFind(cNAME)->isString() &&
          aConfiguration.constFind(cDISPLAY)->isString();
}

} // namespace spaceg

} // namespace wsfg
