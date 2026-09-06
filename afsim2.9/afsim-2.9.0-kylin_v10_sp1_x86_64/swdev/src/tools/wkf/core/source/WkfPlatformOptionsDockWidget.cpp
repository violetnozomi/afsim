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
#include "WkfPlatformOptionsDockWidget.hpp"

#include "WkfEnvironment.hpp"
#include "WkfObserver.hpp"
#include "WkfPlugin.hpp"
#include "WkfPluginManager.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::PlatformOptionsDockWidget::PlatformOptionsDockWidget(QWidget*        aParent /* = nullptr */,
                                                          Qt::WindowFlags aFlags /* = Qt::WindowFlags() */)
   : PlatformContextDockWidget("Platform Options", &Plugin::GetPlatformOptions, aParent, aFlags)
{
   setObjectName("PlatformOptionsDockWidget");
   mUi.treeWidget->SetDragEnabled(false);
   mUi.treeWidget->header()->setVisible(false);

   mCallbacks.Add(wkf::Observer::PlatformConfigured.Connect(&wkf::PlatformOptionsDockWidget::PlatformConfigured, this));
   mCallbacks.Add(wkf::Observer::PlatformPendingDeletion.Connect(&wkf::PlatformOptionsDockWidget::PlatformDeleted, this));
   connect(&wkfEnv, &wkf::Environment::UpdatePlatformOptions, this, &wkf::PlatformOptionsDockWidget::UpdateDisplay);
   connect(&wkfEnv, &wkf::Environment::NewGrouping, this, &wkf::PlatformOptionsDockWidget::NewGrouping);
   connect(&wkfEnv, &wkf::Environment::GroupingChanged, this, &wkf::PlatformOptionsDockWidget::GroupingChanged);
}

bool wkf::PlatformOptionsDockWidget::SetCurrentPlatform(const Platform& aPlatform)
{
   if (PlatformContextDockWidget::SetCurrentPlatform(aPlatform))
   {
      mPlatformIndex = aPlatform.GetIndex();

      RebuildGroupingSet(mPlatformName, mPlatformIndex, true);

      UpdateCheckStates();

      return true;
   }
   return false;
}

void wkf::PlatformOptionsDockWidget::ClearCurrent(bool aExplicit)
{
   mPlatformIndex = 0;

   if (!aExplicit)
   {
      wkf::PlatformContextDockWidget::ClearCurrent();
   }
   else
   {
      mPlatformName.clear();

      PopulateItems();
      RebuildGroupingSet(mPlatformName, mPlatformIndex, false);
      UpdateCheckStates();
   }
}

void wkf::PlatformOptionsDockWidget::ItemChanged(const ChangeInfo& aInfo)
{
   if (aInfo.changedPtr->checkState(0) != Qt::PartiallyChecked)
   {
      Plugin* plugin = GetPluginFromItem(aInfo.changedPtr);
      if (plugin)
      {
         PlatformSet plats;
         GetPlatformSet(plats, mGrouping);
         for (auto& p : plats)
         {
            plugin->SetPlatformOptionState(aInfo.changedPtr->type(), aInfo.changedPtr->checkState(0) == Qt::Checked, p);
         }
         plugin->UpdateOptions(aInfo.changedPtr->type(), aInfo.changedPtr->checkState(0) == Qt::Checked, mGrouping);
      }
   }
}

void wkf::PlatformOptionsDockWidget::GroupIndexChanged(int aIndex)
{
   int i = 0;
   for (auto& it : mGroupingSet)
   {
      if (i == aIndex)
      {
         mGrouping     = it;
         bool oldState = mUi.treeWidget->blockSignals(true);
         for (auto pit : wkfEnv.GetPluginManager()->GetLoadedPlugins())
         {
            pit.second->ShowIndividualOptions(
               (mGrouping.Type() == wkf::PlatformGrouping::eIndividual)); // show or hide based on if we are at an
                                                                          // individual level
         }
         UpdateCheckStates();
         mUi.treeWidget->blockSignals(oldState);
         return;
      }
      ++i;
   }
}

void wkf::PlatformOptionsDockWidget::UpdateCheckStates()
{
   bool sb = mUi.treeWidget->signalsBlocked();
   mUi.treeWidget->blockSignals(true);
   UpdateCheckStates(mUi.treeWidget->invisibleRootItem());
   mUi.treeWidget->blockSignals(sb);
}

