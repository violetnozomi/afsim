// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017-2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "AerodynamicsWidget.hpp"

#include <QFile>
#include <QJsonArray>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

#include "AeroTable2d.hpp"
#include "AeroTable3d.hpp"
#include "AeroTable4d.hpp"
#include "AeroTableBase.hpp"
#include "GeometryObjFile.hpp"
#include "MoverCreatorEnvironment.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "PerformanceWidget.hpp"
#include "PlotDataSet.hpp"
#include "ui_AerodynamicsWidget.h"
#include "VehicleAero.hpp"
#include "VehicleAeroCore.hpp"
#include "VehicleAeroMovable.hpp"

namespace Designer
{

AerodynamicsWidget::AerodynamicsWidget(QWidget*        aParent,
                                       Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QWidget(aParent, aFlags)
   , mUIPtr(new Ui::AerodynamicsWidget)
{
   mUIPtr->setupUi(this);

   connect(mUIPtr->mPlotTreeWidget, &QTreeWidget::itemChanged, this, &AerodynamicsWidget::SetPlotVisibility);
   connect(mUIPtr->mPlotTreeWidget, &QTreeWidget::itemClicked, this, &AerodynamicsWidget::SetPlotSelected);
   connect(mUIPtr->mApplyOffsetButton, &QPushButton::clicked, this, &AerodynamicsWidget::HandleGlobalOffsetClicked);
   connect(mUIPtr->mApplyMultiplierButton, &QPushButton::clicked, this, &AerodynamicsWidget::HandleGlobalMultiplierClicked);
   connect(mUIPtr->mNextButton, &QPushButton::clicked, this, &AerodynamicsWidget::NextButtonClicked);
   connect(mUIPtr->mPreviousButton, &QPushButton::clicked, this, &AerodynamicsWidget::PreviousButtonClicked);
   connect(mUIPtr->mShowModifierCheckbox, &QCheckBox::clicked, this, &AerodynamicsWidget::ToggleModifierLine);
   connect(mUIPtr->mMultiplierRadioButton, &QRadioButton::toggled, this, &AerodynamicsWidget::ToggleModifierType);
   connect(mUIPtr->mSelectAllButton, &QPushButton::clicked, this, &AerodynamicsWidget::HandleSelectAllClicked);
   connect(mUIPtr->mSelectNoneButton, &QPushButton::clicked, this, &AerodynamicsWidget::HandleSelectNoneClicked);
   connect(mUIPtr->mRefreshTrimButton, &QPushButton::clicked, this, &AerodynamicsWidget::HandleRefreshTrimClicked);
   connect(mUIPtr->mRevertPlotButton, &QPushButton::clicked, this, &AerodynamicsWidget::HandleRevertClicked);
   connect(mUIPtr->mRefreshPlotButton, &QPushButton::clicked, this, &AerodynamicsWidget::HandleRefreshPlotClicked);
   connect(mUIPtr->plotManagerWidget, &PlotManagerWidget::ModifierChanged, this, [this]() {SaveAero(); });

   connect(mUIPtr->mMoverComboBox, &QComboBox::currentTextChanged, this, &AerodynamicsWidget::HandleMoverChanged);
   connect(mUIPtr->mCLMaxLineEdit, &QLineEdit::editingFinished, this, &AerodynamicsWidget::CLMaxChanged);

   connect(mUIPtr->aeroComponentComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
           this, &AerodynamicsWidget::BuildTableList);

   connect(mUIPtr->aeroTableComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
           this, &AerodynamicsWidget::BuildPlotList);

   connect(mUIPtr->mPlotIndependentVarComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
           this, &AerodynamicsWidget::PopulateTableSelectionList);

   QIcon revertIcon;
   QString revertIconThemeName = QStringLiteral("revert");
   revertIcon = QIcon::fromTheme(revertIconThemeName);
   mUIPtr->mRevertPlotButton->setIcon(revertIcon);
}

AerodynamicsWidget::~AerodynamicsWidget()
{
   delete mUIPtr;
}

void AerodynamicsWidget::Initialize()
{
}

void AerodynamicsWidget::PopulateTableSelectionList()
{
   ClearGlobalModifierLineEdits();
   if ((mUIPtr->aeroComponentComboBox->currentIndex() == 0))
   {
      LoadSelectedCoreAeroTable();
   }
   else
   {
      LoadSelectedMovableAeroTable();
   }
}

void AerodynamicsWidget::PopulateGMProperties()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();

   if (mw.GetMoverCreatorWidget()->GenerateGuidedMover())
   {
      VehicleAero* aero = mw.GetMoverCreatorWidget()->GetVehicle()->GetAero();
      if (aero)
      {
         mUIPtr->mCLMaxLineEdit->setText(QString::number(aero->GetCLMax()));
      }
   }
}

void AerodynamicsWidget::SetPlotVisibility(QTreeWidgetItem* aItem, int aColumn)
{
   if (aItem->childCount() > 0)
   {
      Qt::CheckState itemVis = aItem->checkState(0);
      for (int i = 0; i < aItem->childCount(); ++i)
      {
         aItem->child(i)->setCheckState(0, itemVis);
      }
   }
   else
   {
      MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
      VehicleAero* aero = mw.GetMoverCreatorWidget()->GetVehicle()->GetAero();
      if (aero)
      {
         mUIPtr->plotManagerWidget->SetPlotVisible(mPlotMap[aItem].get(), aItem->checkState(0), aItem);
      }
   }
   mUIPtr->plotManagerWidget->RefreshPlot();
}

void AerodynamicsWidget::SetPlotSelected(QTreeWidgetItem* aItem, int aColumn)
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();

