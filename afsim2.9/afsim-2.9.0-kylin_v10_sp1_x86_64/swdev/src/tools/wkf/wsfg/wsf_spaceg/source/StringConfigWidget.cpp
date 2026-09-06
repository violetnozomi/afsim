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

#include "StringConfigWidget.hpp"

#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>

namespace
{

const QString cSTRINGTYPE("string");

}

namespace wsfg
{

namespace spaceg
{

StringConfigWidget::StringConfigWidget(QWidget* aParentPtr)
   : ConfigWidget(aParentPtr)
   , mPropertyName{}
   , mLabelPtr{new QLabel{}}
   , mValuePtr{new QLineEdit{}}
{
   QHBoxLayout* layoutPtr = new QHBoxLayout{};
   layoutPtr->addWidget(mLabelPtr, 1);
   layoutPtr->addWidget(mValuePtr, 2);
   setLayout(layoutPtr);
}

//! Configure this widget.
//! Configure this widget with the provided @p aConfiguration. The fields in the
//! @p aConfiguration that effect this widget are:
//!  * "name"    - The name of property represented by this widget.
//!  * "display" - The text displayed in the label in this widget.
void StringConfigWidget::Configure(const ConfigWidgetFactory& /* UNUSED */, const QJsonObject& aConfiguration)
{
   if (ValidateObject(aConfiguration))
   {
      mPropertyName = aConfiguration.constFind(cNAME)->toString();
      mLabelPtr->setText(aConfiguration.constFind(cDISPLAY)->toString());
   }
}

//! Get the values entered into this widget.
//!
//! This will return a JSON object with a single field having a key
//! equal to the configured "name" of this object, and a value equal
//! to the entered text.
QJsonObject StringConfigWidget::GetValues() const
{
   QJsonObject retval{};
   if (!mValuePtr->text().isEmpty())
   {
      retval[mPropertyName] = mValuePtr->text();
   }
   return retval;
}

//! Set the values displayed by this widget.
//!
//! This will set the displayed text to be equal to the value in @p aData
//! associated with the key equal to the configured "name" of this object.
//! If successful, this will return true. Otherwise, false is returned.
bool StringConfigWidget::SetValues(const QJsonObject& aData)
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

void StringConfigWidget::ClearValues()
{
   mValuePtr->clear();
}

QString StringConfigWidget::Type()
{
   return cSTRINGTYPE;
}

bool StringConfigWidget::ValidateObject(const QJsonObject& aConfiguration)
{
   return aConfiguration.constFind(cTYPE)->toString() == cSTRINGTYPE && aConfiguration.constFind(cNAME)->isString() &&
          aConfiguration.constFind(cDISPLAY)->isString();
}

} // namespace spaceg

} // namespace wsfg