void wkf::PlatformOptionsDockWidget::UpdateCheckStates(QTreeWidgetItem* aItem)
{
   for (int i = 0; i < aItem->childCount(); ++i)
   {
      UpdateCheckStates(aItem->child(i));
   }

   if (aItem->type() >= 0)
   {
      Plugin* pluginPtr = GetPluginFromItem(aItem);
      if (pluginPtr)
      {
         PlatformSet plats;
         GetPlatformSet(plats, mGrouping);

         bool all  = true;
         bool none = true;
         for (auto& p : plats)
         {
            Qt::CheckState cs = pluginPtr->GetPlatformOptionState(aItem->type(), p);
            if (cs == Qt::Checked)
            {
               none = false;
            }
            else if (cs == Qt::Unchecked)
            {
               all = false;
            }
            else
            {
               none = false;
               all  = false;
            }
         }

         bool sb = mUi.treeWidget->signalsBlocked();
         mUi.treeWidget->blockSignals(true);
         aItem->setCheckState(0, none ? Qt::Unchecked : (all ? Qt::Checked : Qt::PartiallyChecked));
         mUi.treeWidget->blockSignals(sb);
      }
   }
}

void wkf::PlatformOptionsDockWidget::RebuildGroupingSet(const std::string& aPlatformName,
                                                        int                aPlatformIndex,
                                                        bool               aSelectPlatform)
{
   QString selection = mUi.mGroupSelection->currentText();
   // this routine clears out the internal grouping set and rebuilds it
   if (!aPlatformName.empty())
   {
      // with what groupings the particular platform has from the wkf::Environment
      mGroupingSet = wkfEnv.GetGroupings(aPlatformIndex);
      mGrouping    = wkf::PlatformGrouping(wkf::PlatformGrouping::eIndividual, aPlatformName);
      mGroupingSet.insert(mGrouping);

      if (aSelectPlatform)
      {
         selection = QString::fromStdString(aPlatformName);
      }
   }
   else
   {
      // with the global groupings from the wkf::Environment
      mGroupingSet.clear();
      mGrouping = wkf::PlatformGrouping(); // Default Group: WSF_PLATFORM / "All"
      mGroupingSet.insert(mGrouping);
      auto groupings = wkfEnv.GetGroupings();
      for (const auto& it : groupings)
      {
         if (it.first.Type() == wkf::PlatformGrouping::eSide)
         {
            mGroupingSet.insert(it.first);
         }
      }
   }

   // rebuild the grouping list (combo box)
   RebuildGroupComboBox(selection);
}

void wkf::PlatformOptionsDockWidget::GetPlatformSet(PlatformSet& aSet, PlatformGrouping aGrouping)
{
   if (aGrouping.Type() == wkf::PlatformGrouping::eIndividual)
   {
      auto* plat = dynamic_cast<Platform*>(vaEnv.FindEntity(aGrouping.Name()));
      if (plat)
      {
         aSet.insert(plat);
      }
   }
   else
   {
      auto it = wkfEnv.GetGroupings().find(aGrouping);
      if (it != wkfEnv.GetGroupings().end())
      {
         for (auto& jt : it->second)
         {
            wkf::Scenario* scenPtr = vaEnv.GetStandardScenario();
            if (scenPtr)
            {
               auto* plat = scenPtr->FindPlatformByIndex(jt);
               if (plat)
               {
                  aSet.insert(plat);
               }
            }
         }
      }
   }
}

void wkf::PlatformOptionsDockWidget::PlatformConfigured(Platform* aPlatform)
{
   auto it = wkfEnv.GetGroupings().find(mGrouping);
   if (it != wkfEnv.GetGroupings().end())
   {
      // if the platform is relevant to the current display
      if (it->second.find(aPlatform->GetIndex()) != it->second.end())
      {
         for (int i = 0; i < mUi.treeWidget->topLevelItemCount(); ++i)
         {
            PlatformConfigured(aPlatform, mUi.treeWidget->topLevelItem(i));
         }
      }
   }
}

void wkf::PlatformOptionsDockWidget::PlatformConfigured(Platform* aPlatform, QTreeWidgetItem* aItemPtr)
{
   if (aItemPtr->type() >= 0)
   {
      if (aItemPtr->checkState(0) != Qt::PartiallyChecked)
      {
         Plugin* pluginPtr = GetPluginFromItem(aItemPtr);
         if (pluginPtr)
         {
            Qt::CheckState platformstate = pluginPtr->GetPlatformOptionState(aItemPtr->type(), aPlatform);
            Qt::CheckState checkstate    = aItemPtr->checkState(0);
            if ((platformstate != checkstate) && (checkstate != Qt::PartiallyChecked))
            {
               UpdateCheckStates(aItemPtr);
            }
         }
      }
   }
   for (int i = 0; i < aItemPtr->childCount(); ++i)
   {
      PlatformConfigured(aPlatform, aItemPtr->child(i));
   }
}

