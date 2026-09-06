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

#ifndef ASTROLABECONFIG_HPP
#define ASTROLABECONFIG_HPP

#include "wsf_spaceg_export.h"

#include <sstream>

#include <QJsonArray>
#include <QJsonDocument>

#include "ConfigWidgetFactory.hpp"

namespace wsfg
{

namespace spaceg
{

//! This class handles the configuration of the Astrolabe tool.
//!
//! The configuration file includes not only the number of event types,
//! but also the specifications of the UI elements that are used to
//! display and modify the event data. For a description of the format
//! of the configuration file, please see the Wizard Space Tools
//! documentation.
class WSF_SPACEG_EXPORT AstrolabeConfig
{
public:
   explicit AstrolabeConfig(bool aVirtual);
   ~AstrolabeConfig() = default;

   bool        IsValid() const { return mValid; }
   std::string GetStatusMessage() const { return mStatusMessage.str(); }

   int     NumEventTypes() const;
   QString EventName(int aIndex) const;
   QString EventDisplay(int aIndex) const;
   bool    EventIsAddable(int aIndex) const;
   bool    EventIsTransformationTarget(int aIndex) const;

   ConfigWidget* CreateEventConfigWidget(int aIndex) const;

   static QJsonObject DefaultEventProperties(const QString& aEventName);

private:
   void          SetupTypeFactory(const QJsonObject& aConfig);
   void          SetupConfig(const QJsonObject& aConfig);
   void          ValidateConfig() const;
   void          ValidateProperty(const QJsonObject& aObject, const QString& aKey, int aIndex) const;
   QJsonDocument GetConfigDocument() const;

   ConfigWidgetFactory       mFactory;
   QJsonArray                mConfig;
   mutable std::stringstream mStatusMessage;
   mutable bool              mValid;
};

} // namespace spaceg

} // namespace wsfg

#endif // ASTROLABECONFIG_HPP
