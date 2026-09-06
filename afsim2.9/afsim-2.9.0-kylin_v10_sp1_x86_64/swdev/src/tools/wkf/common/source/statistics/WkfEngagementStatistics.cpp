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

#include "WkfEngagementStatistics.hpp"

#include <QMenu>

#include "WkfAbstractTableModel.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "selectors/WkfGroupingSelector.hpp"

wkf::EngagementStatistics::EngagementStatistics(QWidget* aParent)
   : QDialog(aParent)
   , mPlotMode(0)
{
   mItemModel = new EngagementTableModel(this);
   mItemModel->DefineColumn("Time");
   mItemModel->DefineColumn("End Time");
   mItemModel->DefineColumn("Attacker", -1, 2);
   mItemModel->DefineColumn("Target", -1, 2);
   mItemModel->DefineColumn("Intended Target", -1, 2);
   mItemModel->DefineColumn("Weapon", -1, 0);
   mItemModel->DefineColumn("Track Id", -1, 0);
   mItemModel->DefineColumn("Miss dist.", UtUnits::cLENGTH);
   mItemModel->DefineColumn("Result", -1, 2);
   mItemModel->DefineColumn("Ext. result", -1, 0);
   mUi.setupUi(this);
   mUi.mTablePlot->ShowColumn(0, true);
   mUi.mTablePlot->ShowColumn(1, true);
   mUi.mTablePlot->ShowColumn(2, true);
   mUi.mTablePlot->ShowColumn(3, true);
   mUi.mTablePlot->ShowColumn(4, true);
   mUi.mTablePlot->ShowColumn(5, true);
   mUi.mTablePlot->ShowColumn(6, true);
   mUi.mTablePlot->ShowColumn(7, true);
   mUi.mTablePlot->ShowColumn(8, true);
   mUi.mTablePlot->ShowColumn(9, true);

   mUi.mAttacker->setText("Platform Type: WSF_PLATFORM");
   mUi.mTarget->setText("Platform Type: WSF_PLATFORM");
   mAttacker = wkf::PlatformGrouping();
   mTarget   = wkf::PlatformGrouping();

   connect(mUi.mAttacker, &GroupingSelector::GroupingSelected, this, &EngagementStatistics::SelectAttacker);
   connect(mUi.mTarget, &GroupingSelector::GroupingSelected, this, &EngagementStatistics::SelectTarget);
   connect(mUi.mTablePlot, &UtQtTablePlotWidget::itemClicked, this, &EngagementStatistics::ItemClicked);
   mUi.mTablePlot->SuppressNormalContextMenus();
   connect(mUi.mTablePlot, &UtQtTablePlotWidget::tableContext, this, &EngagementStatistics::ContextMenuEvent);
   mUi.mTablePlot->SetModel(mItemModel);
   mUi.mTablePlot->AddPlot(8);
   emit FilterChanged();
}

wkf::EngagementStatistics::~EngagementStatistics()
{
   delete mItemModel;
}

bool wkf::EngagementStatistics::InterestedIn(unsigned int aAttackerId, unsigned int aTargetId)
{
   return (MatchGrouping(aAttackerId, mAttacker) && MatchGrouping(aTargetId, mTarget));
}

bool wkf::EngagementStatistics::MatchGrouping(unsigned int aPlatformId, const wkf::PlatformGrouping& aGrouping)
{
   if (aGrouping.Type() == PlatformGrouping::eIndividual)
   {
      Scenario* scenario = vaEnv.GetStandardScenario();
      if (scenario)
      {
         Platform* platform = scenario->FindPlatformByIndex(aPlatformId);
         if (platform)
         {
            return platform->GetName() == aGrouping.Name();
         }
      }
   }
   else
   {
      const std::map<wkf::PlatformGrouping, std::set<int>>& groupings = wkfEnv.GetGroupings();
      auto                                                  ait       = groupings.find(aGrouping);
      if (ait != groupings.end())
      {
         if (ait->second.find(aPlatformId) != ait->second.end())
         {
            return true;
         }
      }
   }
   return false;
}


void wkf::EngagementStatistics::AddWeaponFire(float              aSimTime,
                                              unsigned int       aFiringPlatformId,
                                              unsigned int       aTargetPlatformId,
                                              const std::string& aIntendedTargetName,
                                              unsigned int       aWeaponPlatformIndex,
                                              int                aLocalTrackNumber,
                                              const std::string& aLocalTrackOwner,
                                              const QString&     aAttackName,
                                              const QString&     aTargetName,
                                              const QString&     aWeaponName)
{
   mItemModel->AddWeaponFire(aSimTime,
                             aFiringPlatformId,
                             aTargetPlatformId,
                             aIntendedTargetName,
                             aWeaponPlatformIndex,
                             aLocalTrackNumber,
                             aLocalTrackOwner,
                             aAttackName,
                             aTargetName,
                             aWeaponName);

   mUi.mEngagementsLabel->setText(QString("Engagements(") + QString::number(mItemModel->rowCount(QModelIndex())) +
                                  QString(")"));
}