   VehicleAero* aero = mw.GetMoverCreatorWidget()->GetVehicle()->GetAero();
   if (aero)
   {
      mUIPtr->plotManagerWidget->SetSelectedPlot(mPlotMap[aItem].get());
   }
}

void AerodynamicsWidget::showEvent(QShowEvent* aEvent)
{
   mUIPtr->aeroComponentComboBox->clear();
   mUIPtr->aeroTableComboBox->clear();
   mUIPtr->mPlotIndependentVarComboBox->clear();

   BuildMoverList();
   BuildComponentList();
   BuildTableList();
   BuildPlotList();

   PopulateTableSelectionList();
   PopulateGMProperties();

   mUIPtr->plotManagerWidget->SetPlotColors();

   QWidget::showEvent(aEvent);
}

void AerodynamicsWidget::TabTransition(Designer::eMoverCreatorTab aMovingToTab)
{
   // This does not need to be checked for null
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();

   bool outputData = false;
   switch (aMovingToTab)
   {
      case Designer::cSETUP_TAB:
      case Designer::cENGINE_TAB:
      case Designer::cCONTROLS_TAB:
      case Designer::cGEOMETRY_TAB:
      case Designer::cAERODYNAMICS_TAB:
      case Designer::cAUTOPILOT_TAB:
      case Designer::cFLIGHT_TEST_TAB:
         // Do nothing
         break;

      case Designer::cPERFORMANCE_TAB:
         outputData = true;
         break;

      default:
         // Do nothing
         break;
   }

   if (outputData)
   {
      // Generate the scripts (data files)
      mcWidget->GetPerformanceWidget()->BeginScriptGeneration();

      // Output the 3D model set locally and to the site folder
      GeometryObjFile model3d;
      model3d.Output3dModelSet();
   }
}

void AerodynamicsWidget::NextButtonClicked()
{
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();

   // Generate the scripts (data files)
   mcWidget->GetPerformanceWidget()->BeginScriptGeneration();

   // Output the 3D model set locally and to the site folder
   GeometryObjFile model3d;
   model3d.Output3dModelSet();

   mcWidget->MoveToNextTab();
}

void AerodynamicsWidget::PreviousButtonClicked()
{
   MoverCreatorWidget* mcWidget = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();
   mcWidget->MoveToPreviousTab();
}

void AerodynamicsWidget::HandleGlobalOffsetClicked()
{
   mUIPtr->plotManagerWidget->SetGlobalOffset(mUIPtr->mGlobalOffsetLineEdit->text().toDouble());
   SaveAero();
}

void AerodynamicsWidget::HandleGlobalMultiplierClicked()
{
   mUIPtr->plotManagerWidget->SetGlobalMultiplier(mUIPtr->mGlobalMultiplierLineEdit->text().toDouble());
   SaveAero();
}

