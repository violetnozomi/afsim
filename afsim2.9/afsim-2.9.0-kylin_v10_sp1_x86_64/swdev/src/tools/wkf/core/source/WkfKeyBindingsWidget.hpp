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

#ifndef WKKEYBINDINGSWIDGET_HPP
#define WKKEYBINDINGSWIDGET_HPP

#include <QKeySequence>
#include <QString>
#include <QTreeWidgetItem>

#include "WkfKeyBindingsObject.hpp"
#include "WkfPrefWidget.hpp"
#include "ui_WkfKeyBindingsWidget.h"

namespace wkf
{
class Action;

class KeyBindingsWidget : public PrefWidgetT<KeyBindingsObject>
{
   Q_OBJECT

public:
   KeyBindingsWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   ~KeyBindingsWidget() override = default;

   QString GetCategoryHint() const override { return "Application"; }

   //! Button types used in the widget
   enum ShortcutEditorButtonType
   {
      cRESET,
      cCLEAR,
      cASSIGN
   };

private:
   void ReadPreferenceData(const KeyBindingsData& aPrefData) override;

   void CurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

   void showEvent(QShowEvent*) override;
   void WritePreferenceData(KeyBindingsData& aPrefData) override;

   void PopulateShortcut(Action* aActionPtr, QTreeWidgetItem* aItemPtr);

   void UpdateDisplay();
   void UpdateAction(ShortcutEditorButtonType aButton, Action* aActionPtr);
   void UpdateShortcut(ShortcutEditorButtonType aButton);

   bool IsActionGlobal(const Action* aAction) const;

   QKeySequence GetKeySequence(Action* aAction) const;

   QStringList GetActionsByKeySequence(const QKeySequence& aSequence) const;

   Ui::WkfKeyBindingsWidget mUi;

   QMap<QString, QKeySequence> mPendingKeyBindings;
};
} // namespace wkf
#endif // WKKEYBINDINGSWIDGET_HPP