void wkf::EngagementStatistics::AddWeaponTerminate(float              aSimTime,
                                                   unsigned int       aFiringPlatformId,
                                                   unsigned int       aTargetPlatformId,
                                                   const std::string& aIntendedTargetName,
                                                   unsigned int       aWeaponPlatformIndex,
                                                   char               aResult,
                                                   const std::string& aExtendedResult,
                                                   float              aMissDistance,
                                                   const QString&     aAttackName,
                                                   const QString&     aTargetName,
                                                   const QString&     aWeaponName)
{
   mItemModel->AddWeaponTerminate(aSimTime,
                                  aFiringPlatformId,
                                  aTargetPlatformId,
                                  aIntendedTargetName,
                                  aWeaponPlatformIndex,
                                  aResult,
                                  aExtendedResult,
                                  aMissDistance,
                                  aAttackName,
                                  aTargetName,
                                  aWeaponName);

   mUi.mEngagementsLabel->setText(QString("Engagements(") + QString::number(mItemModel->rowCount(QModelIndex())) +
                                  QString(")"));
}

void wkf::EngagementStatistics::ItemClicked(const QModelIndex& aIndex)
{
   if ((aIndex.column() == 2) || (aIndex.column() == 3) || (aIndex.column() == 4) || (aIndex.column() == 5))
   {
      vespa::VaViewer* viewer = vaEnv.GetStandardViewer();
      if (viewer != nullptr)
      {
         QString          name   = mItemModel->data(aIndex, Qt::DisplayRole).toString();
         vespa::VaEntity* entity = vaEnv.FindEntity(name.toStdString());
         if (entity != nullptr)
         {
            viewer->ClearSelection();
            entity->SetSelected(viewer, true);
         }
      }
   }
}

void wkf::EngagementStatistics::ContextMenuEvent(const QModelIndex& aIndex, const QPoint& aPoint)
{
   QMenu menu;
   if ((aIndex.column() == 2) || (aIndex.column() == 3) || (aIndex.column() == 4) || (aIndex.column() == 5))
   {
      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         wkf::Platform* plat = scenarioPtr->FindPlatformByName(aIndex.data(Qt::DisplayRole).toString());
         if (plat)
         {
            wkfEnv.BuildEntityContextMenu(&menu, plat);
            menu.addSeparator();
         }
      }
   }
   else
   {
   }
   mUi.mTablePlot->AddActions(aIndex.column(), &menu);

   QStringList strList;
   for (int i = 0; i < mItemModel->columnCount(QModelIndex()); ++i)
   {
      strList.push_back(mItemModel->data(mItemModel->index(aIndex.row(), i), Qt::DisplayRole).toString());
   }
   emit BuildRowContextMenu(&menu, strList);
   menu.exec(aPoint);
}

void wkf::EngagementStatistics::SelectAttacker(const wkf::PlatformGrouping& aAttacker)
{
   mAttacker = aAttacker;
   mItemModel->Clear();
   emit FilterChanged();
}

void wkf::EngagementStatistics::SelectTarget(const wkf::PlatformGrouping& aTarget)
{
   mTarget = aTarget;
   mItemModel->Clear();
   emit FilterChanged();
}

QVariant wkf::EngagementStatistics::EngagementTableModel::data(const QModelIndex& aIndex, int aRole) const
{
   int  r  = aIndex.row();
   int  c  = aIndex.column();
   auto it = mData.find(r);

   if ((it != mData.end()) && (c < columnCount(aIndex)))
   {
      if (aRole == Qt::DisplayRole)
      {
         switch (c)
         {
         case (0):
            if (it->second.time < 0)
            {
               return "";
            }
            return Convert(c, it->second.time);
         case (1):
            if (it->second.endTime < 0)
            {
               return "";
            }
            return Convert(c, it->second.endTime);
         case (2):
            return it->second.attacker;
         case (3):
            return it->second.target;
         case (4):
            return it->second.intendedTarget;
         case (5):
            return it->second.weapon;
         case (6):
            return QString(it->second.trackOwner) + QString(":") + QString::number(it->second.trackId);
         case (7):
            if (it->second.missDist < 0)
            {
               return "";
            }
            return Convert(c, it->second.missDist);
         case (8):
            return it->second.result;
         case (9):
            return it->second.extResult;
         }
      }
      else if (aRole == Qt::UserRole)
      {
         switch (c)
         {
         case (2):
            return it->second.attackerId;
         case (3):
            return it->second.targetId;
         case (5):
            return it->second.weaponId;
         }
      }
   }
   return QVariant();
}