void AerodynamicsWidget::LoadSelectedCoreAeroTable()
{
   ClearTables();

   disconnect(mUIPtr->mPlotTreeWidget, &QTreeWidget::itemChanged, this, &AerodynamicsWidget::SetPlotVisibility);

   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   VehicleAero* aero = mw.GetMoverCreatorWidget()->GetVehicle()->GetAero();
   mCurrentTablePtr = aero->GetCoreAero().GetTableByName(mUIPtr->aeroTableComboBox->currentText());

   if (mCurrentTablePtr == nullptr)
   {
      return;
   }
   mUIPtr->plotManagerWidget->SetActiveTable(mCurrentTablePtr);
   mUIPtr->plotManagerWidget->SetComponentName(mUIPtr->aeroComponentComboBox->currentText());

   auto is4dTablePtr = dynamic_cast<AeroTable4d*>(mCurrentTablePtr);
   if (is4dTablePtr != nullptr) // We now know that we are loading in a 4d table here
   {
      Load4dTable(is4dTablePtr);
   }
   else
   {
      // Try a 3d table..
      auto is3dTablePtr = dynamic_cast<AeroTable3d*>(mCurrentTablePtr);
      if (is3dTablePtr != nullptr)
      {
         Load3dTable(is3dTablePtr);
      }
      else
      {
         // Try a 2d table....
         auto is2dTablePtr = dynamic_cast<AeroTable2d*>(mCurrentTablePtr);
         if (is2dTablePtr != nullptr)
         {
            Load2dTable(is2dTablePtr);
         }
      }

   }

   connect(mUIPtr->mPlotTreeWidget, &QTreeWidget::itemChanged, this, &AerodynamicsWidget::SetPlotVisibility);

   mUIPtr->plotManagerWidget->AddModifierSeries(mUIPtr->mShowModifierCheckbox->isChecked());

   // With the table loaded, see if this table is one that has been previously modified and saved off...
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   if (curDoc == nullptr)
   {
      return;
   }

   QString curPlotStr = mUIPtr->aeroTableComboBox->currentText();
   QJsonObject mainObj = curDoc->object();
   if (mainObj.contains("modified_aero"))
   {
      QJsonObject modificationObj = mainObj["modified_aero"].toObject();
      mUIPtr->plotManagerWidget->SetModifierObject(modificationObj);
      if (modificationObj.contains("Core Aero"))
      {
         QJsonObject coreAeroObj = modificationObj["Core Aero"].toObject();
         if (coreAeroObj.contains(curPlotStr))
         {
            QJsonObject tableObj = coreAeroObj[curPlotStr].toObject();
            // Determine if the currently set independent variable is in this table object
            QString independentVar = mUIPtr->mPlotIndependentVarComboBox->currentText();
            if (tableObj.contains(independentVar))
            {
               QJsonObject varObj = tableObj[independentVar].toObject();

               // Now, figure out if using a gain or an offset
               if (varObj.contains("gain"))
               {
                  // Finally, figure out if gain is an object or just a value.
                  // If it's an object, then this uses the modifier line,
                  // if just a value, then this uses a global gain
                  if (varObj["gain"].isObject())
                  {
                     QJsonObject gainObj = varObj["gain"].toObject();
                     std::vector<std::pair<double, double>> modifierLine;
                     QJsonArray xVals = gainObj["x_values"].toArray();
                     QJsonArray yVals = gainObj["y_values"].toArray();
                     QJsonArray::iterator xIter = xVals.begin();
                     QJsonArray::iterator yIter = yVals.begin();
                     while (xIter != xVals.end())
                     {
                        double curX = xIter->toDouble();
                        double curY = yIter->toDouble();
                        modifierLine.emplace_back(std::pair<double, double>(curX, curY));
                        ++xIter;
                        ++yIter;
                     }
                     mUIPtr->plotManagerWidget->SetModifierLineType(PlotManagerWidget::ModifierType::cMULTIPLY);
                     mUIPtr->mShowModifierCheckbox->setChecked(true);
                     ToggleModifierLine();
                     mUIPtr->plotManagerWidget->SetModifierLine(modifierLine);
                  }
                  else if (varObj["gain"].isDouble())
                  {
                     double gainModifierVal = varObj["gain"].toDouble();
                     mUIPtr->plotManagerWidget->SetGlobalMultiplier(gainModifierVal);
                     mUIPtr->mGlobalMultiplierLineEdit->setText(QString::number(gainModifierVal));
                  }
               }
               if (varObj.contains("offset"))
               {
                  // Determine if offset is an object or just a value. If it's an object,
                  // then this uses the modifier line, if just a value, then this uses a global offset
                  if (varObj["offset"].isObject())
                  {
                     QJsonObject offsetObj = varObj["offset"].toObject();
                     std::vector<std::pair<double, double>> modifierLine;
                     QJsonArray xVals = offsetObj["x_values"].toArray();
                     QJsonArray yVals = offsetObj["y_values"].toArray();
                     QJsonArray::iterator xIter = xVals.begin();
                     QJsonArray::iterator yIter = yVals.begin();
                     while (xIter != xVals.end())
                     {
                        double curX = xIter->toDouble();
                        double curY = yIter->toDouble();
                        modifierLine.emplace_back(std::pair<double, double>(curX, curY));
                        ++xIter;
                        ++yIter;
                     }
                     mUIPtr->plotManagerWidget->SetModifierLineType(PlotManagerWidget::ModifierType::cOFFSET);
                     mUIPtr->mShowModifierCheckbox->setChecked(true);
                     ToggleModifierLine();
                     mUIPtr->plotManagerWidget->SetModifierLine(modifierLine);
                  }
                  else if (varObj["offset"].isDouble())
                  {
                     double offsetModifierVal = varObj["offset"].toDouble();
                     mUIPtr->plotManagerWidget->SetGlobalOffset(offsetModifierVal);
                     mUIPtr->mGlobalOffsetLineEdit->setText(QString::number(offsetModifierVal));
                  }
               }
            }
         }
      }
   }
   mUIPtr->plotManagerWidget->RefreshPlot();
}

