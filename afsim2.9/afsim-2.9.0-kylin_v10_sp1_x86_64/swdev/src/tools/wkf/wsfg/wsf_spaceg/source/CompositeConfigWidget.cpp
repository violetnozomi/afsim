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

#include "CompositeConfigWidget.hpp"

#include <QGroupBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>

#include "ConfigData.hpp"
#include "ConfigWidgetFactory.hpp"

namespace
{

const QString cCOMPOSITETYPE("composite");

} // namespace

namespace wsfg
{

namespace spaceg
{

CompositeConfigWidget::CompositeConfigWidget(QWidget* aParentPtr)
   : ConfigWidget(aParentPtr)
   , mName{}
   , mProperties{}
   , mGroupPtr{nullptr}
{
   mGroupPtr              = new QGroupBox();
   QHBoxLayout* layoutPtr = new QHBoxLayout{};
   layoutPtr->addWidget(mGroupPtr);
   setLayout(layoutPtr);
}

//! Configure this widget.
//!
//! Configure this widget according to the provided @p aConfiguration. The following
//! keys in @p aConfiguration effect this object:
//!  * "name"       - The name of the group property represented by this object;
//!  * "display"    - The text that appears labeling the group;
//!  * "properties" - An array of other objects that configure the widgets forming this
//!                   group.
//! This object will create further ConfigWidgets for each element of the "properties"
//! array. If one of the elements of the "properties" array is of a type that is not
//! known by the provided @p aFactory, then that property is skipped.
void CompositeConfigWidget::Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration)
{
   if (ValidateObject(aConfiguration))
   {
      mName = aConfiguration.constFind(cNAME)->toString();
      mGroupPtr->setTitle(aConfiguration.constFind(cDISPLAY)->toString());

      QVBoxLayout* layoutPtr = new QVBoxLayout{};
      QJsonArray   props     = aConfiguration.constFind(cPROPERTIES)->toArray();
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
      mGroupPtr->setLayout(layoutPtr);
   }
}

//! Return the values entered into this widget.
//!
//! This will return a JSON object containing the values entered into this widget.
//! The returned object will collect all of the values for each of the "properties"
//! of this widget under a single value with a key equal to the configured "name"
//! of this object.
QJsonObject CompositeConfigWidget::GetValues() const
{
   QJsonObject group{};
   for (auto& iter : mProperties)
   {
      QJsonValue val = iter.second->GetValues();
      ConfigData::Merge(group, val.toObject());
   }

   QJsonObject retval{};
   retval[mName] = group;
   return retval;
}

//! Set the values displayed by this widget.
//!
//! If the provided @p aData contains a value with a key equal to the
//! configured "name" of this object, this method will set the values on
//! each member widget of the group represented by this object.
//!
//! \param aData - the values to set in this widget.
//! \returns     - true if @p aData contained the correct key; false otherwise.
bool CompositeConfigWidget::SetValues(const QJsonObject& aData)
{
   bool retval{true};

   auto groupIter = aData.constFind(mName);
   if (groupIter != aData.constEnd())
   {
      QJsonObject groupData = groupIter->toObject();
      for (auto& iter : mProperties)
      {
         bool ok = iter.second->SetValues(groupData);
         retval  = retval && ok;
      }
   }
   else
   {
      retval = false;
   }

   return retval;
}

//! Clear the values displayed in all the widgets in this group.
void CompositeConfigWidget::ClearValues()
{
   for (auto& iter : mProperties)
   {
      iter.second->ClearValues();
   }
}

//! Return the type of this widget.
QString CompositeConfigWidget::Type()
{
   return cCOMPOSITETYPE;
}

//! Check that the provided configuration contains all of the needed fields.
bool CompositeConfigWidget::ValidateObject(const QJsonObject& aConfiguration)
{
   return aConfiguration.constFind(cTYPE)->toString() == cCOMPOSITETYPE && aConfiguration.constFind(cNAME)->isString() &&
          aConfiguration.constFind(cDISPLAY)->isString() && aConfiguration.constFind(cPROPERTIES)->isArray();
}

} // namespace spaceg

} // namespace wsfg
