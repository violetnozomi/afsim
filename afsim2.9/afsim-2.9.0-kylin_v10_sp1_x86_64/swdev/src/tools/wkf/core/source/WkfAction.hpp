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

#ifndef WKFACTION_HPP
#define WKFACTION_HPP

#include "wkf_export.h"

#include <QAction>
#include <QKeySequence>

namespace wkf
{
// Action is derived from QAction and stores a default QKeySequence.
//  The QKeySequence can be changed, cleared, and reverted to default.
//  Signals are emitted whenever the QKeySequence changes.
class WKF_EXPORT Action : public QAction
{
   Q_OBJECT
public:
   Action(const QString&      aText,
          QObject*            aParent,
          QKeySequence        aKeySequence = QKeySequence(),
          Qt::ShortcutContext aContext     = Qt::WindowShortcut)
      : Action(QIcon(), aText, aParent, aKeySequence, aContext)
   {
   }

   Action(const QString& aText, QObject* aParent, Qt::ShortcutContext aContext)
      : Action(QIcon(), aText, aParent, QKeySequence(), aContext)
   {
   }

   Action(const QIcon&        aIcon,
          const QString&      aText,
          QObject*            aParent,
          QKeySequence        aKeySequence = QKeySequence(),
          Qt::ShortcutContext aContext     = Qt::WindowShortcut);

   void ClearKeySequence();

   void RevertKeySequence();

   void setShortcuts(const QList<QKeySequence>& aShortCuts);

   void setShortcut(const QKeySequence& aShortCut);

   void setShortcuts(QKeySequence::StandardKey aKey);

   QKeySequence DefaultKeySequence() const { return mDefaultKeySequence; }

   void           SetUniqueName(const QString& aUniqueName) { mUniqueName = aUniqueName; }
   const QString& GetUniqueName() const { return mUniqueName; }

signals:
   void KeySequenceChanged(const Action* aAction);

private:
   // Checks if the action has any shortcuts and formats tooltip correctly
   void ChangeToolTip();

   QString      mUniqueName;
   QKeySequence mDefaultKeySequence;
};
} // namespace wkf
#endif