void AerodynamicsWidget::LoadSelectedMovableAeroTable()
{
   ClearTables();

   disconnect(mUIPtr->mPlotTreeWidget, &QTreeWidget::itemChanged, this, &AerodynamicsWidget::SetPlotVisibility);

   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   VehicleAero* aero = mw.GetMoverCreatorWidget()->GetVehicle()->GetAero();
   mCurrentTablePtr =
      aero->GetMovableAero(mUIPtr->aeroComponentComboBox->currentText().toStdString())->GetTableByName(mUIPtr->aeroTableComboBox->currentText());

   if (mCurrentTablePtr == nullptr)
   {
      return;
   }
   mUIPtr->plotManagerWidget->SetActiveTable(mCurrentTablePtr);
   mUIPtr->plotManagerWidget->SetComponentName(mUIPtr->aeroComponentComboBox->currentText());

   auto is4dTablePtr = dynamic_cast<AeroTable4d*>(mCurrentTablePtr);
   if (is4dTablePtr != nullptr) // We now know that we are loading in a 4d table here
   {
      Load4dTable(is4dTablePtr);
   }
   else
   {
      // Try a 3d table..
      auto is3dTablePtr = dynamic_cast<AeroTable3d*>(mCurrentTablePtr);
      if (is3dTablePtr != nullptr)
      {
         Load3dTable(is3dTablePtr);
      }
      else
      {
         // Try a 2d table....
         auto is2dTablePtr = dynamic_cast<AeroTable2d*>(mCurrentTablePtr);
         if (is2dTablePtr != nullptr)
         {
            Load2dTable(is2dTablePtr);
         }
      }
   }

   connect(mUIPtr->mPlotTreeWidget, &QTreeWidget::itemChanged, this, &AerodynamicsWidget::SetPlotVisibility);

   mUIPtr->plotManagerWidget->AddModifierSeries(mUIPtr->mShowModifierCheckbox->isChecked());

   // With the table loaded, see if this table is one that has been previously modified and saved off...
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   if (curDoc == nullptr)
   {
      return;
   }

   QString curPlotStr = mUIPtr->aeroTableComboBox->currentText();
   QJsonObject mainObj = curDoc->object();
   if (mainObj.contains("modified_aero"))
   {
      QJsonObject modificationObj = mainObj["modified_aero"].toObject();
      mUIPtr->plotManagerWidget->SetModifierObject(modificationObj);
      if (modificationObj.contains(mUIPtr->aeroComponentComboBox->currentText()))
      {
         QJsonObject moveableAeroObj = modificationObj[mUIPtr->aeroComponentComboBox->currentText()].toObject();
         if (moveableAeroObj.contains(curPlotStr))
         {
            QJsonObject tableObj = moveableAeroObj[curPlotStr].toObject();
            // Find if the currently set independent variable is in this table object
            QString independentVar = mUIPtr->mPlotIndependentVarComboBox->currentText();
            if (tableObj.contains(independentVar))
            {
               QJsonObject varObj = tableObj[independentVar].toObject();

               // Now, figure out if using a gain or an offset
               if (varObj.contains("gain"))
               {
                  // Finally, determine if gain is an object or just a value. If it's an object,
                  // then this uses the modifier line, if just a value, then this uses a global gain.
                  if (varObj["gain"].isObject())
                  {
                     QJsonObject gainObj = varObj["gain"].toObject();
                     std::vector<std::pair<double, double>> modifierLine;
                     QJsonArray xVals = gainObj["x_values"].toArray();
                     QJsonArray yVals = gainObj["y_values"].toArray();
                     QJsonArray::iterator xIter = xVals.begin();
                     QJsonArray::iterator yIter = yVals.begin();
                     while (xIter != xVals.end())
                     {
                        double curX = xIter->toDouble();
                        double curY = yIter->toDouble();
                        modifierLine.emplace_back(std::pair<double, double>(curX, curY));
                        ++xIter;
                        ++yIter;
                     }
                     mUIPtr->plotManagerWidget->SetModifierLineType(PlotManagerWidget::ModifierType::cMULTIPLY);
                     mUIPtr->mShowModifierCheckbox->setChecked(true);
                     ToggleModifierLine();
                     mUIPtr->plotManagerWidget->SetModifierLine(modifierLine);
                  }
                  else if (varObj["gain"].isDouble())
                  {
                     double gainModifierVal = varObj["gain"].toDouble();
                     mUIPtr->plotManagerWidget->SetGlobalMultiplier(gainModifierVal);
                     mUIPtr->mGlobalMultiplierLineEdit->setText(QString::number(gainModifierVal));
                  }
               }
               if (varObj.contains("offset"))
               {
                  // Determine if offset is an object or just a value. If it's an object, then this uses
                  // the modifier line, if just a value ,then this uses a global offset.
                  if (varObj["offset"].isObject())
                  {
                     QJsonObject offsetObj = varObj["offset"].toObject();
                     std::vector<std::pair<double, double>> modifierLine;
                     QJsonArray xVals = offsetObj["x_values"].toArray();
                     QJsonArray yVals = offsetObj["y_values"].toArray();
                     QJsonArray::iterator xIter = xVals.begin();
                     QJsonArray::iterator yIter = yVals.begin();
                     while (xIter != xVals.end())
                     {
                        double curX = xIter->toDouble();
                        double curY = yIter->toDouble();
                        modifierLine.emplace_back(std::pair<double, double>(curX, curY));
                        ++xIter;
                        ++yIter;
                     }
                     mUIPtr->plotManagerWidget->SetModifierLineType(PlotManagerWidget::ModifierType::cOFFSET);
                     mUIPtr->mShowModifierCheckbox->setChecked(true);
                     ToggleModifierLine();
                     mUIPtr->plotManagerWidget->SetModifierLine(modifierLine);
                  }
                  else if (varObj["offset"].isDouble())
                  {
                     double offsetModifierVal = varObj["offset"].toDouble();
                     mUIPtr->plotManagerWidget->SetGlobalOffset(offsetModifierVal);
                     mUIPtr->mGlobalOffsetLineEdit->setText(QString::number(offsetModifierVal));
                  }
               }
            }
         }
      }
   }
   mUIPtr->plotManagerWidget->RefreshPlot();
}

