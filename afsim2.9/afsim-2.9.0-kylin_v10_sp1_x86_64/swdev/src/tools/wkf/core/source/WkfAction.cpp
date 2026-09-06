// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2022 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfAction.hpp"

#include <QAction>
#include <QKeySequence>

namespace wkf
{
Action::Action(const QIcon& aIcon, const QString& aText, QObject* aParent, QKeySequence aKeySequence, Qt::ShortcutContext aContext)
   : QAction(aIcon, aText, aParent)
   , mDefaultKeySequence(aKeySequence)
{
   // Call the base setShortcut() method so that the signal for changed is not emitted on construction.
   QAction::setShortcut(aKeySequence);
   setShortcutContext(aContext);

   ChangeToolTip();

   if (aParent && aParent->isWidgetType())
   {
      qobject_cast<QWidget*>(aParent)->addAction(this);
   }
}

void Action::ClearKeySequence()
{
   setShortcuts(QList<QKeySequence>());
}

void Action::RevertKeySequence()
{
   setShortcut(mDefaultKeySequence);
}

void Action::setShortcuts(const QList<QKeySequence>& aShortCuts)
{
   QAction::setShortcuts(aShortCuts);
   emit KeySequenceChanged(this);
   ChangeToolTip();
}

void Action::setShortcut(const QKeySequence& aShortCut)
{
   bool emitSignal = (shortcut() != aShortCut);
   QAction::setShortcut(aShortCut);

   if (emitSignal)
   {
      emit KeySequenceChanged(this);
      ChangeToolTip();
   }
}

void Action::setShortcuts(QKeySequence::StandardKey aKey)
{
   QAction::setShortcuts(aKey);
   emit KeySequenceChanged(this);
   ChangeToolTip();
}

void Action::ChangeToolTip()
{
   if (shortcut().count() > 0)
   {
      setToolTip(text() + " (" + shortcut().toString() + ")");
   }
   else
   {
      setToolTip(text());
   }
}
} // namespace wkf
