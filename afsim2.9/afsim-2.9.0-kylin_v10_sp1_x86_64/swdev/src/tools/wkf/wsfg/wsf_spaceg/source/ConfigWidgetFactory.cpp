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

#include "ConfigWidgetFactory.hpp"

#include <QJsonObject>

#include "BlankConfigWidget.hpp"
#include "BooleanConfigWidget.hpp"
#include "CompositeConfigWidget.hpp"
#include "ConfigWidget.hpp"
#include "DisjunctionConfigWidget.hpp"
#include "EnablerConfigWidget.hpp"
#include "IntegerConfigWidget.hpp"
#include "MultiRealConfigWidget.hpp"
#include "RadioConfigWidget.hpp"
#include "RealConfigWidget.hpp"
#include "SequenceConfigWidget.hpp"
#include "StringConfigWidget.hpp"
#include "TargetSelectionConfigWidget.hpp"


namespace wsfg
{

namespace spaceg
{

ConfigWidgetFactory::ConfigWidgetFactory(bool aVirtual)
   : mUserTypes{}
{
   SetupBasicTypes(aVirtual);
}

//! Add user defined types to this factory.
//!
//! This will take the given @p aTypes and add those to the factory, allowing
//! clients to create ConfigWidgets that conform to the provided specification.
//! The input JSON should be an object with keys giving the defined type name
//! and the value associated with that an object specifying the details of the
//! so named type.
//!
//! \param aTypes - a specification of complex (non-basic) types.
void ConfigWidgetFactory::AddUserTypes(const QJsonObject& aTypes)
{
   for (auto iter = aTypes.begin(); iter != aTypes.end(); ++iter)
   {
      if (iter->isObject() && !HasType(iter.key()))
      {
         mUserTypes[iter.key()] = iter->toObject();
      }
   }
}

//! Create and configure a ConfigWidget.
//!
//! This will create a ConfigWidget of the appropriate type, so long as that type is
//! defined in this factory. This will return a nullptr if the requested type does not
//! exist. The caller takes ownership of the returned object. Note that because this
//! method expands the configuration with user types, @p aConfiguration may be
//! modified by this method.
//!
//! \param aConfiguration - a specification of the widget to create.
//! \return               - the configured widget, or nullptr if unable to create the specified widget.
ConfigWidget* ConfigWidgetFactory::Create(QJsonObject& aConfiguration) const
{
   // Expand user types until all expansions have finished.
   bool tryAnotherExpansion{true};
   while (tryAnotherExpansion)
   {
      tryAnotherExpansion = ExpandUserTypes(aConfiguration);
   }

   ConfigWidget* retvalPtr{nullptr};
   auto          iter = aConfiguration.constFind(QString("type"));
   retvalPtr          = Create(iter->toString());
   if (retvalPtr)
   {
      retvalPtr->Configure(*this, aConfiguration);
   }
   return retvalPtr;
}

//! Expand the given configuration with the specified user types in this factory.
//!
//! This will expand the given @p aConfiguration by adding any properties defined in
//! the specified user type to the configuration. With the exception of the 'type'
//! property, these will not replace any property that is currently part of the
//! @p aConfiguration. User types may be nested, as so the result of this
//! method may be that it must be called again for the same @p aConfiguration.
//!
//! \param aConfiguration - the configuration in which to expand user-defined types.
//! \return               - true if an expansion occurred; false otherwise.
bool ConfigWidgetFactory::ExpandUserTypes(QJsonObject& aConfiguration) const
{
   bool    retval{false};
   QString type = aConfiguration.find(ConfigWidget::cTYPE)->toString();
   auto    iter = mUserTypes.find(type);
   if (iter != mUserTypes.end())
   {
      for (auto prop = iter->second.begin(); prop != iter->second.end(); ++prop)
      {
         // If the key in the user type is "type" or anything that does not already
         // exist in the configuration, then go ahead and copy over the values.
         QString key = prop.key();
         if (key == ConfigWidget::cTYPE || aConfiguration.constFind(key) == aConfiguration.constEnd())
         {
            aConfiguration[key] = prop.value();
         }
      }

      retval = true;
   }
   return retval;
}

//! Determine if the given type is defined in this factory.
bool ConfigWidgetFactory::HasType(const QString& aType) const
{
   return HasBasicType(aType) || (mUserTypes.find(aType) != mUserTypes.end());
}

//! Create a ConfigWidget of the given type.
//!
//! The caller takes ownership of the returned object.
ConfigWidget* ConfigWidgetFactory::Create(const QString& aType) const
{
   ConfigWidget* retvalPtr{nullptr};
   auto          iter = mBasicTypes.find(aType);
   if (iter != mBasicTypes.end())
   {
      retvalPtr = iter->second();
   }
   return retvalPtr;
}

bool ConfigWidgetFactory::HasBasicType(const QString& aType) const
{
   return mBasicTypes.find(aType) != mBasicTypes.end();
}

//! Populate the basic types into this factory.
void ConfigWidgetFactory::SetupBasicTypes(bool aVirtual)
{
   mBasicTypes[IntegerConfigWidget::Type()]         = []() { return new IntegerConfigWidget{}; };
   mBasicTypes[BooleanConfigWidget::Type()]         = []() { return new BooleanConfigWidget{}; };
   mBasicTypes[LengthConfigWidget::Type()]          = []() { return new LengthConfigWidget{}; };
   mBasicTypes[TimeConfigWidget::Type()]            = []() { return new TimeConfigWidget{}; };
   mBasicTypes[SpeedConfigWidget::Type()]           = []() { return new SpeedConfigWidget{}; };
   mBasicTypes[AngleConfigWidget::Type()]           = []() { return new AngleConfigWidget{}; };
   mBasicTypes[AngleRateConfigWidget::Type()]       = []() { return new AngleRateConfigWidget{}; };
   mBasicTypes[RealConfigWidget::Type()]            = []() { return new RealConfigWidget{}; };
   mBasicTypes[CompositeConfigWidget::Type()]       = []() { return new CompositeConfigWidget{}; };
   mBasicTypes[MultiLengthConfigWidget::Type()]     = []() { return new MultiLengthConfigWidget{}; };
   mBasicTypes[StringConfigWidget::Type()]          = []() { return new StringConfigWidget{}; };
   mBasicTypes[EnablerConfigWidget::Type()]         = []() { return new EnablerConfigWidget{}; };
   mBasicTypes[DisjunctionConfigWidget::Type()]     = []() { return new DisjunctionConfigWidget{}; };
   mBasicTypes[SequenceConfigWidget::Type()]        = []() { return new SequenceConfigWidget{}; };
   mBasicTypes[RadioConfigWidget::Type()]           = []() { return new RadioConfigWidget{}; };
   mBasicTypes[BlankConfigWidget::Type()]           = []() { return new BlankConfigWidget{}; };
   mBasicTypes[TargetSelectionConfigWidget::Type()] = [aVirtual]() { return new TargetSelectionConfigWidget{aVirtual}; };
}

} // namespace spaceg

} // namespace wsfg
