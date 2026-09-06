// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WkfMilStdIconPrefWidget.hpp"

#include "WkfEnvironment.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"

namespace
{
const QString cFRIEND_SECTION  = "Friend";
const QString cHOSTILE_SECTION = "Hostile";
const QString cNEUTRAL_SECTION = "Neutral";
} // namespace

wkf::MilStdIconPrefWidget::MilStdIconPrefWidget(QObject* parent)
{
   mUi.setupUi(this);

   QStringList categories = {cFRIEND_SECTION, cHOSTILE_SECTION, cNEUTRAL_SECTION};
   mUi.mCategoryBox->addItems(categories);
   auto teamVisPrefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
   if (teamVisPrefs)
   {
      UpdateTeamBox();
      connect(teamVisPrefs, &TeamVisibilityPrefObject::TeamVisibilityChanged, this, &MilStdIconPrefWidget::UpdateTeamBox);
   }

   mUi.mTeamMapping->setSelectionMode(QAbstractItemView::SingleSelection);
   mUi.mTeamMapping->setDragEnabled(true);
   mUi.mTeamMapping->setAcceptDrops(true);
   mUi.mTeamMapping->setDropIndicatorShown(true);
   mUi.mTeamMapping->setDragDropMode(QAbstractItemView::InternalMove);
   mUi.mTeamMapping->setDragDropOverwriteMode(true);
   mUi.mTeamMapping->invisibleRootItem()->setFlags(mUi.mTeamMapping->invisibleRootItem()->flags() ^ Qt::ItemIsDropEnabled);

   QTreeWidgetItem* friendTeamSection  = new QTreeWidgetItem(mUi.mTeamMapping, {cFRIEND_SECTION, ""});
   QTreeWidgetItem* hostileTeamSection = new QTreeWidgetItem(mUi.mTeamMapping, {cHOSTILE_SECTION, ""});
   QTreeWidgetItem* neutralTeamSection = new QTreeWidgetItem(mUi.mTeamMapping, {cNEUTRAL_SECTION, ""});

   friendTeamSection->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
   hostileTeamSection->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
   neutralTeamSection->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
   mUi.mTeamMapping->addTopLevelItem(friendTeamSection);
   mUi.mTeamMapping->addTopLevelItem(hostileTeamSection);
   mUi.mTeamMapping->addTopLevelItem(neutralTeamSection);
   mTeamSections.emplace(cFRIEND_SECTION, friendTeamSection);
   mTeamSections.emplace(cHOSTILE_SECTION, hostileTeamSection);
   mTeamSections.emplace(cNEUTRAL_SECTION, neutralTeamSection);

   connect(mUi.mAddButton, &QPushButton::clicked, this, &MilStdIconPrefWidget::AddTeamToSection);
   connect(mUi.mRemoveButton, &QPushButton::clicked, this, &MilStdIconPrefWidget::RemoveTeamFromSection);
   connect(mUi.mTeamMapping->model(),
           &QAbstractItemModel::rowsInserted,
           this,
           [&](const QModelIndex& parent, int first, int last)
           {
              for (auto& it : mTeamSections)
              {
                 it.second->sortChildren(0, Qt::SortOrder::AscendingOrder);
              }
           });
}

void wkf::MilStdIconPrefWidget::ReadPreferenceData(const MilStdIconPrefData& aPrefData)
{
   // There will be no duplicates unless the user manually modifies the .ini file, but this would be undefined behavior regardless.
   auto AddChildrenToSection = [](QTreeWidgetItem* aSection, const std::set<QString>& aList)
   {
      aSection->takeChildren();
      for (const auto& it : aList)
      {
         aSection->addChild(new QTreeWidgetItem(aSection, {it}));
      }
   };

   mUi.mUseSymbologyBox->setChecked(aPrefData.mUseSymbology);
   mUi.mVelocityVectorBox->setChecked(aPrefData.mShowVelocityVector);
   AddChildrenToSection(mTeamSections[cFRIEND_SECTION], aPrefData.mFriendTeams);
   AddChildrenToSection(mTeamSections[cHOSTILE_SECTION], aPrefData.mHostileTeams);
   AddChildrenToSection(mTeamSections[cNEUTRAL_SECTION], aPrefData.mNeutralTeams);
}

void wkf::MilStdIconPrefWidget::WritePreferenceData(MilStdIconPrefData& aPrefData)
{
   auto WriteTeamMapping = [&](std::set<QString>& aPref, const QString& aSection)
   {
      aPref.clear();
      int count = mTeamSections[aSection]->childCount();
      for (int i = 0; i < count; ++i)
      {
         aPref.emplace(mTeamSections[aSection]->child(i)->text(0));
      }
      mTeamSections[aSection]->sortChildren(0, Qt::SortOrder::AscendingOrder);
   };

   aPrefData.mUseSymbology       = mUi.mUseSymbologyBox->isChecked();
   aPrefData.mShowVelocityVector = mUi.mVelocityVectorBox->isChecked();
   WriteTeamMapping(aPrefData.mFriendTeams, cFRIEND_SECTION);
   WriteTeamMapping(aPrefData.mHostileTeams, cHOSTILE_SECTION);
   WriteTeamMapping(aPrefData.mNeutralTeams, cNEUTRAL_SECTION);
}

void wkf::MilStdIconPrefWidget::UpdateTeamBox()
{
   mUi.mTeamBox->clear();
   auto        teamVisPrefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
   QStringList teams;
   auto        teamMaps = teamVisPrefs->GetTeamVisibilityMap();
   for (const auto& it : teamMaps)
   {
      teams.push_back(it.first);
   }
   mUi.mTeamBox->addItems(teams);
}

void wkf::MilStdIconPrefWidget::AddTeamToSection()
{
   QTreeWidgetItem* selectedSection  = mTeamSections.at(mUi.mCategoryBox->currentText());
   QString          selectedTeam     = mUi.mTeamBox->currentText();
   auto             existingChildren = mUi.mTeamMapping->findItems(selectedTeam, Qt::MatchExactly | Qt::MatchRecursive);
   // section labels are caught here as well (e.g. you can't ave a team with the name "Friend" "Hostile" or "Neutral")
   if (existingChildren.empty())
   {
      selectedSection->addChild(new QTreeWidgetItem(selectedSection, {selectedTeam}));
   }
   else
   {
      QMessageBox::warning(nullptr,
                           "Mapping Error",
                           "The team has already been mapped. Please remove it before adding it again.");
   }
}

void wkf::MilStdIconPrefWidget::RemoveTeamFromSection()
{
   auto selectedItems = mUi.mTeamMapping->selectedItems();
   if (selectedItems.size() == 1)
   {
      selectedItems[0]->parent()->removeChild(selectedItems[0]);
      delete selectedItems[0];
   }
}
