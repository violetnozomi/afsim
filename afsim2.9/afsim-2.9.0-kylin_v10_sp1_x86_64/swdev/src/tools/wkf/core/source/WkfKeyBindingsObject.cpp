// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "WkfKeyBindingsObject.hpp"

#include <QKeySequence>
#include <QSettings>

#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlugin.hpp"
#include "WkfPluginManager.hpp"
#include "WkfStandardActionsObject.hpp"

wkf::KeyBindingsObject::KeyBindingsObject(QObject* parent)
   : PrefObjectT<KeyBindingsData>(parent, cNAME)
   , mStandardActions(new wkf::StandardActionsObject())
{
   connect(&wkfEnv,
           &Environment::Initialize,
           this,
           [this]()
           {
              // Get the list of all the shortcuts from plugins
              auto plugins = wkfEnv.GetPluginManager()->GetLoadedPlugins();
              for (const auto& p : plugins)
              {
                 AddActions(p.second->GetActions(), p.second->GetName());
              }
           });

   // Get the list of all the shortcuts from standard actions
   AddActions(mStandardActions->GetActions(), "Standard");
}

void wkf::KeyBindingsObject::AddAction(Action* aAction, const QString& aPluginName)
{
   if (!ActionExist(aAction))
   {
      // Concatenate the Plugin Name and Action name together to ensure a unique name.
      aAction->SetUniqueName(QString("%1.%2").arg(aPluginName, aAction->text()).remove(' '));
      mActionList.append(aAction);
      connect(aAction, &Action::KeySequenceChanged, this, &KeyBindingsObject::KeySequenceChangedCB);
   }
}

void wkf::KeyBindingsObject::AddActions(const QList<Action*>& aActionList, const QString& aPluginName)
{
   for (auto& i : aActionList)
   {
      AddAction(i, aPluginName);
   }
}

void wkf::KeyBindingsObject::RemoveActions(const QList<Action*>& aActionList)
{
   for (const auto& i : aActionList)
   {
      RemoveAction(i);
   }
}

void wkf::KeyBindingsObject::RemoveAction(const Action* aAction)
{
   QList<Action*>::iterator iter = FindAction(aAction);
   if (iter != mActionList.end())
   {
      mActionList.erase(iter);
   }
}

//! @details the default data for the key bindings PrefObject is nothing
//! so, the key bindings PrefObject stores the changes in the data struct.
//! this causes problems when the user closes and reopens the Preferences window
//! because there are no changes stored.  the parent RestoreDefaults just assigns
//! the default data to the current data, which does nothing in the case of the
//! key bindings PrefObject.  this override of the RestoreDefaults takes care of the
//! problem by checking each key binding individually and resetting if necessary.
//! It also calls the parent RestoreDefaults function at the end to reset
//! any changes made to the key bindings in the current instance of the Preferences dialog.
void wkf::KeyBindingsObject::RestoreDefaults()
{
   // loop over the actions to see if they differ from the default
   for (auto actionPtr : mActionList)
   {
      // revert the action if different from the default
      if (actionPtr->shortcut() != actionPtr->DefaultKeySequence())
      {
         actionPtr->setShortcut(actionPtr->DefaultKeySequence());
      }
   }

   // call the parent RestoreDefault function
   BasePrefObject::RestoreDefaults();
}

void wkf::KeyBindingsObject::UpdateActionKeyBindings(const QList<Action*>& aActionList)
{
   const QSettings& settings = wkfEnv.GetUserSettingsFile();
   for (auto& i : aActionList)
   {
      i->setShortcut(settings.value(i->GetUniqueName(), i->DefaultKeySequence()).toString());
   }
}

QList<wkf::Action*> wkf::KeyBindingsObject::GetActionsByKeySequence(const QKeySequence& aKeySequence) const
{
   QList<Action*> retVal;
   for (auto& i : mActionList)
   {
      if (i->shortcut() == aKeySequence)
      {
         retVal.push_back(i);
      }
   }
   return retVal;
}

wkf::Action* wkf::KeyBindingsObject::GetActionByUniqueName(const QString& aText) const
{
   for (const auto& i : mActionList)
   {
      if (i->GetUniqueName() == aText)
      {
         return i;
      }
   }
   return nullptr;
}

void wkf::KeyBindingsObject::Apply()
{
   for (auto& i : mCurrentPrefs.mShortcutMap.toStdMap())
   {
      Action* action = GetActionByUniqueName(i.first);
      if (action != nullptr)
      {
         action->setShortcut(i.second);
      }
   }
   mCurrentPrefs.mShortcutMap.clear();
}

void wkf::KeyBindingsObject::SaveSettingsP(QSettings& aSettings) const
{
   for (const Action* iter : mActionList)
   {
      SaveKeyBinding(aSettings, iter);
   }
}

wkf::KeyBindingsData wkf::KeyBindingsObject::ReadSettings(QSettings& aSettings) const
{
   for (Action* iter : mActionList)
   {
      iter->setShortcut(aSettings.value(iter->GetUniqueName(), iter->DefaultKeySequence()).toString());
   }
   return mCurrentPrefs;
}

void wkf::KeyBindingsObject::KeySequenceChangedCB(const Action* aAction)
{
   SaveKeyBinding(aAction);
}

bool wkf::KeyBindingsObject::ActionExist(const Action* aAction)
{
   return (FindAction(aAction) != mActionList.end());
}

void wkf::KeyBindingsObject::SaveKeyBinding(const Action* aAction) const
{
   SaveKeyBinding(wkfEnv.GetUserSettingsFile(), aAction);
}

void wkf::KeyBindingsObject::SaveKeyBinding(QSettings& aSettings, const Action* aAction) const
{
   // Only store key-bindings that are different than the default key-binding.
   if (aAction->shortcut() != aAction->DefaultKeySequence())
   {
      aSettings.setValue(aAction->GetUniqueName(), aAction->shortcut());
   }
   else
   {
      aSettings.remove(aAction->GetUniqueName());
   }
}

QList<wkf::Action*>::iterator wkf::KeyBindingsObject::FindAction(const Action* aAction)
{
   for (QList<Action*>::iterator iter = mActionList.begin(); iter != mActionList.end(); ++iter)
   {
      if ((*iter) == aAction)
      {
         return iter;
      }
   }
   return mActionList.end();
}
