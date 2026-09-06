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
#include "WkfKeyBindingsWidget.hpp"

#include <QList>

#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfKeyBindingsObject.hpp"

wkf::KeyBindingsWidget::KeyBindingsWidget(QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/)
   : PrefWidgetT<KeyBindingsObject>(parent, f)
{
   mUi.setupUi(this);

   // Reset, Clear, or Apply buttons
   connect(mUi.assignButton, &QPushButton::clicked, this, [=]() { UpdateShortcut(cASSIGN); });
   connect(mUi.clearButton, &QPushButton::clicked, this, [=]() { UpdateShortcut(cCLEAR); });
   connect(mUi.resetButton, &QPushButton::clicked, this, [=]() { UpdateShortcut(cRESET); });

   // Selection of a row in the tree view
   connect(mUi.treeWidget, &QTreeWidget::currentItemChanged, this, &KeyBindingsWidget::CurrentItemChanged);
}

void wkf::KeyBindingsWidget::ReadPreferenceData(const KeyBindingsData& aPrefData)
{
   // Clear the Pending Changes as we have been requested to update the Pref Widget to match the pref object
   mPendingKeyBindings.clear();
   UpdateDisplay();
}

void wkf::KeyBindingsWidget::UpdateDisplay()
{
   // Clear the model data including headers
   mUi.treeWidget->clear();

   QList<QTreeWidgetItem*> globalItems;
   QList<QTreeWidgetItem*> localItems;

   // Iterate through the action set and show the shortcuts
   const QList<Action*> actionList = mPrefObjectPtr->GetActions();
   for (auto iter : actionList)
   {
      auto* item = new QTreeWidgetItem();
      PopulateShortcut(iter, item);
      if (IsActionGlobal(iter))
      {
         globalItems.push_back(item);
      }
      else
      {
         localItems.push_back(item);
      }
   }

   if (localItems.empty())
   {
      for (auto& i : globalItems)
      {
         mUi.treeWidget->addTopLevelItem(i);
      }
   }
   else
   {
      auto* globalItem = new QTreeWidgetItem({"Global"});
      globalItem->addChildren(globalItems);
      mUi.treeWidget->addTopLevelItem(globalItem);
      auto* localItem = new QTreeWidgetItem({"Local"});
      localItem->addChildren(localItems);
      mUi.treeWidget->addTopLevelItem(localItem);
   }

   mUi.treeWidget->setHeaderLabels({"Command", "Shortcut"});
   // Resize the first column based on the content width
   mUi.treeWidget->expandAll();
   mUi.treeWidget->resizeColumnToContents(0);
}

void wkf::KeyBindingsWidget::CurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
   if (current)
   {
      mUi.keySequenceEdit->setKeySequence(QKeySequence(current->text(1)));
   }
}

void wkf::KeyBindingsWidget::showEvent(QShowEvent*)
{
   // This is necessary because the pref widget isn't constructed until after settings are loaded into pref object
   ReadPreferenceData(mPrefObjectPtr->GetPreferences());
}

void wkf::KeyBindingsWidget::WritePreferenceData(KeyBindingsData& aPrefData)
{
   aPrefData.mShortcutMap = mPendingKeyBindings;
}

void wkf::KeyBindingsWidget::PopulateShortcut(Action* aActionPtr, QTreeWidgetItem* aItemPtr)
{
   aItemPtr->setText(0, aActionPtr->GetUniqueName());

   // Get the shortcut from the action
   QKeySequence shortcut = GetKeySequence(aActionPtr);

   aItemPtr->setText(1, shortcut.toString());

   // If the shortcut is not empty
   if (shortcut != QKeySequence())
   {
      // Check for shared shortcuts, so that we can indicate there are multiple actions bound to same KeySequence
      QStringList actionNames = GetActionsByKeySequence(shortcut);

      // If there are still more than 1 shortcut bound to the KeySequence,
      //  then mark the shortcuts as conflicted: changing the color of the text to red
      if (actionNames.size() > 1)
      {
         // Check to make sure at least one of the action is not a local action
         // Two local actions should not flagged as conflicted key bindings, as they may have different scopes.
         bool atLeastOneActionIsGlobal = false;
         foreach (QString name, actionNames)
         {
            Action* actionPtr        = mPrefObjectPtr->GetActionByUniqueName(name);
            atLeastOneActionIsGlobal = (actionPtr && IsActionGlobal(actionPtr));
            if (atLeastOneActionIsGlobal)
            {
               break;
            }
         }

         if (atLeastOneActionIsGlobal)
         {
            foreach (QString name, actionNames)
            {
               Action* actionPtr = mPrefObjectPtr->GetActionByUniqueName(name);
               // Do not flag shortcuts in the same QActionGroup as conflicted (make the text red)
               if (!aActionPtr->actionGroup() || (aActionPtr->actionGroup() != actionPtr->actionGroup()))
               {
                  QString warning = tr("Shortcut shared by more than one command:\n") + actionNames.join("\n");
                  aItemPtr->setToolTip(1, warning);
                  QColor red(Qt::red);
                  red.setAlpha(192);
                  aItemPtr->setForeground(1, red);
                  break;
               }
            }
         }
      }
   }
}

