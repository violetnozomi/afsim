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
#include "WkfPlugin.hpp"

#include "UtQtMemory.hpp"
#include "WkfEnvironment.hpp"
#include "WkfExceptionMessage.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatformOptionsDockWidget.hpp"
#include "WkfScenario.hpp"
#include "WkfSimpleOptionHistoryManager.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::Plugin::Plugin(const QString&        aPluginName,
                    const size_t          aUniqueId,
                    const QFlags<Roles>&  aRoles,
                    OptionHistoryManager* aOptionHistoryManagerPtr)
   : QObject(nullptr)
   , mPluginName(aPluginName)
   , mUniqueId(aUniqueId)
   , mRoles(aRoles)
   , mShowIndividualOptions(true)
   , mOptionHistoryManagerPtr(aOptionHistoryManagerPtr)
{
   if (!mOptionHistoryManagerPtr)
   {
      mOptionHistoryManagerPtr = new SimpleOptionHistoryManager(mPluginName, this);
   }
   connect(&wkfEnv, &Environment::UpdateFrame, this, WKF_EXCEPTION_HANDLER(Plugin, GuiUpdate, mPluginName));

   mCallbacks.Add(Observer::LoadSettings.Connect(WKF_EXCEPTION_HANDLER(Plugin, LoadSettings, mPluginName)));
   mCallbacks.Add(Observer::SaveSettings.Connect(WKF_EXCEPTION_HANDLER(Plugin, SaveSettings, mPluginName)));
   connect(mOptionHistoryManagerPtr, &OptionHistoryManager::SetPlatformOptionState, this, &wkf::Plugin::SetPlatformOptionState);
   connect(mOptionHistoryManagerPtr, &OptionHistoryManager::ResetOptionStates, this, &wkf::Plugin::ResetOptionStates);
}

void wkf::Plugin::SetOptionHistoryManager(wkf::OptionHistoryManager* aOptionHistoryManager)
{
   if (mOptionHistoryManagerPtr)
   {
      delete mOptionHistoryManagerPtr;
   }
   mOptionHistoryManagerPtr = aOptionHistoryManager;
   connect(mOptionHistoryManagerPtr, &OptionHistoryManager::SetPlatformOptionState, this, &wkf::Plugin::SetPlatformOptionState);
   connect(mOptionHistoryManagerPtr, &OptionHistoryManager::ResetOptionStates, this, &wkf::Plugin::ResetOptionStates);
}

// when something is changed on the platform options menu, this will record it as a rule to be applied
// to any entities that get added later.
void wkf::Plugin::UpdateOptions(int aOptionId, bool aState, wkf::PlatformGrouping aGrouping)
{
   mOptionHistoryManagerPtr->UpdateOptionsRulesList(aOptionId, aState, aGrouping);
}

QTreeWidgetItem* wkf::Plugin::RegisterOption(QTreeWidgetItem* aParentPtr,
                                             const QString&   aString,
                                             const QString&   aOptName,
                                             bool             aIndividual,
                                             bool             aInitCheckState,
                                             bool             aSetDefaultTrue)
{
   QString name  = aOptName.isEmpty() ? aString : aOptName;
   int     ptype = -1;
   if (aParentPtr)
   {
      ptype = aParentPtr->type();
   }
   int optionId = mOptionHistoryManagerPtr->AddOption(name, ptype);
   if (aSetDefaultTrue)
   {
      mOptionHistoryManagerPtr->DefaultOptionTrue(name);
   }

   bool oldState = false;
   if (aParentPtr)
   {
      if (aParentPtr->treeWidget())
      {
         oldState = aParentPtr->treeWidget()->blockSignals(true);
      }
   }
   auto* item = ut::qt::make_qt_ptr<QTreeWidgetItem>(aParentPtr, QStringList{aString}, optionId);
   item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate);
   item->setCheckState(0, aInitCheckState ? Qt::Checked : Qt::Unchecked);
   if (aParentPtr)
   {
      if (aParentPtr->treeWidget())
      {
         aParentPtr->treeWidget()->blockSignals(oldState);
      }
   }

   if (!aParentPtr)
   {
      if (!aIndividual)
      {
         mPlatformOptions.push_back(item);
      }
      else
      {
         mIndividualRootOptions.push_back(item);
      }
   }
   if (aIndividual)
   {
      mIndividualOptions.push_front(item); // push front so that we clean up in an appropriate order
      mIndividualOptionIds.push_back(optionId);
   }
   return item;
}

QTreeWidgetItem* wkf::Plugin::RegisterOptionGroup(QTreeWidgetItem* aParentPtr, const QString& aString, bool aIndividual)
{
   if (aParentPtr == nullptr)
   {
      if (!aIndividual)
      {
         for (auto& p : mPlatformOptions)
         {
            if (p->text(0) == aString)
            {
               return p;
            }
         }
      }
      else
      {
         for (auto& p : mIndividualRootOptions)
         {
            if (p->text(0) == aString)
            {
               return p;
            }
         }
      }
   }
   else
   {
      for (int i = 0; i < aParentPtr->childCount(); ++i)
      {
         QTreeWidgetItem* child = aParentPtr->child(i);
         if (child->text(0) == aString)
         {
            return child;
         }
      }
   }
   auto* item = ut::qt::make_qt_ptr<QTreeWidgetItem>(aParentPtr, QStringList{aString}, -1);
   if (!aParentPtr)
   {
      if (!aIndividual)
      {
         mPlatformOptions.push_back(item);
      }
      else
      {
         mIndividualRootOptions.push_back(item);
      }
   }
   if (aIndividual)
   {
      mIndividualOptions.push_front(item); // push front so that we clean up in an appropriate order
   }
   return item;
}

QList<QTreeWidgetItem*> wkf::Plugin::GetPlatformOptions(const std::string& aStringId)
{
   ClearIndividualOptions();
   RegisterIndividualOptions(aStringId);

   QList<QTreeWidgetItem*> opts = mPlatformOptions;
   opts.append(mIndividualRootOptions);
   return opts;
}

void wkf::Plugin::ClearIndividualOptions()
{
   mIndividualRootOptions.clear();
   for (auto it : mIndividualOptions)
   {
      QTreeWidgetItem* item = it;
      delete item;
   }
   mIndividualOptions.clear();
}

void wkf::Plugin::ShowIndividualOptions(bool aState)
{
   mShowIndividualOptions = aState;
   for (auto it : mIndividualOptions)
   {
      QTreeWidgetItem* item = it;
      item->setHidden(!aState);
   }
}

const QString wkf::Plugin::OptionName(int aOptionId) const
{
   return mOptionHistoryManagerPtr->OptionName(aOptionId);
}
