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

#include "DisjunctionConfigWidget.hpp"

#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QSizePolicy>
#include <QStackedWidget>

#include "ConfigData.hpp"
#include "ConfigWidgetFactory.hpp"

namespace
{

const QString cDISJUNCTIONTYPE("disjunction");

} // namespace

namespace wsfg
{

namespace spaceg
{

DisjunctionConfigWidget::DisjunctionConfigWidget(QWidget* aParentPtr)
   : ConfigWidget(aParentPtr)
   , mGroupBoxPtr{nullptr}
   , mComboBoxPtr{nullptr}
   , mStackedWidgetPtr{nullptr}
   , mProperties{}
{
   // Create the group
   mGroupBoxPtr           = new QGroupBox{};
   QHBoxLayout* layoutPtr = new QHBoxLayout{};
   layoutPtr->addWidget(mGroupBoxPtr);
   setLayout(layoutPtr);

   // now add the two widgets to the group
   mComboBoxPtr                = new QComboBox{};
   mStackedWidgetPtr           = new QStackedWidget{};
   QVBoxLayout* groupLayoutPtr = new QVBoxLayout{};
   groupLayoutPtr->addWidget(mComboBoxPtr);
   groupLayoutPtr->addWidget(mStackedWidgetPtr);
   mGroupBoxPtr->setLayout(groupLayoutPtr);

   connect(mComboBoxPtr, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DisjunctionConfigWidget::ChangePanel);
}

//! Configure this widget.
//!
//! Configure this widget according to the provided @p aConfiguration. The following fields
//! in @p aData effect this object:
//!  * "name" - the name of the properties represented by this object;
//!  * "display" - the text displayed on the group box surrounding the selected widget;
//!  * "properties" - an array of other widget configurations giving the set of widgets for
//!                   the user to select from. Note that to be used in a DisjunctionConfigWidget,
//!                   the entries in "properties" must also specify "option" and "label"
//!                   fields.
//! The result of this configuration is a widget that displays a set of other widgets one
//! at a time inside a labeled group box, with a dropdown containing all of the available
//! options, listed by their "label".
void DisjunctionConfigWidget::Configure(const ConfigWidgetFactory& aFactory, const QJsonObject& aConfiguration)
{
   if (ValidateObject(aConfiguration))
   {
      mName = aConfiguration.constFind(cNAME)->toString();
      mGroupBoxPtr->setTitle(aConfiguration.constFind(cDISPLAY)->toString());

      QJsonArray props = aConfiguration.constFind(cPROPERTIES)->toArray();
      for (int i = 0; i < props.size(); ++i)
      {
         QJsonObject   currObject = props[i].toObject();
         ConfigWidget* widgetPtr  = aFactory.Create(currObject);
         if (widgetPtr)
         {
            // We only want to keep the widget if it is appropriate for a disjunction.
            if (currObject.constFind(cLABEL)->isString() && currObject.constFind(cOPTION)->isString())
            {
               widgetPtr->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
               mStackedWidgetPtr->addWidget(widgetPtr);
               mProperties.push_back(widgetPtr);
               mComboBoxPtr->addItem(currObject.constFind(cLABEL)->toString(), currObject.constFind(cOPTION)->toString());
            }
            else
            {
               delete widgetPtr;
            }
         }
      }
   }
}

//! Get the values entered into this widgets.
//!
//! This will return a JSON object with a single field under the key equal to the
//! configured "name" of this object. The value of that field will be an object containing
//! all of the properties of the selected widget, as well as a field under the key
//! "option" that will indicate which widget is being displayed.
QJsonObject DisjunctionConfigWidget::GetValues() const
{
   QJsonObject group{};
   group[cOPTION]              = mComboBoxPtr->currentData().toString();
   ConfigWidget* currentWidget = static_cast<ConfigWidget*>(mStackedWidgetPtr->currentWidget());
   ConfigData::Merge(group, currentWidget->GetValues());

   QJsonObject retval{};
   retval[mName] = group;
   return retval;
}

//! Set the values displayed by this widget.
//!
//! This will set the selected widget and populate the provided @p aData into that
//! widget. This will return true if the values were successfully set.
bool DisjunctionConfigWidget::SetValues(const QJsonObject& aData)
{
   bool retval{false};

   auto iter = aData.constFind(mName);
   if (iter != aData.constEnd())
   {
      QJsonObject groupData = iter->toObject();
      if (!groupData.empty())
      {
         auto optionIter = groupData.constFind(cOPTION);
         if (optionIter != groupData.constEnd())
         {
            QString option = optionIter->toString();
            int     idx    = mComboBoxPtr->findData(option);
            if (idx != -1)
            {
               mComboBoxPtr->setCurrentIndex(idx);
               // NOTE: From the connection, that should have set the stacked widget
               // to be current as well.
               ConfigWidget* currentWidgetPtr = static_cast<ConfigWidget*>(mStackedWidgetPtr->currentWidget());
               retval                         = currentWidgetPtr->SetValues(groupData);
            }
         }
      }
   }

   return retval;
}

//! Clear the values in this widget.
void DisjunctionConfigWidget::ClearValues()
{
   mComboBoxPtr->setCurrentIndex(0);
   for (auto iter = mProperties.begin(); iter != mProperties.end(); ++iter)
   {
      (*iter)->ClearValues();
   }
}

//! Return the type of this ConfigWidget.
QString DisjunctionConfigWidget::Type()
{
   return cDISJUNCTIONTYPE;
}

bool DisjunctionConfigWidget::ValidateObject(const QJsonObject& aConfiguration)
{
   return aConfiguration.constFind(cTYPE)->toString() == cDISJUNCTIONTYPE && aConfiguration.constFind(cNAME)->isString() &&
          aConfiguration.constFind(cDISPLAY)->isString() && aConfiguration.constFind(cPROPERTIES)->isArray();
}

//! Change the panel selection.
//!
//! This will change which option is displayed by this widget. In some use cases,
//! the properties in one panel are not wholly distinct from the options in anther.
//! In those cases, any overlapping properties from the previous panel are carried
//! over into the newly selected panel.
void DisjunctionConfigWidget::ChangePanel(int aIndex)
{
   // Get the current values.
   QJsonObject prevValues = GetValues();

   // Switch panels.
   mStackedWidgetPtr->currentWidget()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
   mStackedWidgetPtr->setCurrentIndex(aIndex);
   mStackedWidgetPtr->currentWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
   mStackedWidgetPtr->adjustSize();
   adjustSize();

   // Set relevant values in new panel.
   auto iter = prevValues.constFind(mName);
   if (iter != prevValues.constEnd())
   {
      QJsonObject   values           = iter->toObject();
      ConfigWidget* currentWidgetPtr = static_cast<ConfigWidget*>(mStackedWidgetPtr->currentWidget());
      currentWidgetPtr->SetValues(values);
   }
}

} // namespace spaceg

} // namespace wsfg