void AerodynamicsWidget::Load4dTable(AeroTable4d* aTablePtr)
{
   if (aTablePtr != nullptr)
   {
      // Figure out which independent variable the user wants to plot...
      QString independentVarStr = mUIPtr->mPlotIndependentVarComboBox->currentText();
      QString tertiaryStr = QString::fromStdString(aTablePtr->GetTertiaryIndependentVarName());
      QString secondaryStr = QString::fromStdString(aTablePtr->GetSecondaryIndependentVarName());
      QString primaryStr = QString::fromStdString(aTablePtr->GetPrimaryIndependentVarName());

      QString parentStr;
      std::vector<double> tertiaryVars;
      if (independentVarStr == primaryStr)
      {
         tertiaryVars = aTablePtr->GetTertiaryIndepenentValues();
         parentStr = QString::fromStdString(aTablePtr->GetTertiaryIndependentVarName());
      }
      else if (independentVarStr == secondaryStr)
      {
         tertiaryVars = aTablePtr->GetTertiaryIndepenentValues();
         parentStr = QString::fromStdString(aTablePtr->GetTertiaryIndependentVarName());
      }
      else if (independentVarStr == tertiaryStr)
      {
         tertiaryVars = aTablePtr->GetSecondaryIndependentValues();
         parentStr = QString::fromStdString(aTablePtr->GetSecondaryIndependentVarName());
      }

      for (auto curTertiary : tertiaryVars)
      {
         QTreeWidgetItem* parentTreeItem = new QTreeWidgetItem(mUIPtr->mPlotTreeWidget);
         parentTreeItem->setCheckState(0, Qt::Unchecked);
         parentTreeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
         parentTreeItem->setData(0, Qt::DisplayRole, curTertiary);
         parentTreeItem->setText(0, QString::number(curTertiary, 'g', 2) + " " + parentStr);

         QString childStr;
         std::vector<double> secondaryVars;
         if (independentVarStr == primaryStr)
         {
            secondaryVars = aTablePtr->GetSecondaryIndependentValues(curTertiary);
            childStr = QString::fromStdString(aTablePtr->GetSecondaryIndependentVarName());
         }

         else if (independentVarStr == secondaryStr)
         {
            secondaryVars = aTablePtr->GetPrimaryIndependentValuesGivenTertiary(curTertiary);
            childStr = QString::fromStdString(aTablePtr->GetPrimaryIndependentVarName());
         }
         else if (independentVarStr == tertiaryStr)
         {
            secondaryVars = aTablePtr->GetPrimaryIndependentValues();
            childStr = QString::fromStdString(aTablePtr->GetPrimaryIndependentVarName());
         }

         for (auto curSecondary : secondaryVars)
         {
            QTreeWidgetItem* childTreeItem = new QTreeWidgetItem(parentTreeItem);
            childTreeItem->setCheckState(0, Qt::Unchecked);
            childTreeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            childTreeItem->setData(0, Qt::DisplayRole, curSecondary);
            childTreeItem->setText(0, QString::number(curSecondary) + " " + childStr);

            std::vector<std::pair<double, double>> curData;
            if (independentVarStr == primaryStr)
            {
               aTablePtr->GetPrimaryBaseData(curSecondary, curTertiary, curData);
            }

            if (independentVarStr == secondaryStr)
            {
               aTablePtr->GetSecondaryBaseData(curSecondary, curTertiary, curData);
            }

            if (independentVarStr == tertiaryStr)
            {
               aTablePtr->GetTertiaryBaseData(curSecondary, curTertiary, curData);
            }

            mPlotMap.emplace(childTreeItem, ut::make_unique<PlotDataSet>());
            mPlotMap[childTreeItem]->SetPlotData(std::move(curData));
            mUIPtr->plotManagerWidget->AddPlot(mPlotMap[childTreeItem].get(), curSecondary, curTertiary, false);
         }
      }

      // These labels are just to make a single plot look nice
      QString plotTitle = QString(aTablePtr->GetDependentVarName().c_str()) + " vs " +
                          QString(aTablePtr->GetPrimaryIndependentVarName().c_str()) + " " +
                          QString(aTablePtr->GetSecondaryIndependentVarName().c_str()) + " " +
                          QString(aTablePtr->GetTertiaryIndependentVarName().c_str());
      mUIPtr->plotManagerWidget->SetTitle(plotTitle);
      mUIPtr->plotManagerWidget->SetLabelXAxis(independentVarStr);
      mUIPtr->plotManagerWidget->SetLabelYAxis(QString::fromStdString(aTablePtr->GetDependentVarName()));

      // Check the first child
      QTreeWidgetItem* curItem = mUIPtr->mPlotTreeWidget->topLevelItem(0);
      if (curItem)
      {
         // See if there is a '0' value top level item...
         for (int j = 0; j < mUIPtr->mPlotTreeWidget->topLevelItemCount(); ++j)
         {
            QTreeWidgetItem* curChild = mUIPtr->mPlotTreeWidget->topLevelItem(j);
            QStringList dataStrList = curChild->text(0).split(" ");
            if (dataStrList[0].toDouble() == 0.0)
            {
               curItem = curChild;
            }
         }
         if (curItem->childCount())
         {
            for (int i = 0; i < curItem->childCount(); ++i)
            {
               QTreeWidgetItem* curChild = curItem->child(i);
               QStringList dataStrList = curChild->text(0).split(" ");
               if (dataStrList[0].toDouble() == 0.0)
               {
                  curChild->setCheckState(0, Qt::Checked);
                  curChild->setSelected(true);
                  mUIPtr->plotManagerWidget->SetPlotVisible(mPlotMap[curChild].get(), true, curChild);
               }
            }
         }
         curItem->setExpanded(true);
      }
   }
}

