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

#include "WkfBM_PlatformStatus.hpp"

#include <QDialogButtonBox>
#include <QInputDialog>
#include <QMenu>

#include "UtQtMemory.hpp"
#include "WkfBM_PlatformStatusPrefObject.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlatform.hpp"
#include "WkfPluginManager.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::bm::PlatformStatus::PlatformStatus(QWidget*                    aParent,
                                        PlatformStatusPrefObject&   aPrefObject,
                                        const PlatformPlotDatabase* aDatabase) noexcept
   : QDialog(aParent)
   , mExportHelper(aPrefObject, "Settings")
   , mPrefObject(aPrefObject)
   , mModel(aDatabase)
{
   setWindowFlag(Qt::WindowContextHelpButtonHint, false);
   mUi.setupUi(this);
   mUi.restoreButton->setAutoDefault(false);

   mUi.tableView->SetModel(&mModel);
   mUi.tableView->SetFixedColumnsSortable(true);
   mUi.tableView->SetDataColumnsSortable(true);
   mUi.tableView->SetFixedColumnCount(2);
   mUi.tableView->SetDataColumnsMovable(true);
   mUi.tableView->SetStretchLastColumn(true);
   mUi.tableView->SetRowHeight(18);
   mUi.tableView->SetDataEditTriggers(QAbstractItemView::NoEditTriggers);

   connect(mUi.tableView, &ut::qt::FixedColumnTableView::DoubleClicked, &mModel, &ModelBase::DoubleClicked);

   connect(mUi.currentColumnName, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PlatformStatus::GoToRuleSet);
   connect(mUi.addColumnButton, &QToolButton::clicked, this, &PlatformStatus::AddRuleSetClicked);
   connect(mUi.restoreButton, &QPushButton::clicked, this, &PlatformStatus::RestoreDefaultsClicked);

   connect(&mModel, &PlatformStatusModel::DataDoubleClicked, this, &PlatformStatus::GoToRuleSet);

   connect(this, &PlatformStatus::customContextMenuRequested, this, &PlatformStatus::CustomContextMenu);

   connect(&aPrefObject, &PlatformStatusPrefObject::ReadComplete, this, &PlatformStatus::ReadUserSettings);

   connect(&mExportHelper, &ExportHelper::WriteState, this, &PlatformStatus::WriteState);
   connect(&mExportHelper, &ExportHelper::ReadState, this, &PlatformStatus::ReadState);

   connect(mUi.tableView->SelectionModel(), &QItemSelectionModel::selectionChanged, this, &PlatformStatus::TableSelectionChanged);
   connect(&wkfEnv, &wkf::Environment::PlatformSelectionChanged, this, &PlatformStatus::PlatformSelectionChanged);
}

void wkf::bm::PlatformStatus::AddPlatform(const wkf::Platform* aPlatform) noexcept
{
   mModel.Platform(aPlatform);
   UpdatePlatform(aPlatform);
   mModel.UpdateCache();
}

void wkf::bm::PlatformStatus::AddRuleSet(wkf::bm::RuleSet* aRuleSet) noexcept
{
   mRuleSets.emplace_back(aRuleSet);
   mUi.ruleSets->addWidget(aRuleSet);
   mUi.currentColumnName->addItem(aRuleSet->GetLabel());

   aRuleSet->SetDefaults();

   mModel.AddDataColumns({aRuleSet->GetDisplayLabel()});

   connect(aRuleSet, &RuleSet::RulesChanged, this, &PlatformStatus::RuleSetChanged);
   connect(aRuleSet, &RuleSet::ShowClicked, this, &PlatformStatus::ShowDataColumn);

   GoToRuleSet(static_cast<int>(mRuleSets.size() - 1));
}

void wkf::bm::PlatformStatus::ClearPlatforms() noexcept
{
   mModel.Clear();
}

void wkf::bm::PlatformStatus::ReadUserSettings()
{
   mExportHelper.ReadInitialState();

   // Show all rows.
   mModel.ExpandAll();
   mModel.ShowAll();

   // Show all columns.
   for (auto* ruleSet : mRuleSets)
   {
      ruleSet->ShowColumn();
   }

   GoToRuleSet(0);
}

void wkf::bm::PlatformStatus::WriteUserSettings() const
{
   mExportHelper.WriteFinalState();
}

void wkf::bm::PlatformStatus::PlatformChanged(const wkf::Platform* aPlatform)
{
   UpdatePlatform(aPlatform);
}

void wkf::bm::PlatformStatus::RuleSetChanged(const wkf::bm::RuleSet* aRuleSet)
{
   const auto* scenario = vaEnv.GetStandardScenario();
   if (scenario)
   {
      for (const auto& platform : scenario->GetIndexPlatformMap())
      {
         auto proxy = mModel.Platform(platform.second);

         if (proxy)
         {
            QString text = aRuleSet->GetDisplayValue(platform.second);

            proxy.SetData(GetRuleSetIndex(aRuleSet), text, aRuleSet->GetColor(platform.second));
         }
      }
   }
}