void wkf::PlatformOptionsDockWidget::PlatformDeleted(Platform* aPlatform)
{
   auto it = wkfEnv.GetGroupings().find(mGrouping);
   if (it != wkfEnv.GetGroupings().end())
   {
      // if the platform is relevant to the current display
      if (it->second.find(aPlatform->GetIndex()) != it->second.end())
      {
         for (int i = 0; i < mUi.treeWidget->topLevelItemCount(); ++i)
         {
            PlatformDeleted(aPlatform, mUi.treeWidget->topLevelItem(i));
         }
      }
   }
}

void wkf::PlatformOptionsDockWidget::PlatformDeleted(Platform* aPlatform, QTreeWidgetItem* aItemPtr)
{
   if (aItemPtr->type() >= 0)
   {
      if (aItemPtr->checkState(0) == Qt::PartiallyChecked)
      {
         Plugin* pluginPtr = GetPluginFromItem(aItemPtr);
         if (pluginPtr)
         {
            auto it = wkfEnv.GetGroupings().find(mGrouping);
            if (it != wkfEnv.GetGroupings().end())
            {
               bool tfound = false;
               bool ffound = false;
               bool match  = false;
               for (auto jt : it->second)
               {
                  if (jt != static_cast<int>(aPlatform->GetIndex()))
                  {
                     auto* scenarioPtr = vaEnv.GetStandardScenario();
                     if (scenarioPtr)
                     {
                        wkf::Platform* plat = scenarioPtr->FindPlatformByIndex(jt);
                        if (plat)
                        {
                           Qt::CheckState cs = pluginPtr->GetPlatformOptionState(aItemPtr->type(), plat);
                           if (cs == Qt::Checked)
                           {
                              tfound = true;
                              if (ffound)
                              {
                                 match = true;
                                 break;
                              }
                           }
                           else if (cs == Qt::Unchecked)
                           {
                              ffound = true;
                              if (tfound)
                              {
                                 match = true;
                                 break;
                              }
                           }
                           else
                           {
                              match = true;
                              break;
                           }
                        }
                     }
                  }
               }
               if (!match)
               {
                  bool sb = mUi.treeWidget->signalsBlocked();
                  mUi.treeWidget->blockSignals(true);
                  aItemPtr->setCheckState(0, tfound ? Qt::Checked : Qt::Unchecked); // change the state of the check to
                                                                                    // the opposite of platform ptr
                  mUi.treeWidget->blockSignals(sb);
               }
            }
         }
      }
   }
   for (int i = 0; i < aItemPtr->childCount(); ++i)
   {
      PlatformConfigured(aPlatform, aItemPtr->child(i));
   }
}

void wkf::PlatformOptionsDockWidget::UpdateDisplay()
{
   UpdateCheckStates();
}

void wkf::PlatformOptionsDockWidget::NewGrouping(int aType, const QString& aName, int aPlatformIndex)
{
   if ((mPlatformName.empty() && (aType == wkf::PlatformGrouping::eSide)) || (aPlatformIndex == mPlatformIndex))
   {
      if (mGroupingSet.empty())
      {
         ClearCurrent(true); // rebuild the UI, we got something of interest
      }
      else
      {
         RebuildGroupingSet(mPlatformName, mPlatformIndex, false);
      }
   }
}

void wkf::PlatformOptionsDockWidget::GroupingChanged(int aType, const QString& aName, int aPlatformIndex)
{
   if (aPlatformIndex == mPlatformIndex)
   {
      RebuildGroupingSet(mPlatformName, mPlatformIndex, false);
   }
}

void wkf::PlatformOptionsDockWidget::RebuildGroupComboBox(const QString& aGroupText)
{
   mUi.mGroupSelection->clear();

   for (auto& it : mGroupingSet)
   {
      QString label;
      if (it.IsGlobal())
      {
         label = "All";
      }
      else
      {
         if (it.Type() != wkf::PlatformGrouping::eIndividual)
         {
            label = QString::fromStdString(it.TypeString());
            label += ": ";
         }
         label += it.Name().c_str();
      }
      mUi.mGroupSelection->addItem(label);
   }

   // Attempt to select the desired text.
   if (mUi.mGroupSelection->findText(aGroupText) != -1)
   {
      mUi.mGroupSelection->setCurrentText(aGroupText);
   }
   else // Otherwise select "All"
   {
      mUi.mGroupSelection->setCurrentIndex(mUi.mGroupSelection->findText("All"));
   }
}