void wkf::EngagementStatistics::EngagementTableModel::AddWeaponFire(float              aSimTime,
                                                                    unsigned int       aFiringPlatformId,
                                                                    unsigned int       aTargetPlatformId,
                                                                    const std::string& aIntendedTargetName,
                                                                    unsigned int       aWeaponPlatformIndex,
                                                                    int                aLocalTrackNumber,
                                                                    const std::string& aLocalTrackOwner,
                                                                    const QString&     aAttackName,
                                                                    const QString&     aTargetName,
                                                                    const QString&     aWeaponName)
{
   int row = mData.size();
   beginInsertRows(QModelIndex(), row, row);
   mData[row].time           = aSimTime;
   mData[row].endTime        = -1;
   mData[row].attacker       = BuildPlatformName(aAttackName, aFiringPlatformId);
   mData[row].attackerId     = aFiringPlatformId;
   mData[row].target         = BuildPlatformName(aTargetName, aTargetPlatformId);
   mData[row].targetId       = aTargetPlatformId;
   mData[row].intendedTarget = aIntendedTargetName.c_str();
   mData[row].weapon         = BuildPlatformName(aWeaponName, aWeaponPlatformIndex);
   mData[row].weaponId       = aWeaponPlatformIndex;
   mData[row].trackOwner     = aLocalTrackOwner.c_str();
   mData[row].trackId        = aLocalTrackNumber;
   mData[row].missDist       = -1;
   mData[row].result         = "";
   mData[row].extResult      = "";
   mUnpairedRows.insert(row);
   endInsertRows();
}

void wkf::EngagementStatistics::EngagementTableModel::AddWeaponTerminate(float              aSimTime,
                                                                         unsigned int       aFiringPlatformId,
                                                                         unsigned int       aTargetPlatformId,
                                                                         const std::string& aIntendedTargetName,
                                                                         unsigned int       aWeaponPlatformIndex,
                                                                         char               aResult,
                                                                         const std::string& aExtendedResult,
                                                                         float              aMissDistance,
                                                                         const QString&     aAttackName,
                                                                         const QString&     aTargetName,
                                                                         const QString&     aWeaponName)
{
   bool inserting = false;
   int  row       = -1;
   for (auto&& it : mUnpairedRows)
   {
      if (aFiringPlatformId == mData[it].attackerId)
      {
         if (aTargetPlatformId == mData[it].targetId)
         {
            if (QString(aIntendedTargetName.c_str()) == mData[it].intendedTarget)
            {
               if (aWeaponPlatformIndex == mData[it].weaponId)
               {
                  row = it;
                  break;
               }
            }
         }
      }
   }
   if (row == -1)
   {
      inserting = true;
      row       = mData.size();
      beginInsertRows(QModelIndex(), row, row);
      mData[row].time           = -1;
      mData[row].attacker       = BuildPlatformName(aAttackName, aFiringPlatformId);
      mData[row].attackerId     = aFiringPlatformId;
      mData[row].target         = BuildPlatformName(aTargetName, aTargetPlatformId);
      mData[row].targetId       = aTargetPlatformId;
      mData[row].intendedTarget = aIntendedTargetName.c_str();
      mData[row].weapon         = BuildPlatformName(aWeaponName, aWeaponPlatformIndex);
      mData[row].weaponId       = aWeaponPlatformIndex;
   }
   mData[row].endTime  = aSimTime;
   mData[row].missDist = aMissDistance;

   QString result;
   switch (aResult)
   {
   case (0):
      result = "in progress";
      break;
   case (1):
      result = "dud";
      break;
   case (2):
      result = "target impact";
      break;
   case (3):
      result = "far away in air";
      break;
   case (4):
      result = "far away above ground";
      break;
   case (5):
      result = "far away ground impact";
      break;
   case (6):
      result = "target prox. air burst";
      break;
   case (7):
      result = "target prox. above ground";
      break;
   case (8):
      result = "target prox. ground impact";
      break;
   case (9):
      result = "part disabled or destroyed";
      break;
   }
   mData[row].result    = result;
   mData[row].extResult = aExtendedResult.c_str();

   if (inserting)
   {
      endInsertRows();
   }
   else
   {
      emit dataChanged(index(row, 0), index(row, 9));
   }
}

void wkf::EngagementStatistics::EngagementTableModel::Clear()
{
   beginResetModel();
   mData.clear();
   mUnpairedRows.clear();
   endResetModel();
}

QString wkf::EngagementStatistics::EngagementTableModel::BuildPlatformName(const QString& aName, unsigned int aPlatformId)
{
   if (!aName.isEmpty())
   {
      return aName;
   }
   else
   {
      return QString("*") + QString::number(aPlatformId);
   }
}