void wkf::bm::PlatformStatus::RescanAll(double aSimTime)
{
   const auto* scenario = vaEnv.GetStandardScenario();
   if (scenario)
   {
      for (const auto& platform : scenario->GetIndexPlatformMap())
      {
         UpdatePlatform(platform.second);
      }
   }

   mModel.UpdatePlatformColors(aSimTime);
}

void wkf::bm::PlatformStatus::RestoreDefaultsClicked()
{
   QMessageBox prompt(this);
   QString     label = mRuleSets[CurrentRuleSetIndex()]->GetLabel();

   prompt.setWindowTitle("Restore Defaults");
   prompt.setText(QString("Would you like to restore the defaults to all columns or only %1?").arg(label));
   prompt.setIcon(QMessageBox::Question);
   prompt.setStandardButtons(QMessageBox::Cancel | QMessageBox::RestoreDefaults | QMessageBox::Reset);
   prompt.setDefaultButton(QMessageBox::Cancel);

   prompt.button(QMessageBox::Reset)->setText("All");
   prompt.button(QMessageBox::RestoreDefaults)->setText(label);

   const int userChoice = prompt.exec();
   switch (userChoice)
   {
   case QMessageBox::Reset:
      for (auto* ruleSet : mRuleSets)
      {
         ruleSet->SetDefaults();
      }
      break;
   case QMessageBox::RestoreDefaults:
      mRuleSets[CurrentRuleSetIndex()]->SetDefaults();
      break;
   default:
      // do nothing
      break;
   }
}

void wkf::bm::PlatformStatus::GoToRuleSet(int aIndex)
{
   mUi.ruleSets->setCurrentIndex(aIndex);
   mUi.currentColumnName->setCurrentIndex(aIndex);

   const auto* ruleSet = mRuleSets[aIndex];
   switch (ruleSet->GetMode())
   {
   case Mode::Boolean:
      mUi.typeLabel->setText("Type: Bool");
      break;
   case Mode::Number:
      mUi.typeLabel->setText("Type: Number");
      break;
   case Mode::Text:
      mUi.typeLabel->setText("Type: Text");
      break;
   default:
      mUi.typeLabel->setText("Type: Unknown");
   }
}

