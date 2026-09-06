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

#include "SequenceConfigWidget.hpp"

#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>

#include "ConfigData.hpp"
#include "ConfigWidgetFactory.hpp"

namespace
{

const QString cSEQUENCETYPE("sequence");

}

namespace wsfg
{

namespace spaceg
{

SequenceConfigWidget::SequenceConfigWidget(QWidget* aParentPtr)
   : ConfigWidget(aParentPtr)
   , mProperties{}
{
}

//! Configure this object.
//!
//! Configure this object according to the provided @p aConfiguration. The fields in
//! @p aConfiguration that effect this object are:
//!  * "properties" - An array containing a set of other property specifications to display.
//! The "properties" objects can be any other valid configuration for a ConfigWidget subclass.
//! This configuration step will create and configure any contained widgets. This
//! object does not use "name" or "display" because the properties included are
//! not grouped in any way. If one of the "properties" is of a "type" unknown to the
//! @p aFactory, then it will be ignored, and a ConfigWidget subclass will not be
//! created for it.
void SequenceConfigWidget::Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration)
{
   if (ValidateObject(aConfiguration))
   {
      QVBoxLayout* layoutPtr = new QVBoxLayout{};
      layoutPtr->setContentsMargins(0, 0, 0, 0);
      QJsonArray props = aConfiguration.constFind(cPROPERTIES)->toArray();
      for (int i = 0; i < props.size(); ++i)
      {
         QJsonObject   currObject = props[i].toObject();
         ConfigWidget* widgetPtr  = aFactory.Create(currObject);
         if (widgetPtr)
         {
            layoutPtr->addWidget(widgetPtr);
            mProperties[currObject.constFind(cNAME)->toString()] = widgetPtr;
         }
      }
      layoutPtr->addStretch(1);
      setLayout(layoutPtr);
   }
}

//! Get the values entered into this widget.
//!
//! This will return a JSON object with entries for each of the
//! "properties" configured for this widget. The fields added to the
//! return value will be added directly, and will not be grouped in
//! any fashion.
QJsonObject SequenceConfigWidget::GetValues() const
{
   QJsonObject retval{};
   for (auto& iter : mProperties)
   {
      ConfigData::Merge(retval, iter.second->GetValues());
   }
   return retval;
}

//! Set the valued displayed by this widget.
//!
//! This will set the values for each property in this widget in turn. If
//! all of these values are set successfully, this will return true. Otherwise,
//! this will return false.
bool SequenceConfigWidget::SetValues(const QJsonObject& aData)
{
   bool retval{true};
   for (auto& iter : mProperties)
   {
      retval = iter.second->SetValues(aData) && retval;
   }
   return retval;
}

void SequenceConfigWidget::ClearValues()
{
   for (auto& iter : mProperties)
   {
      iter.second->ClearValues();
   }
}

QString SequenceConfigWidget::Type()
{
   return cSEQUENCETYPE;
}

bool SequenceConfigWidget::ValidateObject(const QJsonObject& aConfiguration)
{
   return aConfiguration.constFind(cTYPE)->toString() == cSEQUENCETYPE && aConfiguration.constFind(cPROPERTIES)->isArray();
}

} // namespace spaceg

} // namespace wsfg