void AerodynamicsWidget::Load3dTable(AeroTable3d* aTablePtr)
{
   if (aTablePtr != nullptr)
   {
      // Figure out which independent variable the user wants to plot...
      QString independentVarStr = mUIPtr->mPlotIndependentVarComboBox->currentText();
      QString secondaryStr = QString::fromStdString(aTablePtr->GetSecondaryIndependentVarName());
      QString primaryStr = QString::fromStdString(aTablePtr->GetPrimaryIndependentVarName());

      QString childStr;
      std::vector<double> secondaryVars;
      if (independentVarStr == primaryStr)
      {
         secondaryVars = aTablePtr->GetSecondaryIndependentValues();
         childStr = QString::fromStdString(aTablePtr->GetSecondaryIndependentVarName());
      }

      else if (independentVarStr == secondaryStr)
      {
         secondaryVars = aTablePtr->GetPrimaryIndependentValues();
         childStr = QString::fromStdString(aTablePtr->GetPrimaryIndependentVarName());
      }

      for (auto curSecondary : secondaryVars)
      {
         QTreeWidgetItem* childTreeItem = new QTreeWidgetItem(mUIPtr->mPlotTreeWidget);
         childTreeItem->setCheckState(0, Qt::Unchecked);
         childTreeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
         childTreeItem->setData(0, Qt::DisplayRole, curSecondary);
         childTreeItem->setText(0, QString::number(curSecondary) + " " + childStr);

         std::vector<std::pair<double, double>> curData;
         if (independentVarStr == primaryStr)
         {
            aTablePtr->GetPrimaryBaseData(curSecondary, curData);
         }

         if (independentVarStr == secondaryStr)
         {
            aTablePtr->GetSecondaryBaseData(curSecondary, curData);
         }

         mPlotMap.emplace(childTreeItem, ut::make_unique<PlotDataSet>());
         mPlotMap[childTreeItem]->SetPlotData(std::move(curData));
         mUIPtr->plotManagerWidget->AddPlot(mPlotMap[childTreeItem].get(), curSecondary, -1, false);
      }

      // These labels are just to make a single plot look nice
      QString plotTitle = QString(aTablePtr->GetDependentVarName().c_str()) + " vs " +
                          QString(aTablePtr->GetPrimaryIndependentVarName().c_str()) + " " +
                          QString(aTablePtr->GetSecondaryIndependentVarName().c_str());
      mUIPtr->plotManagerWidget->SetTitle(plotTitle);
      mUIPtr->plotManagerWidget->SetLabelXAxis(independentVarStr);
      mUIPtr->plotManagerWidget->SetLabelYAxis(QString::fromStdString(aTablePtr->GetDependentVarName()));

      // Check the first child
      QTreeWidgetItem* curItem = mUIPtr->mPlotTreeWidget->topLevelItem(0);
      if (curItem)
      {
         // See if there is a '0' value top level item...
         for (int j = 0; j < mUIPtr->mPlotTreeWidget->topLevelItemCount(); ++j)
         {
            QTreeWidgetItem* curChild = mUIPtr->mPlotTreeWidget->topLevelItem(j);
            QStringList dataStrList = curChild->text(0).split(" ");
            if (dataStrList[0].toDouble() == 0.0)
            {
               curItem = curChild;
            }
         }

         curItem->setCheckState(0, Qt::Checked);
         curItem->setSelected(true);
         mUIPtr->plotManagerWidget->SetPlotVisible(mPlotMap[curItem].get(), true, curItem);
      }
   }
}

void AerodynamicsWidget::Load2dTable(AeroTable2d* aTablePtr)
{
   if (aTablePtr != nullptr)
   {
      // Figure out which independent variable the user wants to plot...
      QString independentVarStr = mUIPtr->mPlotIndependentVarComboBox->currentText();
      QString independentStr = QString::fromStdString(aTablePtr->GetIndependentVarName());
      QString dependentStr = QString::fromStdString(aTablePtr->GetDependentVarName());

      QString plotTitle = dependentStr + " vs " + independentStr;
      QTreeWidgetItem* childTreeItem = new QTreeWidgetItem(mUIPtr->mPlotTreeWidget);
      childTreeItem->setCheckState(0, Qt::Unchecked);
      childTreeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      childTreeItem->setData(0, Qt::DisplayRole, plotTitle);
      childTreeItem->setText(0, plotTitle);

      std::vector<std::pair<double, double>> curData;
      aTablePtr->GetBaseData(curData);
      mPlotMap.emplace(childTreeItem, ut::make_unique<PlotDataSet>());
      mPlotMap[childTreeItem]->SetPlotData(std::move(curData));
      mUIPtr->plotManagerWidget->AddPlot(mPlotMap[childTreeItem].get(), -1, -1, false);

      // These labels are just to make a single plot look nice

      mUIPtr->plotManagerWidget->SetTitle(plotTitle);
      mUIPtr->plotManagerWidget->SetLabelXAxis(independentVarStr);
      mUIPtr->plotManagerWidget->SetLabelYAxis(QString::fromStdString(aTablePtr->GetDependentVarName()));

      // Check the first child
      QTreeWidgetItem* curItem = mUIPtr->mPlotTreeWidget->topLevelItem(0);
      if (curItem)
      {
         // See if there is a '0' value top level item...
         for (int j = 0; j < mUIPtr->mPlotTreeWidget->topLevelItemCount(); ++j)
         {
            QTreeWidgetItem* curChild = mUIPtr->mPlotTreeWidget->topLevelItem(j);
            QStringList dataStrList = curChild->text(0).split(" ");
            if (dataStrList[0].toDouble() == 0.0)
            {
               curItem = curChild;
            }
         }

         curItem->setCheckState(0, Qt::Checked);
         curItem->setSelected(true);
         mUIPtr->plotManagerWidget->SetPlotVisible(mPlotMap[curItem].get(), true, curItem);
      }
   }
}

void AerodynamicsWidget::BuildComponentList()
{
   // Build this list only with enabled tables from the current mVehicleAeroCore
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   VehicleAero* aero = mw.GetMoverCreatorWidget()->GetVehicle()->GetAero();

   mUIPtr->aeroComponentComboBox->clear();

   // Add in vehicle core first
   mUIPtr->aeroComponentComboBox->addItem("Core Aero");

   // Add in each moveable aero
   for (auto curMovable : aero->GetMovableAeroList())
   {
      if (curMovable.GetEnabledTables().size() > 0)
      {
         std::string curMovableStr = curMovable.GetName();
         mUIPtr->aeroComponentComboBox->addItem(QString::fromStdString(curMovableStr));
      }
   }
}

