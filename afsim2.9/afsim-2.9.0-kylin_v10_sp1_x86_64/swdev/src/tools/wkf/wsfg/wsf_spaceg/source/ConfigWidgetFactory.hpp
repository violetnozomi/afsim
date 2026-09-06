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

#ifndef CONFIGWIDGETFACTORY_H
#define CONFIGWIDGETFACTORY_H

#include <functional>
#include <map>

#include <QJsonObject>
#include <QString>

namespace wsfg
{

namespace spaceg
{

class ConfigWidget;

//! This class is a factory for ConfigWidget subclasses.
//!
//! This object can produce ConfigWidget subclasses given the "type"
//! of that subclass. This factory can handle both basic types, meaning
//! those ConfigWidget subclasses that are built-in to the system, and
//! user types, meaning those specified in the configuration file
//! composed from basic types.
class ConfigWidgetFactory
{
public:
   explicit ConfigWidgetFactory(bool aVirtual);

   void AddUserTypes(const QJsonObject& aTypes);

   ConfigWidget* Create(QJsonObject& aConfiguration) const;

private:
   bool          ExpandUserTypes(QJsonObject& aConfiguration) const;
   bool          HasType(const QString& aType) const;
   ConfigWidget* Create(const QString& aType) const;
   bool          HasBasicType(const QString& aType) const;
   void          SetupBasicTypes(bool aVirtual);

   std::map<QString, std::function<ConfigWidget*()>> mBasicTypes;
   std::map<QString, QJsonObject>                    mUserTypes;
};

} // namespace spaceg

} // namespace wsfg

#endif // CONFIGWIDGETFACTORY_H
