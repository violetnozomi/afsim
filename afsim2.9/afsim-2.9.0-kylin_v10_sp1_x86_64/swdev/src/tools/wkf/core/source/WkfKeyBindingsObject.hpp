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

#ifndef WKFKEYBINDINGSOBJECT_HPP
#define WKFKEYBINDINGSOBJECT_HPP

#include <QKeySequence>
#include <QList>
class QSettings;
#include "wkf_export.h"

#include <QString>

#include "WkfPrefObject.hpp"

namespace wkf
{
class Action;
class StandardActionsObject;
struct KeyBindingsData
{
   QMap<QString, QKeySequence> mShortcutMap; // Used to store pending key bindings that have not been applied; default is empty
};

class WKF_EXPORT KeyBindingsObject : public PrefObjectT<KeyBindingsData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "KeyBindingsObject";

   KeyBindingsObject(QObject* parent = nullptr);
   ~KeyBindingsObject() override = default;

   // Return all Actions stored in the KeyBindingObject
   const QList<Action*> GetActions() const { return mActionList; }
   // Returns the list of all actions that have the specified KeySequence
   QList<Action*> GetActionsByKeySequence(const QKeySequence& aKeySequence) const;
   // Returns the action that has the specified name
   Action* GetActionByUniqueName(const QString& aText) const;

   void AddActions(const QList<Action*>& aActionList, const QString& aPluginName);
   void RemoveActions(const QList<Action*>& aActionList);
   //! restores the default key bindings
   void RestoreDefaults() override;

   // Used to handle settings being loaded prior to certain actions being created. The owner of the actions will call
   // this to load in the relevant settings.
   void UpdateActionKeyBindings(const QList<Action*>& aActionList);

private:
   void AddAction(Action* aAction, const QString& aPluginName);
   void RemoveAction(const Action* aAction);

   void KeySequenceChangedCB(const Action* aAction);

   void            Apply() override;
   void            SaveSettingsP(QSettings& aSettings) const override;
   KeyBindingsData ReadSettings(QSettings& aSettings) const override;

   bool ActionExist(const Action* aAction);
   void SaveKeyBinding(const Action* aAction) const;
   void SaveKeyBinding(QSettings& aSettings, const Action* aAction) const;

   QList<Action*>::iterator FindAction(const Action* aAction);

   QList<Action*>         mActionList;
   StandardActionsObject* mStandardActions;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::KeyBindingsData)
#endif
