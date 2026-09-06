// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef WKFPLUGININTERACTIONBASE_HPP
#define WKFPLUGININTERACTIONBASE_HPP

#include "wkf_common_export.h"

#include "WkfAttachmentInteraction.hpp"
#include "WkfInteractionPrefObject.hpp"
#include "WkfInteractionPrefWidget.hpp"
#include "WkfPlugin.hpp"

namespace wkf
{
//! An abstract base class for a plugin to a Wkf application that visualizes interactions between platforms.
//! Contains a preference object and widget, as well as a list of "platform options" tree widget items.
class WKF_COMMON_EXPORT InteractionPluginBase : public Plugin
{
public:
   InteractionPluginBase(const QString& aPluginName, const size_t aUniqueId, const QFlags<Roles>& aRoles = eNO_ROLE);
   ~InteractionPluginBase() override = 0;

   void           ResetOptionStates() override;
   void           SetPlatformOptionState(int aType, bool aState, Platform* aPlatformPtr) override;
   Qt::CheckState GetPlatformOptionState(int aType, const Platform* aPlatformPtr) const override;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   typedef std::pair<std::string, int> InteractionType;

   void RegisterStateCard(const QString& aName, const QString& aIcon, const QString& aGroup);
   void RegisterInteraction(const QString& aName, const QString& aGroup, const QString& aHoverText, const QColor& aColor);
   void AddRegisteredTypes(wkf::AttachmentInteraction& aInteraction);

protected:
   QColor                       GetTypeColor(const std::string& aType) const;
   const InteractionPrefObject* GetPrefObject() const;

   PluginUiPointer<InteractionPrefWidget> mPrefWidgetPtr; // Use guarded pointer because objects will be owned by main window
   std::map<int, InteractionType>         mIdLookup;
   std::map<QString, QString>             mCardMap;
   std::map<QString, std::pair<QString, QColor>> mInteractionTypeMap;
};
} // namespace wkf
#endif