void AerodynamicsWidget::BuildTableList()
{
   // Build this list only with enabled tables from the current mVehicleAeroCore?
   mUIPtr->aeroTableComboBox->clear();
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   VehicleAero* aero = mw.GetMoverCreatorWidget()->GetVehicle()->GetAero();
   std::string curComponent = mUIPtr->aeroComponentComboBox->currentText().toStdString();
   if (curComponent == "Core Aero")
   {
      VehicleAeroCore* coreAero = &aero->GetCoreAero();
      std::vector<std::string> availableTables = coreAero->GetEnabledTables();
      for (auto curTableName : availableTables)
      {
         mUIPtr->aeroTableComboBox->addItem(QString::fromStdString(curTableName));
      }
   }
   else
   {
      VehicleAeroMovable* curMovable = aero->GetMovableAero(curComponent);
      if (curMovable != nullptr)
      {
         std::vector<std::string> availableTables = curMovable->GetEnabledTables();
         for (auto curTableName : availableTables)
         {
            mUIPtr->aeroTableComboBox->addItem(QString::fromStdString(curTableName));
         }
      }
   }
}

void AerodynamicsWidget::BuildPlotList()
{
   mUIPtr->mPlotIndependentVarComboBox->clear();
   QStringList plotVarList = mUIPtr->aeroTableComboBox->currentText().split(" ");
   mUIPtr->mDependentVariableLabel->setText(plotVarList[0]);

   // Find "vs" and ensure we're past it
   int i;
   for (i = 0; i < plotVarList.size(); ++i)
   {
       if (plotVarList[i] == "vs") { break; }
   }

   for (i++; i < plotVarList.size(); ++i)
   {
      mUIPtr->mPlotIndependentVarComboBox->addItem(plotVarList[i]);
   }
}

void AerodynamicsWidget::BuildMoverList()
{
   mUIPtr->mMoverComboBox->clear();
   MoverCreatorWidget* mcw = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget();

   if (mcw->GenerateP6DOFMover() && mcw->GenerateRB6DOFMover())
   {
      mUIPtr->mMoverComboBox->addItem("P6DOF/RB6DOF Mover");
   }
   else
   {
      if (mcw->GenerateP6DOFMover())
      {
         mUIPtr->mMoverComboBox->addItem("P6DOF Mover");
      }
      if (mcw->GenerateRB6DOFMover())
      {
         mUIPtr->mMoverComboBox->addItem("RB6DOF Mover");
      }
   }
   if (mcw->GeneratePM6DOFMover())
   {
       mUIPtr->mMoverComboBox->addItem("PM6DOF Mover");
   }
   if (mcw->GenerateGuidedMover())
   {
      mUIPtr->mMoverComboBox->addItem("Guided Mover");
   }
}

void AerodynamicsWidget::ClearTables()
{
   mUIPtr->plotManagerWidget->ClearPlots();
   mPlotMap.clear();
   mUIPtr->mPlotTreeWidget->clear();
   mCurrentTablePtr = nullptr;
}

void AerodynamicsWidget::SortTree(QTreeWidget* aTreeWidget, QString aSuffix)
{
   // First, remove all of the suffixes and strip each one down to just a value that can be sorted...
   for (int i = 0; i < aTreeWidget->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* curItem = aTreeWidget->topLevelItem(i);
      QString curText = curItem->text(0);
      QStringList strList = curText.split(" ");
      // The first value is the value and the second is the suffix
      double curValue = strList[0].toDouble();
      // Overwrite the current text
      curItem->setData(0, Qt::DisplayRole, curValue);
   }

   // Now, do the sort, then add text
   aTreeWidget->sortItems(0, Qt::AscendingOrder);

   for (int i = 0; i < aTreeWidget->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* curItem = aTreeWidget->topLevelItem(i);
      QString curText = curItem->text(0);
      curItem->setText(0, curText + " " + aSuffix);
   }
}

void AerodynamicsWidget::SortTree(QTreeWidgetItem* aTreeWidgetItem, QString aSuffix)
{
   // First, remove all of the suffixes and strip each one down to just a value that can be sorted...
   for (int i = 0; i < aTreeWidgetItem->childCount(); ++i)
   {
      QTreeWidgetItem* curItem = aTreeWidgetItem->child(i);
      QString curText = curItem->text(0);
      QStringList strList = curText.split(" ");

      // The first value is the value and the second is the suffix
      double curValue = strList[0].toDouble();

      // Overwrite the current text
      curItem->setData(0, Qt::DisplayRole, curValue);
   }

   // Now do the sort, then add text
   aTreeWidgetItem->sortChildren(0, Qt::AscendingOrder);

   for (int i = 0; i < aTreeWidgetItem->childCount(); ++i)
   {
      QTreeWidgetItem* curItem = aTreeWidgetItem->child(i);
      QString curText = curItem->text(0);
      curItem->setText(0, curText + " " + aSuffix);
   }
}

void AerodynamicsWidget::ToggleModifierLine()
{
   bool visible = mUIPtr->mShowModifierCheckbox->isChecked();
   mUIPtr->plotManagerWidget->SetModifierLineVisible(visible);
}

