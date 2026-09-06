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

#include "BooleanConfigWidget.hpp"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QJsonObject>

namespace
{

const QString cTHISTYPE("boolean");

}

namespace wsfg
{

namespace spaceg
{

BooleanConfigWidget::BooleanConfigWidget(QWidget* aParentPtr)
   : ConfigWidget(aParentPtr)
   , mCheckboxPtr{nullptr}
{
   mCheckboxPtr           = new QCheckBox();
   QHBoxLayout* layoutPtr = new QHBoxLayout();
   layoutPtr->addWidget(mCheckboxPtr);
   setLayout(layoutPtr);
}

//! Configure this widget.
//!
//! Configure this widget according to the provided @p aConfiguration. The following
//! keys in @p aConfiguration effect this object:
//!  * "name"    - The name of the boolean property represented by this widget;
//!  * "display" - The text displayed in the check box.
void BooleanConfigWidget::Configure(const ConfigWidgetFactory& /* UNUSED */, const QJsonObject& aConfiguration)
{
   if (ValidateObject(aConfiguration))
   {
      mPropertyName = aConfiguration.constFind(cNAME)->toString();
      mCheckboxPtr->setText(aConfiguration.constFind(cDISPLAY)->toString());
   }
}

//! Get the values entered into this widget.
//!
//! This will return a JSON object with a single value with a key
//! equal to the configured "name" of this object, but only if the
//! check box is selected. Otherwise, this returns an empty JSON object.
QJsonObject BooleanConfigWidget::GetValues() const
{
   QJsonObject retval{};
   if (mCheckboxPtr->isChecked())
   {
      retval[mPropertyName] = true;
   }
   return retval;
}

//! Set the values displayed by this widget.
//!
//! This will set the state of the check box to match the value of the
//! the value in @p aData under the key equal to the configured "name"
//! of this object. If there was such a key in @p aData, then this
//! method returns true.
//!
//! \param aData - the value to set in this widget.
//! \returns     - true if @p aData contained the correct key; false otherwise.
bool BooleanConfigWidget::SetValues(const QJsonObject& aData)
{
   bool retval{false};
   auto iter = aData.constFind(mPropertyName);
   if (iter != aData.constEnd())
   {
      bool value = iter->toBool();
      mCheckboxPtr->setChecked(value);
      retval = true;
   }
   return retval;
}

//! Clear the value displayed by this widget.
void BooleanConfigWidget::ClearValues()
{
   mCheckboxPtr->setChecked(false);
}

//! Return the type of this widget.
QString BooleanConfigWidget::Type()
{
   return cTHISTYPE;
}

//! Check that the provided configuration contains all of the needed fields.
bool BooleanConfigWidget::ValidateObject(const QJsonObject& aConfiguration)
{
   return aConfiguration.constFind(cTYPE)->toString() == cTHISTYPE && aConfiguration.constFind(cNAME)->isString() &&
          aConfiguration.constFind(cDISPLAY)->isString();
}

} // namespace spaceg

} // namespace wsfg