void wkf::bm::PlatformStatus::AddRuleSetClicked()
{
   QDialog dialog;

   dialog.setWindowTitle("New Column");
   dialog.setWindowFlag(Qt::WindowContextHelpButtonHint, false);

   auto* layout = ut::qt::make_qt_ptr<QVBoxLayout>();

   auto* label = ut::qt::make_qt_ptr<QLabel>("Aux Data Key:");
   layout->addWidget(label);

   auto* lineEdit = ut::qt::make_qt_ptr<QLineEdit>();
   layout->addWidget(lineEdit);

   auto* label2 = ut::qt::make_qt_ptr<QLabel>("Denominator Aux Data Key:");
   layout->addWidget(label2);
   label2->setVisible(false);

   auto* lineEdit2 = ut::qt::make_qt_ptr<QLineEdit>();
   layout->addWidget(lineEdit2);
   lineEdit2->setVisible(false);

   auto* comboBox = ut::qt::make_qt_ptr<QComboBox>();
   comboBox->addItems({"Boolean", "Number", "Text", "Ratio"});
   comboBox->setCurrentIndex(1);
   layout->addWidget(comboBox);

   connect(comboBox,
           QOverload<int>::of(&QComboBox::currentIndexChanged),
           [label, label2, lineEdit2](int aIndex)
           {
              const bool shown = (aIndex == 3);
              if (shown)
              {
                 label->setText("Numerator Aux Data Key:");
              }
              else
              {
                 label->setText("Aux Data Key:");
              }

              label2->setVisible(shown);
              lineEdit2->setVisible(shown);
           });

   auto* buttonBox = ut::qt::make_qt_ptr<QDialogButtonBox>(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
   layout->addWidget(buttonBox);

   dialog.setLayout(layout);

   connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
   connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

   connect(&dialog,
           &QDialog::accepted,
           [=]()
           {
              if (!lineEdit->text().isEmpty())
              {
                 switch (comboBox->currentIndex())
                 {
                 case 0:
                    emit AddAuxDataColumn(lineEdit->text(), RuleSet::Mode::Boolean);
                    break;
                 case 1:
                    emit AddAuxDataColumn(lineEdit->text(), RuleSet::Mode::Number);
                    break;
                 case 2:
                    emit AddAuxDataColumn(lineEdit->text(), RuleSet::Mode::Text);
                    break;
                 case 3:
                    if (!lineEdit2->text().isEmpty())
                    {
                       emit AddResourceAuxDataColumn(lineEdit->text(), lineEdit2->text());
                    }
                    break;
                 }
              }
           });

   dialog.exec();
}

void wkf::bm::PlatformStatus::RemoveRuleSet(RuleSet* aRuleSet)
{
   auto it = std::find(mRuleSets.begin(), mRuleSets.end(), aRuleSet);
   if (it != mRuleSets.end())
   {
      const int index = it - mRuleSets.begin();
      mRuleSets.erase(it);

      mUi.currentColumnName->removeItem(index);
      mModel.RemoveDataColumn(index);
      mUi.ruleSets->removeWidget(aRuleSet);
      mUi.currentColumnName->setCurrentIndex(CurrentRuleSetIndex());

      aRuleSet->deleteLater();
   }
}

void wkf::bm::PlatformStatus::HideDataColumn(RuleSet* aRuleSet)
{
   auto it = std::find(mRuleSets.begin(), mRuleSets.end(), aRuleSet);
   if (it != mRuleSets.end())
   {
      int index = mModel.FixedColumnDisplayCount() + (it - mRuleSets.begin());
      mUi.tableView->HideColumn(index);
      aRuleSet->EnableShowButton();
      GoToRuleSet(it - mRuleSets.begin());
   }
}

void wkf::bm::PlatformStatus::ShowDataColumn(RuleSet* aRuleSet)
{
   auto it = std::find(mRuleSets.begin(), mRuleSets.end(), aRuleSet);
   if (it != mRuleSets.end())
   {
      int index = mModel.FixedColumnDisplayCount() + (it - mRuleSets.begin());
      mUi.tableView->ShowColumn(index);
   }
}

void wkf::bm::PlatformStatus::CustomContextMenu(const QPoint& aPoint)
{
   QMenu menu;

   if (mUi.tableView->contentsRect().contains(aPoint))
   {
      const auto* selection = mUi.tableView->SelectionModel();
      menu.addActions(mModel.MenuActions(selection->selectedIndexes()));
      menu.addSeparator();
   }

   mExportHelper.BuildContextMenu(menu);

   if (mUi.tableView->contentsRect().contains(aPoint))
   {
      const int x      = mUi.tableView->mapFrom(this, aPoint).x();
      const int column = mUi.tableView->ColumnAt(x);

      RemoveColumnMenuItem(menu, column);

      if (column == 1)
      {
         BuildPlatformContextMenu(menu);
      }
   }

   menu.exec(mapToGlobal(aPoint));
}

void wkf::bm::PlatformStatus::WriteState(QString aName) const
{
   mPrefObject.Export(aName, ExportState());
}

void wkf::bm::PlatformStatus::ReadState(QString aName)
{
   ImportState(mPrefObject.GetExport(aName));
}

wkf::bm::Configuration wkf::bm::PlatformStatus::ExportState() const
{
   Configuration retval;

   retval.mSplitter = mUi.splitter->saveState();

   for (const auto* ruleSet : mRuleSets)
   {
      retval.mColumns.append(ruleSet->ExportState());
   }

   // retval.mHeader = mUi.tableView->horizontalHeader()->saveState();

   return retval;
}

void wkf::bm::PlatformStatus::ImportState(const Configuration& aConfiguration)
{
   mUi.splitter->restoreState(aConfiguration.mSplitter);
   RemoveRemovableRuleSets();

   mModel.SetHiddenNodes(aConfiguration);

   for (const auto& column : aConfiguration.mColumns)
   {
      if (column.mName.startsWith(cAUX_DATA_PREFIX))
      {
         QString copy = column.mName;
         copy.remove(cAUX_DATA_PREFIX);

         if (copy.contains(" / "))
         {
            auto parts = copy.split(" / ");
            emit AddResourceAuxDataColumn(parts[0], parts[1]);
         }
         else
         {
            emit AddAuxDataColumn(copy, column.mMode);
         }
      }

      for (auto* ruleSet : mRuleSets)
      {
         if (ruleSet->GetLabel() == column.mName)
         {
            ruleSet->ImportState(column);
            if (column.mHidden)
            {
               HideDataColumn(ruleSet);
            }
            break;
         }
      }
   }

   // mUi.tableView->horizontalHeader()->restoreState(aConfiguration.mHeader);
}

void wkf::bm::PlatformStatus::RemoveColumnMenuItem(QMenu& aMenu, int aColumn)
{
   if (aColumn >= mModel.FixedColumnDisplayCount())
   {
      auto index = aColumn - mModel.FixedColumnDisplayCount();
      aMenu.addSeparator();
      if (mRuleSets[index]->IsRemovable())
      {
         auto* act = ut::qt::make_qt_ptr<QAction>("Remove Column");
         connect(act, &QAction::triggered, [this, index]() { RemoveRuleSet(mRuleSets[index]); });
         aMenu.addAction(act);
      }
      else
      {
         auto* act = ut::qt::make_qt_ptr<QAction>("Hide Column");
         connect(act, &QAction::triggered, [this, index]() { HideDataColumn(mRuleSets[index]); });
         aMenu.addAction(act);
      }
   }
}

void wkf::bm::PlatformStatus::BuildPlatformContextMenu(QMenu& aMenu)
{
   const auto* selection = mUi.tableView->SelectionModel();
   auto        index     = selection->currentIndex();
   auto*       scenario  = vaEnv.GetStandardScenario();

   if (scenario && index.isValid() && index.column() == 1) // 1 is visual index of platform names
   {
      auto proxy = mModel.GetLeafAt(index.row());
      if (proxy)
      {
         auto* platform = scenario->FindPlatformByName(proxy.GetTitle());
         if (platform)
         {
            aMenu.addSeparator();
            wkfEnv.BuildEntityContextMenu(&aMenu, platform);
         }
      }
   }
}

void wkf::bm::PlatformStatus::TableSelectionChanged(const QItemSelection& aSelected, const QItemSelection& aDeselected)
{
   auto scenario = vaEnv.GetStandardScenario();
   if (scenario && !mInSelectionCallback)
   {
      mInSelectionCallback       = true;
      wkf::PlatformList selected = wkfEnv.GetSelectedPlatforms();

      for (auto index : aSelected.indexes())
      {
         if (index.column() == 1) // visual column for platforms
         {
            auto leaf = mModel.GetLeafAt(index.row());
            if (leaf)
            {
               auto* platform = scenario->FindPlatformByName(leaf.GetTitle());
               selected.append(platform);
            }
         }
      }

      for (auto index : aDeselected.indexes())
      {
         if (index.column() == 1) // visual column for platforms
         {
            auto leaf = mModel.GetLeafAt(index.row());
            if (leaf)
            {
               auto* platform = scenario->FindPlatformByName(leaf.GetTitle());
               selected.removeOne(platform);
            }
         }
      }

      selected.removeAll(nullptr);

      wkfEnv.SetPlatformsSelected(selected);

      mInSelectionCallback = false;
   }
}

void wkf::bm::PlatformStatus::PlatformSelectionChanged(const wkf::PlatformList& aSelected,
                                                       const wkf::PlatformList& aDeselected)
{
   auto scenario       = vaEnv.GetStandardScenario();
   auto selectionModel = mUi.tableView->SelectionModel();
   if (scenario && !mInSelectionCallback)
   {
      mInSelectionCallback    = true;
      QItemSelection selected = selectionModel->selection();

      for (const auto* platform : aSelected)
      {
         auto leaf = mModel.Platform(platform);
         if (leaf)
         {
            auto index = mModel.index(leaf.GetCachedRow(), 1);
            selected.select(index, index);
         }
      }

      QItemSelection deselected;

      for (const auto* platform : aDeselected)
      {
         auto leaf = mModel.Platform(platform);
         if (leaf)
         {
            auto index = mModel.index(leaf.GetCachedRow(), 1);
            deselected.select(index, index);
         }
      }

      selected.merge(deselected, QItemSelectionModel::SelectionFlag::Deselect);

      mUi.tableView->SelectionModel()->select(selected, QItemSelectionModel::SelectionFlag::ClearAndSelect);

      mInSelectionCallback = false;
   }
}

void wkf::bm::PlatformStatus::UpdatePlatform(const wkf::Platform* aPlatform)
{
   auto proxy = mModel.Platform(aPlatform);
   if (proxy)
   {
      const int count = static_cast<int>(mRuleSets.size());

      for (int i = 0; i < count; i++)
      {
         QString text = mRuleSets[i]->GetDisplayValue(aPlatform);

         proxy.SetData(i, text, mRuleSets[i]->GetColor(aPlatform));
      }
   }
}

int wkf::bm::PlatformStatus::CurrentRuleSetIndex() const noexcept
{
   return mUi.ruleSets->currentIndex();
}

int wkf::bm::PlatformStatus::GetRuleSetIndex(const RuleSet* aRuleSet) const noexcept
{
   const int size = static_cast<int>(mRuleSets.size());
   for (int i = 0; i < size; i++)
   {
      if (mRuleSets[i] == aRuleSet)
      {
         return i;
      }
   }
   return -1;
}

void wkf::bm::PlatformStatus::RemoveRemovableRuleSets()
{
   size_t i = 0;
   while (i < mRuleSets.size())
   {
      if (mRuleSets[i]->IsRemovable())
      {
         // don't increment i
         RemoveRuleSet(mRuleSets[i]);
      }
      else
      {
         i++;
      }
   }
}