void AerodynamicsWidget::ToggleModifierType()
{
   if (mUIPtr->mOffsetRadioButton->isChecked())
   {
      mUIPtr->plotManagerWidget->SetModifierLineType(PlotManagerWidget::ModifierType::cOFFSET);
   }
   else
   {
      mUIPtr->plotManagerWidget->SetModifierLineType(PlotManagerWidget::ModifierType::cMULTIPLY);
   }
}

void AerodynamicsWidget::HandleSelectNoneClicked()
{
   QTreeWidgetItemIterator it(mUIPtr->mPlotTreeWidget);
   while (*it)
   {
      (*it)->setCheckState(0, Qt::Unchecked);
      ++it;
   }

   mUIPtr->plotManagerWidget->ClearAllPlots();
}

void AerodynamicsWidget::HandleSelectAllClicked()
{
   QTreeWidgetItemIterator it(mUIPtr->mPlotTreeWidget);
   while (*it)
   {
      (*it)->setCheckState(0, Qt::Checked);
      ++it;
   }
}

void AerodynamicsWidget::HandleRefreshTrimClicked()
{
    // TODO: This should only redo calculations needed for trim tables. 
    // It currently fully recreates all aero for every mover, which is overkill.

    MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetVehicle()->GetAero()->GetCoreAero().ClearAllData();
    for (auto& aero : MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetVehicle()->GetAero()->GetMovableAeroList())
    {
        aero.ClearAllData();
    }
    MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetVehicle()->GetAero()->CalculateFullVehicleAerodynamics();
}

void AerodynamicsWidget::SaveAero()
{
   if (MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentTab() != Designer::cAERODYNAMICS_TAB)
   {
      return;
   }

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   if (curDoc == nullptr)
   {
      return;
   }

   QJsonObject mainObj = curDoc->object();
   mainObj["modified_aero"] = mUIPtr->plotManagerWidget->GetModifierObject();
   curDoc->setObject(mainObj);
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentVehicleFile();
}

void AerodynamicsWidget::HandleRevertClicked()
{
   mUIPtr->mGlobalMultiplierLineEdit->setText("");
   mUIPtr->mGlobalOffsetLineEdit->setText("");
   mUIPtr->plotManagerWidget->RevertPlot();

   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentVehicleFile();
   QJsonObject mainObj = curDoc->object();
   QJsonObject copyModifierObj = mainObj["modified_aero"].toObject();

   if (copyModifierObj.contains(mUIPtr->aeroComponentComboBox->currentText()))
   {
      QJsonObject componentObj = copyModifierObj[mUIPtr->aeroComponentComboBox->currentText()].toObject();
      if (componentObj.contains(mUIPtr->aeroTableComboBox->currentText()))
      {
         QJsonObject tableObj = componentObj[mUIPtr->aeroTableComboBox->currentText()].toObject();
         if (tableObj.contains(mUIPtr->mPlotIndependentVarComboBox->currentText()))
         {
            tableObj.take(mUIPtr->mPlotIndependentVarComboBox->currentText());
            if (tableObj.size() == 0)
            {
               componentObj.take(mUIPtr->aeroTableComboBox->currentText());
            }
            else
            {
               componentObj[mUIPtr->aeroTableComboBox->currentText()] = tableObj;
            }
            copyModifierObj[mUIPtr->aeroComponentComboBox->currentText()] = componentObj;
         }
      }
   }
   mainObj["modified_aero"] = copyModifierObj;
   curDoc->setObject(mainObj);
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentVehicleFile();
   PopulateTableSelectionList();
}

void AerodynamicsWidget::ClearGlobalModifierLineEdits()
{
   mUIPtr->mGlobalMultiplierLineEdit->setText("");
   mUIPtr->mGlobalOffsetLineEdit->setText("");
}

void AerodynamicsWidget::HandleRefreshPlotClicked()
{
   mUIPtr->plotManagerWidget->RefreshPlot();
}

void AerodynamicsWidget::HandleMoverChanged(const QString& aMoverText)
{
   bool enableP6DOF = aMoverText.contains("P6DOF") || aMoverText.contains("RB6DOF");
   bool enablePM6DOF = aMoverText == "PM6DOF Mover";
   bool enableGM = aMoverText == "Guided Mover";

   mUIPtr->mCLMaxLabel->setEnabled(enableGM);
   mUIPtr->mCLMaxLineEdit->setEnabled(enableGM);

   // Enable the appropriate tables so that they appear in the table combo-box
   MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetVehicle()->GetAero()->EnableDisableTables(enableP6DOF, enableGM, enablePM6DOF);

   mUIPtr->mRefreshTrimButton->setEnabled(enablePM6DOF | enableGM);

   mUIPtr->aeroComponentComboBox->clear();
   mUIPtr->aeroTableComboBox->clear();
   mUIPtr->mPlotIndependentVarComboBox->clear();

   BuildComponentList();
   BuildTableList();
   BuildPlotList();

   PopulateTableSelectionList();
   PopulateGMProperties();
}

void AerodynamicsWidget::CLMaxChanged()
{
   MoverCreatorMainWindow& mw = MoverCreatorMainWindow::GetInstance();
   VehicleAero* aero = mw.GetMoverCreatorWidget()->GetVehicle()->GetAero();
   if (aero)
   {
      aero->SetCLMax(mUIPtr->mCLMaxLineEdit->text().toDouble());
      aero->CalculateVehicleAspectRatio();
   }
}

} // namespace Designer
