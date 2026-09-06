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

#include "WkfGroupingSelector.hpp"

#include <set>

#include "VaEntity.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlatformGrouping.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::GroupingSelector::GroupingSelector(QWidget* aParent)
   : QToolButton(aParent)
   , mGrouping(wkf::PlatformGrouping::eType, "WSF_PLATFORM")
{
   setText("Platform Type: WSF_PLATFORM");
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
   setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
   setArrowType(Qt::DownArrow);

   connect(this, &QToolButton::clicked, this, &GroupingSelector::Activated);
}

void wkf::GroupingSelector::Activated()
{
   GroupingSelectorDialog* catSelector = new GroupingSelectorDialog(true, true, this);
   catSelector->move(mapToGlobal(QPoint(0, height())));
   catSelector->resize(width(), catSelector->height());
   if (catSelector->exec())
   {
      mGrouping   = catSelector->GetGrouping();
      QString str = QString(mGrouping.TypeString().c_str()) + QString(": ") + QString(mGrouping.Name().c_str());
      setText(str);
      emit GroupingSelected(mGrouping);
   }
   delete catSelector;
}

wkf::GroupingSelectorDialog::GroupingSelectorDialog(bool aShowIndividuals, bool aShowGroups, QWidget* aParent)
   : QDialog(aParent, Qt::Popup)
{
   mUi.setupUi(this);

   if (aShowGroups)
   {
      const std::map<wkf::PlatformGrouping, std::set<int>>& categories = wkfEnv.GetGroupings();
      for (auto&& it : categories)
      {
         QString cat = wkf::PlatformGrouping::GetTypeString(it.first.Type()).c_str();

         QTreeWidgetItem*        parent;
         QList<QTreeWidgetItem*> list = mUi.mTree->findItems(cat, Qt::MatchExactly);
         if (list.isEmpty())
         {
            QStringList cats;
            cats.push_back(cat);
            parent = new QTreeWidgetItem(cats);
            mUi.mTree->addTopLevelItem(parent);
         }
         else
         {
            parent = *list.begin();
         }
         QStringList strings;
         strings.push_back(it.first.Name().c_str());
         QTreeWidgetItem* item = new QTreeWidgetItem(parent, strings);
         parent->addChild(item);
         QByteArray  ba;
         QDataStream stream(&ba, QIODevice::WriteOnly);
         stream << QPair<int, QString>(it.first.Type(), QString(it.first.Name().c_str()));
         item->setData(0, Qt::UserRole, QVariant(ba));
      }
   }
   if (aShowIndividuals)
   {
      QTreeWidgetItem*        parent;
      QList<QTreeWidgetItem*> list = mUi.mTree->findItems("Platform", Qt::MatchExactly);
      if (list.isEmpty())
      {
         QStringList cats;
         cats.push_back("Platform");
         parent = new QTreeWidgetItem(cats);
         mUi.mTree->addTopLevelItem(parent);
      }
      else
      {
         parent = *list.begin();
      }

      Scenario* scenario = vaEnv.GetStandardScenario();
      if (scenario)
      {
         vespa::VaEntityList list;
         scenario->GetEntityList(list);

         for (auto&& it : list)
         {
            if (it->IsA_TypeOf<Platform>())
            {
               QStringList strings;
               strings.push_back(it->GetName().c_str());
               QTreeWidgetItem* item = new QTreeWidgetItem(parent, strings);
               parent->addChild(item);
               QByteArray  ba;
               QDataStream stream(&ba, QIODevice::WriteOnly);
               stream << QPair<int, QString>(wkf::PlatformGrouping::eIndividual, QString(it->GetName().c_str()));
               item->setData(0, Qt::UserRole, QVariant(ba));
            }
         }
      }
   }
   connect(mUi.mTree, &QTreeWidget::itemClicked, this, &GroupingSelectorDialog::ItemClicked);
   connect(mUi.mFilter, &QLineEdit::textChanged, this, &GroupingSelectorDialog::FilterChanged);
}

void wkf::GroupingSelectorDialog::ItemClicked(QTreeWidgetItem* aItemPtr, int aColumn)
{
   if (aItemPtr->data(0, Qt::UserRole).isValid())
   {
      QVariant            attackerVariant = aItemPtr->data(0, Qt::UserRole);
      QByteArray          aba             = attackerVariant.toByteArray();
      QDataStream         astream(&aba, QIODevice::ReadOnly);
      QPair<int, QString> qp;
      astream >> qp;
      mGrouping = wkf::PlatformGrouping(qp.first, qp.second.toStdString());
      accept();
   }
}

void wkf::GroupingSelectorDialog::FilterChanged(const QString& aFilter)
{
   QTreeWidgetItemIterator iter(mUi.mTree);
   while (QTreeWidgetItem* item = (*iter))
   {
      bool hidden = true;

      if (item->data(0, Qt::UserRole).isValid())
      {
         QString itemText = item->text(0);
         hidden           = !itemText.contains(aFilter, Qt::CaseInsensitive);
         item->setHidden(hidden);
      }
      ++iter;
   }
}