void wkf::KeyBindingsWidget::UpdateAction(ShortcutEditorButtonType aButton, Action* aActionPtr)
{
   // Perform the update to the action based on the button selected
   switch (aButton)
   {
   case cRESET:
      mPendingKeyBindings[aActionPtr->GetUniqueName()] = aActionPtr->DefaultKeySequence();
      break;
   case cCLEAR:
      mPendingKeyBindings[aActionPtr->GetUniqueName()] = QKeySequence();
      break;
   case cASSIGN:
      mPendingKeyBindings[aActionPtr->GetUniqueName()] = mUi.keySequenceEdit->keySequence();
      break;
   default:
      break;
   }
}

void wkf::KeyBindingsWidget::UpdateShortcut(ShortcutEditorButtonType aButton)
{
   // Get current selection - index and items
   auto* item = mUi.treeWidget->currentItem();
   if (item)
   {
      QString text   = item->text(0);
      Action* action = mPrefObjectPtr->GetActionByUniqueName(text);
      if (action != nullptr)
      {
         // Perform the update
         UpdateAction(aButton, action);

         // Update the display view
         UpdateDisplay();

         // Ignoring the fact that an action can have multiple shortcuts assigned
         mUi.keySequenceEdit->setKeySequence(GetKeySequence(action));
      }

      // Since the treeWidget->currentItem() was destroyed during the UpdateDisplay call, find the
      //  new item that has the same text as the old item, and set that as the currentItem()
      auto items = mUi.treeWidget->findItems(text, Qt::MatchExactly | Qt::MatchRecursive, 0);
      foreach (QTreeWidgetItem* i, items)
      {
         mUi.treeWidget->setCurrentItem(i);
      }
   }
}

bool wkf::KeyBindingsWidget::IsActionGlobal(const Action* aAction) const
{
   return (aAction->shortcutContext() == Qt::WindowShortcut || aAction->shortcutContext() == Qt::ApplicationShortcut);
}

QKeySequence wkf::KeyBindingsWidget::GetKeySequence(Action* aAction) const
{
   // If it is an un-applied change within the GUI show that value instead.
   return mPendingKeyBindings.value(aAction->GetUniqueName(), aAction->shortcut());
}

QStringList wkf::KeyBindingsWidget::GetActionsByKeySequence(const QKeySequence& aSequence) const
{
   QStringList retVal;

   // Get any actions that that the pending Key Sequence for the is aSequence
   for (const auto& iter : mPendingKeyBindings.toStdMap())
   {
      if (iter.second == aSequence)
      {
         // Retrieve the action from the Key Binding Object
         retVal.push_back(iter.first);
      }
   }

   // Get any actions from the Key Binding Object in which the Key Sequence matches.
   QList<Action*> list = mPrefObjectPtr->GetActionsByKeySequence(aSequence);
   for (const auto& i : list)
   {
      // If the shortcut is not pending a change
      if (i->shortcut() == GetKeySequence(i))
      {
         // This action matches the key sequence, add its it name to return value
         retVal.push_back(i->GetUniqueName());
      }
   }

   // It is possible that an action will be in the pending list (has not been applied) and still be located by
   // GetActionsByKeySequence()
   retVal.removeDuplicates();

   return retVal;
}
