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

#include "RadioConfigWidget.hpp"

#include <QComboBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QLabel>

namespace
{

const QString cRADIOTYPE{"radio"};

}

namespace wsfg
{

namespace spaceg
{

RadioConfigWidget::RadioConfigWidget(QWidget* aParentPtr)
   : ConfigWidget{aParentPtr}
   , mName{}
   , mLabelPtr{new QLabel{}}
   , mComboBoxPtr{new QComboBox{}}
{
   QHBoxLayout* layoutPtr = new QHBoxLayout{};
   layoutPtr->addWidget(mLabelPtr, 1);
   layoutPtr->addWidget(mComboBoxPtr, 2);
   setLayout(layoutPtr);
}

//! Configure this widget.
//!
//! Configure this widget according to the provided @p aConfiguration. The following
//! keys in @p aConfiguration effect this object:
//!  * "name"       - The name of the property represented by this widget;
//!  * "display"    - The text displayed labeling the property.
//!  * "properties" - An array containing the mutually exclusive options for this property.
//! The objects forming the "properties" need to have both a "option" and "label" value
//! provided. The "option" is the output value provided by GetValues, and the "label"
//! is the text displayed in the combo box.
void RadioConfigWidget::Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration)
{
   if (ValidateObject(aConfiguration))
   {
      mName = aConfiguration.constFind(cNAME)->toString();
      mLabelPtr->setText(aConfiguration.constFind(cDISPLAY)->toString());
      // now setup the combo box
      QJsonArray props = aConfiguration.constFind(cPROPERTIES)->toArray();
      for (int i = 0; i < props.size(); ++i)
      {
         QJsonObject currObject = props[i].toObject();
         if (currObject.constFind(cLABEL)->isString() && currObject.constFind(cOPTION)->isString())
         {
            mComboBoxPtr->addItem(currObject.constFind(cLABEL)->toString(), currObject.constFind(cOPTION)->toString());
         }
      }
   }
}

//! Get the value entered into this widget.
//!
//! This will return a JSON object with a single value with a key
//! equal to the configured "name" of this object. The value will
//! be the "option" associated with the selected entry in the combo
//! box in this widget.
QJsonObject RadioConfigWidget::GetValues() const
{
   QJsonObject retval{};
   retval[mName] = mComboBoxPtr->currentData().toString();
   return retval;
}

//! Set the values displayed by this widget.
//!
//! This will set the state of the combo box to match the value of
//! the provided field in @p aData. If there is no field in @p aDate
//! with a key equal to the configured "name" of this widget, then
//! nothing will happen, and false is returned.
//!
//! \param aData - The value to set in this widget.
//! \returns     - true is set was successful; false otherwise.
bool RadioConfigWidget::SetValues(const QJsonObject& aData)
{
   bool retval{false};

   auto iter = aData.constFind(mName);
   if (iter != aData.constEnd())
   {
      int index = mComboBoxPtr->findData(iter->toString());
      if (index != -1)
      {
         mComboBoxPtr->setCurrentIndex(index);
      }
   }

   return retval;
}

void RadioConfigWidget::ClearValues()
{
   mComboBoxPtr->setCurrentIndex(0);
}

QString RadioConfigWidget::Type()
{
   return cRADIOTYPE;
}

bool RadioConfigWidget::ValidateObject(const QJsonObject& aConfiguration)
{
   return aConfiguration.constFind(cTYPE)->toString() == Type() && aConfiguration.constFind(cNAME)->isString() &&
          aConfiguration.constFind(cDISPLAY)->isString() && aConfiguration.constFind(cPROPERTIES)->isArray();
}

} // namespace spaceg

} // namespace wsfg
