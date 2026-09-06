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

#include "AstrolabeConfig.hpp"

#include <stdexcept>

#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "UtPath.hpp"

namespace // {anonymous}
{

static const QString cTYPES_KEY{"types"};
static const QString cCONFIG_KEY{"config"};

static const QString cEVENT_TYPE_DISPLAY_KEY{"display"};
static const QString cEVENT_TYPE_NAME_KEY{"name"};
static const QString cEVENT_TYPE_TYPE_KEY{"type"};

static const QString cEVENT_IS_ADDABLE_KEY{"event_is_addable"};
static const QString cEVENT_IS_TRANSFORMATION_TARGET_KEY{"event_is_transformation_target"};

} // namespace

namespace wsfg
{

namespace spaceg
{

AstrolabeConfig::AstrolabeConfig(bool aVirtual)
   : mFactory{aVirtual}
   , mConfig{}
   , mStatusMessage{}
   , mValid{true}
{
   QJsonDocument doc = GetConfigDocument();
   if (IsValid())
   {
      if (doc.isObject())
      {
         QJsonObject obj = doc.object();
         SetupTypeFactory(obj);
         SetupConfig(obj);
      }
      else
      {
         mStatusMessage << "Astrolabe Configuration Error: the configuration document should be an object.\n";
      }
   }
}

//! Return the number of defined event types.
int AstrolabeConfig::NumEventTypes() const
{
   return mConfig.size();
}

//! Return the name of an event type.
//!
//! The returned name matches that of the underlying WsfOrbitalEvent subclass,
//! and is a single token in all uppercase.
//!
//! \param aIndex - An index indicating the event in question.
//! \returns      - The indicated event's name.
QString AstrolabeConfig::EventName(int aIndex) const
{
   // NOTE: We skip safety checks here, because any conditions that might fail will
   // have been checked in ValidateConfig.
   QString     retval{};
   QJsonObject value = mConfig.at(aIndex).toObject();
   auto        iter  = value.constFind(cEVENT_TYPE_NAME_KEY);
   retval            = iter->toString();
   return retval;
}

//! Return an event identifier suitable for display.
//!
//! This method returns an identifier for an event that is suitable for display.
//!
//! \param aIndex - An index indicating the event in question.
//! \returns      - The display identifier for the indicated event.
QString AstrolabeConfig::EventDisplay(int aIndex) const
{
   // NOTE: We skip safety checks here, because any conditions that might fail will
   // have been checked in ValidateConfig.
   QString     retval{};
   QJsonObject value = mConfig.at(aIndex).toObject();
   auto        iter  = value.constFind(cEVENT_TYPE_DISPLAY_KEY);
   retval            = iter->toString();
   return retval;
}

//! Return if the given event is able to be added to the sequence.
//!
//! Currently, only the mission sequence as a whole is not addable.
//! Astrolabe does not support nesting sequences inside the mission sequence
//! for a platform.
//!
//! \param aIndex - An index indicating the event in question.
//! \returns      - true is the event can be added to the sequence; false otherwise.
bool AstrolabeConfig::EventIsAddable(int aIndex) const
{
   bool        retval{true};
   QJsonObject value = mConfig.at(aIndex).toObject();
   auto        iter  = value.constFind(cEVENT_IS_ADDABLE_KEY);
   if (iter != value.constEnd())
   {
      retval = iter->toBool();
   }
   return retval;
}

//! Return if the given event is able to be the target of a transformation operation.
//!
//! Currently, only the mission sequence as a whole is not a valid transformation
//! target. Astrolabe does not support nesting sequences inside the mission sequence
//! for a platform.
//!
//! \param aIndex - An index indicating the event in question.
//! \returns      - true if the given event type can be the target of a transformation; false otherwise.
bool AstrolabeConfig::EventIsTransformationTarget(int aIndex) const
{
   bool        retval{true};
   QJsonObject value = mConfig.at(aIndex).toObject();
   auto        iter  = value.constFind(cEVENT_IS_TRANSFORMATION_TARGET_KEY);
   if (iter != value.constEnd())
   {
      retval = iter->toBool();
   }
   return retval;
}

//! Create a ConfigWidget that can display and edit the indicated event type.
//!
//! \param aIndex - An index indicating the event in question.
//! \returns      - A ConfigWidget instance that can be used to display and edit the event; nullptr if
//                  @p aIndex is out of bounds.
ConfigWidget* AstrolabeConfig::CreateEventConfigWidget(int aIndex) const
{
   ConfigWidget* retvalPtr{nullptr};
   QJsonObject   value = mConfig.at(aIndex).toObject();
   if (!value.isEmpty())
   {
      retvalPtr = mFactory.Create(value);
   }
   return retvalPtr;
}

//! Produce default event properties for the given event type.
//!
//! \param aEventName - the name of the event for which to get default properties.
//! \returns          - a JSON object containing the default properties for the event.
QJsonObject AstrolabeConfig::DefaultEventProperties(const QString& aEventName)
{
   QJsonObject retval{};
   retval["option"] = aEventName.toLower();
   return retval;
}

//! Initialize the type factory from the configuration.
//!
//! This will add the types specified in the configuration file to the type factory
//! so that the event configuration widgets can be constructed. For information about the
//! format of the configuration file, please see the Astrolabe documentation.
void AstrolabeConfig::SetupTypeFactory(const QJsonObject& aConfig)
{
   auto iter = aConfig.constFind(cTYPES_KEY);
   if (iter->isObject())
   {
      QJsonObject typesConfig = iter->toObject();
      mFactory.AddUserTypes(typesConfig);
   }
   else
   {
      mStatusMessage
         << "Astrolabe Configuration Error: the configuration object should contain an object under the key '"
         << cTYPES_KEY.toStdString() << "'.\n";
      mValid = false;
   }
}

//! Read the set of available events from the configuration.
//!
//! This will store the set of event configurations from the provided JSON object
//! for later use. The configuration is an array of events that Astrolabe is able to
//! modify. For more information on the format of the configuration file, please see
//! the Astrolabe documentation.
void AstrolabeConfig::SetupConfig(const QJsonObject& aConfig)
{
   auto iter = aConfig.constFind(cCONFIG_KEY);
   if (iter->isArray())
   {
      mConfig = iter->toArray();
      ValidateConfig();
   }
   else
   {
      mStatusMessage
         << "Astrolabe Configuration Error: the configuration object should contain an array under the key '"
         << cCONFIG_KEY.toStdString() << "'.\n";
      mValid = false;
   }
}

//! Validate the loaded configuration.
//!
//! Failed validation will result in messages being appended to the status log, which
//! is displayed in lieu of Astrolabe when there is a configuration error.
void AstrolabeConfig::ValidateConfig() const
{
   int numEntries = NumEventTypes();
   for (int i = 0; i < numEntries; ++i)
   {
      QJsonObject entry = mConfig.at(i).toObject();
      if (entry.isEmpty())
      {
         mStatusMessage << "Astrolabe Configuration Error: the configuration array should be an array of objects.\n";
         mValid = false;
      }
      else
      {
         ValidateProperty(entry, cEVENT_TYPE_NAME_KEY, i);
         ValidateProperty(entry, cEVENT_TYPE_DISPLAY_KEY, i);
         ValidateProperty(entry, cEVENT_TYPE_TYPE_KEY, i);
      }
   }
}

//! Validate a single property of the configuration.
void AstrolabeConfig::ValidateProperty(const QJsonObject& aObject, const QString& aKey, int aIndex) const
{
   if (!aObject.contains(aKey))
   {
      mStatusMessage << "Astrolabe Configuration Error: object " << aIndex
                     << " of the configuration array does not contain the key '" << aKey.toStdString() << "'.\n";
      mValid = false;
   }
   else if (!aObject.constFind(aKey)->isString())
   {
      mStatusMessage << "Astrolabe Configuration Error: object " << aIndex
                     << " of the configuration array should contain a string under the key '" << aKey.toStdString()
                     << "'.\n";
      mValid = false;
   }
}

//! Return the configuration document.
//!
//! This will load the configuration from file. If there is trouble, this method will
//! return a valid, but empty document. Instead, the error is indicated via this object's
//! valid flag, and through the status log.
QJsonDocument AstrolabeConfig::GetConfigDocument() const
{
   QFile inFile(":/AstrolabeConfig.json");
   inFile.open(QIODevice::ReadOnly | QIODevice::Text);
   QString fileText = inFile.readAll();
   inFile.close();

   QJsonDocument retval = QJsonDocument::fromJson(fileText.toUtf8());
   return retval;
}

} // namespace spaceg

} // namespace wsfg
