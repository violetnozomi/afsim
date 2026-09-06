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

#include "EnablerConfigWidget.hpp"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>

#include "ConfigData.hpp"
#include "ConfigWidgetFactory.hpp"

namespace
{

const QString cENABLERTYPE("enabler");

} // namespace

namespace wsfg
{

namespace spaceg
{

EnablerConfigWidget::EnablerConfigWidget(QWidget* aParentPtr)
   : ConfigWidget(aParentPtr)
   , mName{}
   , mGroupPtr{new QGroupBox}
   , mProperties{}
{
   QHBoxLayout* layoutPtr = new QHBoxLayout{};
   layoutPtr->addWidget(mGroupPtr);
   setLayout(layoutPtr);
}

//! Configure this widget.
//!
//! This method configures this widget according to the provided @p aConfiguration. The
//! following fields in @p aData effect this object:
//!  * "name"       - The name of the set of properties represented by this widget;
//!  * "display"    - The text displayed in the checkable group box containing the following "properties";
//!  * "properties" - An array of other objects that configure the widgets forming this group.
//! This object will create further ConfigWidgets for each element of the "properties" array.
//! If one of the elements of the "properties" array is of a type not known by the provided
//! @p aFactory, then that property is skipped.
void EnablerConfigWidget::Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration)
{
   if (ValidateObject(aConfiguration))
   {
      mName = aConfiguration.constFind(cNAME)->toString();
      mGroupPtr->setTitle(aConfiguration.constFind(cDISPLAY)->toString());
      mGroupPtr->setCheckable(true);
      mGroupPtr->setChecked(false);

      QVBoxLayout* layoutPtr = new QVBoxLayout{};
      QJsonArray   props     = aConfiguration.constFind(cPROPERTIES)->toArray();
      for (int i = 0; i < props.size(); ++i)
      {
         QJsonObject   currObject = props[i].toObject();
         ConfigWidget* widgetPtr  = aFactory.Create(currObject);
         if (widgetPtr)
         {
            layoutPtr->addWidget(widgetPtr);
            auto nameIter = currObject.constFind(cNAME);
            if (nameIter != currObject.constEnd())
            {
               mProperties[currObject.constFind(cNAME)->toString()] = widgetPtr;
            }
         }
      }
      mGroupPtr->setLayout(layoutPtr);
   }
}

//! Get the values entered into this widget.
//!
//! If the group as a whole is not enabled, then this will return an empty object.
//! Otherwise, this will return an object with a single field under the key
//! equal to the configured "name" of this object. The value of that field will
//! be an object containing all of the values of the "properties" of this
//! group.
QJsonObject EnablerConfigWidget::GetValues() const
{
   QJsonObject retval{};
   bool        checked = mGroupPtr->isChecked();
   if (checked)
   {
      QJsonObject group{};
      for (auto& iter : mProperties)
      {
         ConfigData::Merge(group, iter.second->GetValues());
      }
      retval[mName] = group;
   }

   return retval;
}

//! Set the values displayed by this widget.
//!
//! If the provided @p aData does not contain a field under the key
//! equal to the configured "name" of this widget, then this widget will
//! be cleared, disabling it. Otherwise, the value of the field will be
//! used to set the values on the "properties" of this widget.
//! This will return true is all properties are successfully set by
//! this method.
bool EnablerConfigWidget::SetValues(const QJsonObject& aData)
{
   bool retval{true};

   auto groupIter = aData.constFind(mName);
   if (groupIter != aData.constEnd())
   {
      QJsonObject groupData = groupIter->toObject();
      mGroupPtr->setChecked(true);
      for (auto& iter : mProperties)
      {
         bool ok = iter.second->SetValues(groupData);
         retval  = retval && ok;
      }
   }
   else
   {
      ClearValues();
   }

   return retval;
}

void EnablerConfigWidget::ClearValues()
{
   mGroupPtr->setChecked(false);
   for (auto& iter : mProperties)
   {
      iter.second->ClearValues();
   }
}

QString EnablerConfigWidget::Type()
{
   return cENABLERTYPE;
}

bool EnablerConfigWidget::ValidateObject(const QJsonObject& aConfiguration)
{
   return aConfiguration.constFind(cTYPE)->toString() == cENABLERTYPE && aConfiguration.constFind(cNAME)->isString() &&
          aConfiguration.constFind(cDISPLAY)->isString() && aConfiguration.constFind(cPROPERTIES)->isArray();
}

} // namespace spaceg

} // namespace wsfg
