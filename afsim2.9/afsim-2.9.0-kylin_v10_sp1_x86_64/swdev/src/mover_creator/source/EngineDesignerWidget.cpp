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

#include "EngineDesignerWidget.hpp"

#include <memory>
#include <QFileDialog>
#include <QJsonArray>
#include <QSettings>

#include "AeroJetEngine.hpp"
#include "AeroLiquidRocketEngine.hpp"
#include "AeroRamjetEngine.hpp"
#include "AeroSolidRocketEngine.hpp"
#include "AuditErrorDialog.hpp"
#include "EngineSaveDialog.hpp"
#include "EngineWidget.hpp"
#include "JetEngineWidget.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "MoverCreatorWidget.hpp"
#include "LiquidRocketEngineWidget.hpp"
#include "RamjetEngineWidget.hpp"
#include "SolidRocketEngineWidget.hpp"
#include "ui_EngineDesignerWidget.h"
#include "UtQtGL2DPlotWidget.hpp"

namespace Designer
{

EngineDesignerWidget::EngineDesignerWidget(QWidget*        aParent,
                                           Qt::WindowFlags aFlags /*= Qt::WindowFlags()*/)
   : QWidget(aParent, aFlags)
   , mUIPtr(new Ui::EngineDesignerWidget())
   , mEngineSaved(true)
{
   mUIPtr->setupUi(this);

   mPositiveValidator   = std::shared_ptr<QDoubleValidator>(new QDoubleValidator(0, std::numeric_limits<double>::max(), 10));
   mRealNumberValidator = std::shared_ptr<QDoubleValidator>(new QDoubleValidator(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 10));

   mUIPtr->mLengthLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->mDiameterLineEdit->setValidator(mPositiveValidator.get());
   mUIPtr->mThrustOffsetLineEdit->setValidator(mRealNumberValidator.get());

   mUIPtr->plotManagerWidget->setContextMenuPolicy(Qt::CustomContextMenu);

   // When any engine spec changes in the engine designer widget, notify the AeroEngine of the change
   connect(mUIPtr->mLengthLineEdit, &QLineEdit::editingFinished, this, [this]()
   {
      mEngineWidgetPtr->LengthChanged(mUIPtr->mLengthLineEdit->text().toDouble());
   });
   connect(mUIPtr->mDiameterLineEdit, &QLineEdit::editingFinished, this, [this]()
   {
      mEngineWidgetPtr->DiameterChanged(mUIPtr->mDiameterLineEdit->text().toDouble());
   });
   connect(mUIPtr->mThrustOffsetLineEdit, &QLineEdit::editingFinished, this, [this]()
   {
      mEngineWidgetPtr->ThrustOffsetChanged(mUIPtr->mThrustOffsetLineEdit->text().toDouble());
   });

   // Save the engine model (JSON)
   connect(mUIPtr->mSaveModelButton, &QPushButton::clicked, this, &EngineDesignerWidget::SaveModel);

   // Apply a global offset to the current table
   connect(mUIPtr->mApplyOffsetButton, &QPushButton::clicked, this, &EngineDesignerWidget::HandleGlobalOffsetClicked);

   // Apply a global gain to the current table
   connect(mUIPtr->mApplyMultiplierButton, &QPushButton::clicked, this, &EngineDesignerWidget::HandleGlobalMultiplierClicked);

   connect(mUIPtr->plotManagerWidget, &PlotManagerWidget::ModifierChanged, this, [this]() {SaveModel();});

   // When the user chooses a plot to display, update the plot
   connect(mUIPtr->plotTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &EngineDesignerWidget::UpdatePlot);

   // Toggle the modifier line
   connect(mUIPtr->mShowModifierCheckbox, &QCheckBox::clicked, this, &EngineDesignerWidget::ToggleModifierLine);

   // Toggle modifier type
   connect(mUIPtr->mMultiplierRadioButton, &QRadioButton::toggled, this, &EngineDesignerWidget::ToggleModifierType);

   // Revert plot
   connect(mUIPtr->mRevertPlotButton, &QPushButton::clicked, this, &EngineDesignerWidget::RevertPlot);

   QIcon revertIcon;
   QString revertIconThemeName = QStringLiteral("revert");
   revertIcon = QIcon::fromTheme(revertIconThemeName);
   mUIPtr->mRevertPlotButton->setIcon(revertIcon);
}

EngineDesignerWidget::~EngineDesignerWidget()
{
   delete mUIPtr;
}

void EngineDesignerWidget::SaveModel()
{
   if (MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentTab() != Designer::cENGINE_TAB)
   {
      return;
   }

   QStringList errorList = Audit();
   if (errorList.size())
   {
      AuditErrorDialog errorDialog(this);
      errorDialog.AddErrors(errorList);
      errorDialog.ShowOverrideButton();
      if (errorDialog.exec() == QDialog::Accepted)
      {
         // Override was clicked, save anyway
         mAeroEnginePtr->SaveToFile();

         std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
         QJsonObject mainObj    = curDoc->object();
         QJsonObject plotsObj   = mainObj["plots"].toObject();
         QJsonObject curPlotObj = plotsObj[mUIPtr->plotTypeComboBox->currentText()].toObject();
         QJsonObject modifierObject;
         QJsonArray modifierXValues;
         QJsonArray modifierYValues;
         for (const auto& curPt : mUIPtr->plotManagerWidget->GetModifierPoints())
         {
            modifierXValues.push_back(curPt.first);
            modifierYValues.push_back(curPt.second);
         }
         QJsonObject arrayObj;
         arrayObj["x_values"] = modifierXValues;
         arrayObj["y_values"] = modifierYValues;
         QJsonObject modifierObj;
         if (mUIPtr->mMultiplierRadioButton->isChecked())
         {
            modifierObj["gain"] = arrayObj;
         }
         else if (mUIPtr->mOffsetRadioButton->isChecked())
         {
            modifierObj["offset"] = arrayObj;
         }
         curPlotObj["modifier"] = modifierObj;

         plotsObj[mUIPtr->plotTypeComboBox->currentText()] = curPlotObj;
         mainObj["plots"] = plotsObj;
         curDoc->setObject(mainObj);
         MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentEngineFile();

         mEngineSaved = true;
      }
   }
   else
   {
      mAeroEnginePtr->SaveToFile();
      std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
      QJsonObject mainObj          = curDoc->object();
      QJsonObject plotsObj         = mainObj["plots"].toObject();
      QJsonObject curPlotObj       = plotsObj[mUIPtr->plotTypeComboBox->currentText()].toObject();
      AeroEngine::DataKey plotType = static_cast<AeroEngine::DataKey>(mUIPtr->plotTypeComboBox->currentData().toInt());

      // Find out what kind of modifier is being applied...
      AeroTable2d* curTablePtr       = mAeroEnginePtr->GetEngineTable(plotType);
      bool usingGainModifierList     = curTablePtr->UsingPrimaryGainModiferList();
      bool usingOffsetModifierList   = curTablePtr->UsingPrimaryOffsetModiferList();
      bool usingSingleGainModifier   = curTablePtr->UsingSinglePrimaryGainModifer();//UsingPrimaryGainModifer();
      bool usingSingleOffsetModifier = curTablePtr->UsingSinglePrimaryOffsetModifer();

      if (usingGainModifierList || usingOffsetModifierList || usingSingleGainModifier || usingSingleOffsetModifier)
      {
         QJsonObject modifierObj;
         if (usingGainModifierList || usingOffsetModifierList)
         {
            QJsonArray modifierXValues;
            QJsonArray modifierYValues;
            for (const auto& curPt : mUIPtr->plotManagerWidget->GetModifierPoints())
            {
               modifierXValues.push_back(curPt.first);
               modifierYValues.push_back(curPt.second);
            }
            QJsonObject arrayObj;
            arrayObj["x_values"] = modifierXValues;
            arrayObj["y_values"] = modifierYValues;

            if (usingGainModifierList)
            {
               modifierObj["gain"] = arrayObj;
            }
            else if (usingOffsetModifierList)
            {
               modifierObj["offset"] = arrayObj;
            }
         }

         else if (usingSingleGainModifier)
         {
            if (mUIPtr->mGlobalMultiplierLineEdit->text() != "")
            {
               modifierObj["gain"] = mUIPtr->mGlobalMultiplierLineEdit->text().toDouble();
            }
         }
         else if (usingSingleOffsetModifier)
         {
            if (mUIPtr->mGlobalOffsetLineEdit->text() != "")
            {
               modifierObj["offset"] = mUIPtr->mGlobalOffsetLineEdit->text().toDouble();
            }
         }
         curPlotObj["modifier"] = modifierObj;
      }
      plotsObj[mUIPtr->plotTypeComboBox->currentText()] = curPlotObj;
      mainObj["plots"] = plotsObj;
      curDoc->setObject(mainObj);
      MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentEngineFile();

      mEngineSaved = true;
   }
}

void EngineDesignerWidget::UpdatePlot()
{
   mUIPtr->plotManagerWidget->ClearPlots();

   mUIPtr->mGlobalOffsetLineEdit->setText("");
   mUIPtr->mGlobalMultiplierLineEdit->setText("");

   PlotDataSet* plotData = new PlotDataSet;

   QString plotType = mUIPtr->plotTypeComboBox->currentText();
   if (!plotType.isEmpty())
   {
      std::vector<std::pair<double, double>> engineTableData;
      mAeroEnginePtr->CalcEngineData();

      AeroTable2d* curTablePtr = mAeroEnginePtr->GetEngineTable(plotType);
      curTablePtr->GetBaseData(engineTableData);

      mUIPtr->plotManagerWidget->SetActiveTable(curTablePtr);
      mUIPtr->plotManagerWidget->SetPointUnmodifiable(0);
      plotData->SetPlotData(std::move(engineTableData));
      mUIPtr->plotManagerWidget->AddPlot(plotData);

      mUIPtr->plotManagerWidget->AddModifierSeries(mUIPtr->mShowModifierCheckbox->isChecked());
      LoadModifierInfo();

      bool usingGainModifierList     = curTablePtr->UsingPrimaryGainModiferList();
      bool usingOffsetModifierList   = curTablePtr->UsingPrimaryOffsetModiferList();
      bool usingSingleGainModifier   = curTablePtr->UsingSinglePrimaryGainModifer();//UsingPrimaryGainModifer();
      bool usingSingleOffsetModifier = curTablePtr->UsingSinglePrimaryOffsetModifer();

      AeroEngine::DataKey dataType = static_cast<AeroEngine::DataKey>(mUIPtr->plotTypeComboBox->currentData().toInt());
      if (usingGainModifierList || usingSingleGainModifier)
      {
         mUIPtr->plotManagerWidget->SetModifierYAxisBound(0, 10.0);
      }
      else if (usingOffsetModifierList || usingSingleOffsetModifier)
      {
         double upperBound = mAeroEnginePtr->GetDataMultiplier(dataType);
         upperBound *= 2;
         mUIPtr->plotManagerWidget->SetModifierYAxisBound(-1 * upperBound, upperBound);
      }
      else if (mUIPtr->mShowModifierCheckbox->isChecked() && mUIPtr->mOffsetRadioButton->isChecked())
      {
         double upperBound = mAeroEnginePtr->GetDataMultiplier(dataType);
         mUIPtr->plotManagerWidget->SetModifierYAxisBound(0, upperBound);
      }
      else
      {
         mUIPtr->plotManagerWidget->SetModifierYAxisBound(0, 10.0);
      }

      const AeroEngine::DataLabels& temp = mAeroEnginePtr->GetPlotLabels(plotType);
      mUIPtr->plotManagerWidget->SetTitle(QString::fromStdString(temp.title));
      mUIPtr->plotManagerWidget->SetLabelXAxis(QString::fromStdString(temp.xLabel));
      mUIPtr->plotManagerWidget->SetLabelYAxis(QString::fromStdString(temp.yLabel));
      mUIPtr->plotManagerWidget->SetPlotVisible(plotData, true);
   }
}

void Designer::EngineDesignerWidget::HandleEngineSpecsChanged()
{
   // Get the updated EngineSpecs from the widget (may have changed due to user input)
   mAeroEnginePtr->SetEngineSpecsOptions(mEngineWidgetPtr->GetEngineSpecsOptions());
   mAeroEnginePtr->SetEngineSpecs(mEngineWidgetPtr->GetEngineSpecs());

   mEngineWidgetPtr->UpdateEngine(mAeroEnginePtr.get());

   mEngineSaved = false;

   UpdatePlot();
}

void EngineDesignerWidget::ToggleModifierLine()
{
   bool visible = mUIPtr->mShowModifierCheckbox->isChecked();
   mUIPtr->plotManagerWidget->SetModifierLineVisible(visible);
   mUIPtr->plotManagerWidget->RefreshPlot();
   ToggleModifierType();
}

void EngineDesignerWidget::showEvent(QShowEvent* aEvent)
{
   LoadEngine();
   mUIPtr->plotManagerWidget->SetPlotColors();
   QWidget::showEvent(aEvent);
}

void EngineDesignerWidget::LoadEngine()
{
   // If there is already an allocated engine get rid of it and clear out its data
   mEngineWidgetPtr = nullptr;

   if (mUIPtr->mEngineTypeLineEdit->text() == "Jet")
   {
      QString modelName = mUIPtr->mModelNameLineEdit->text();

      mAeroEnginePtr = ut::make_unique<AeroJetEngine>(modelName.toStdString());

      bool includeAfterburner = dynamic_cast<AeroJetEngine*>(mAeroEnginePtr.get())->IncludeAfterburner();

      mEngineWidgetPtr = ut::make_unique<JetEngineWidget>(QString::fromStdString(mAeroEnginePtr->GetModelName()),
                                                          mAeroEnginePtr->GetNumBaseSpecs(),
                                                          mAeroEnginePtr->GetNumBaseSpecsOptions(),
                                                          includeAfterburner);
   }
   else if (mUIPtr->mEngineTypeLineEdit->text() == "SolidRocket")
   {
      QString modelName = mUIPtr->mModelNameLineEdit->text();

      mAeroEnginePtr = ut::make_unique<AeroSolidRocketEngine>(modelName.toStdString());

      mEngineWidgetPtr = ut::make_unique<SolidRocketEngineWidget>(QString::fromStdString(mAeroEnginePtr->GetModelName()),
                                                                  mAeroEnginePtr->GetNumBaseSpecs(),
                                                                  mAeroEnginePtr->GetNumBaseSpecsOptions());
   }
   else if (mUIPtr->mEngineTypeLineEdit->text() == "Ramjet")
   {
      QString modelName = mUIPtr->mModelNameLineEdit->text();

      mAeroEnginePtr = ut::make_unique<AeroRamjetEngine>(modelName.toStdString());

      mEngineWidgetPtr = ut::make_unique<RamjetEngineWidget>(QString::fromStdString(mAeroEnginePtr->GetModelName()),
                                                             mAeroEnginePtr->GetNumBaseSpecs(),
                                                             mAeroEnginePtr->GetNumBaseSpecsOptions());
   }
   else if (mUIPtr->mEngineTypeLineEdit->text() == "LiquidRocket")
   {
      QString modelName = mUIPtr->mModelNameLineEdit->text();

      mAeroEnginePtr = ut::make_unique<AeroLiquidRocketEngine>(modelName.toStdString());

      mEngineWidgetPtr = ut::make_unique<LiquidRocketEngineWidget>(QString::fromStdString(mAeroEnginePtr->GetModelName()),
                                                                   mAeroEnginePtr->GetNumBaseSpecs(),
                                                                   mAeroEnginePtr->GetNumBaseSpecsOptions());
   }
   else
   {
      // If there is an invalid selection, cut your losses and get out of here
      mEngineWidgetPtr = nullptr;
      mAeroEnginePtr = nullptr;
      return;
   }

   mEngineWidgetPtr->LoadEngine(mAeroEnginePtr.get());

   if (mEngineWidgetPtr != nullptr)
   {
      // When any engine spec changes in the engine widget, notify the AeroEngine of the change
      connect(mEngineWidgetPtr.get(), &EngineWidget::EngineSpecsChanged, this, &EngineDesignerWidget::HandleEngineSpecsChanged);

      mUIPtr->plotTypeComboBox->clear();

      for (const auto& i : mAeroEnginePtr->GetPlotNameMap())
      {
         // Populate the plot selection combo box
         mUIPtr->plotTypeComboBox->addItem(QString::fromStdString(i.second.title), static_cast<int>(i.first));
      }

      // Add the design parameters to the GUI
      mUIPtr->verticalLayout_2->insertWidget(1, mEngineWidgetPtr.get());

      mUIPtr->mLengthLineEdit->setText(QString::number(mAeroEnginePtr->GetLength_ft()));
      mUIPtr->mDiameterLineEdit->setText(QString::number(mAeroEnginePtr->GetDiameter_ft()));
      mUIPtr->mThrustOffsetLineEdit->setText(QString::number(mAeroEnginePtr->GetThrustOffset_ft()));
   }

   // Load the modifier line if one was saved off
   LoadModifierInfo();
}

void EngineDesignerWidget::SetTypeAndModel(QString aType, QString aModel)
{
   mUIPtr->mEngineTypeLineEdit->setText(aType);
   mUIPtr->mModelNameLineEdit->setText(aModel);
}

bool EngineDesignerWidget::CheckNeedsSave()
{
   if (!mEngineSaved)
   {
      EngineSaveDialog saveDlg(this);
      int retval = saveDlg.exec();
      if (retval == 0)
      {
         return true;
      }
      else
      {
         return false;
      }
   }

   return false;
}

QStringList EngineDesignerWidget::Audit()
{
   QStringList errorString;

   if (mEngineWidgetPtr != nullptr)
   {
      errorString = mEngineWidgetPtr->Audit();
   }

   return errorString;
}

void EngineDesignerWidget::RevertPlot()
{
   mUIPtr->plotManagerWidget->RevertPlot();

   QString plotName = mUIPtr->plotTypeComboBox->currentText();
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
   QJsonObject mainObj = curDoc->object();
   QJsonObject copyPlotsObj = mainObj["plots"].toObject();
   QJsonObject copyCurPlotObj = copyPlotsObj[plotName].toObject();
   if (copyCurPlotObj.contains("modifier"))
   {
      copyCurPlotObj.take("modifier");
      copyPlotsObj[plotName] = copyCurPlotObj;
      mainObj["plots"] = copyPlotsObj;
      curDoc->setObject(mainObj);
      MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->SaveCurrentEngineFile();
   }
   mUIPtr->mGlobalMultiplierLineEdit->setText("");
   mUIPtr->mGlobalOffsetLineEdit->setText("");
   UpdatePlot();

   // Set the modifier y-axis to something reasonable for this plot depending on whether it's an offset or gain modifier
   if (mUIPtr->mOffsetRadioButton->isChecked())
   {
      AeroEngine::DataKey plotType = static_cast<AeroEngine::DataKey>(mUIPtr->plotTypeComboBox->currentData().toInt());
      double upperBound = mAeroEnginePtr->GetDataMultiplier(plotType);
      upperBound *= 2;
      mUIPtr->plotManagerWidget->SetModifierYAxisBound(-1 * upperBound, upperBound);
   }
   else
   {
      mUIPtr->plotManagerWidget->SetModifierYAxisBound(0, 10.0);
   }
}

void EngineDesignerWidget::ToggleModifierType()
{
   if (mUIPtr->mOffsetRadioButton->isChecked())
   {
      mUIPtr->plotManagerWidget->SetModifierLineType(PlotManagerWidget::ModifierType::cOFFSET);
      AeroEngine::DataKey plotType = static_cast<AeroEngine::DataKey>(mUIPtr->plotTypeComboBox->currentData().toInt());
      double upperBound = mAeroEnginePtr->GetDataMultiplier(plotType);
      upperBound *= 2;
      mUIPtr->plotManagerWidget->SetModifierYAxisBound(-1 * upperBound, upperBound);
   }
   else
   {
      mUIPtr->plotManagerWidget->SetModifierLineType(PlotManagerWidget::ModifierType::cMULTIPLY);
      mUIPtr->plotManagerWidget->SetModifierYAxisBound(0, 10.0);
   }
}

void EngineDesignerWidget::LoadModifierInfo()
{
   // Load the modifier line if one was saved off
   std::shared_ptr<QJsonDocument> curDoc = MoverCreatorMainWindow::GetInstance().GetMoverCreatorWidget()->GetCurrentEngineFile();
   if (!curDoc)
   {
      return;
   }
   QJsonObject mainObj = curDoc->object();
   QJsonObject plotsObj = mainObj["plots"].toObject();

   // Find the current plot and see if there is a modifier line
   QJsonObject curPlot = plotsObj[mUIPtr->plotTypeComboBox->currentText()].toObject();
   if (curPlot.contains("modifier"))
   {
      QJsonObject modifierObj = curPlot["modifier"].toObject();

      // Now, figure out if using a gain or an offset

      if (modifierObj.contains("gain"))
      {
         // Finally, figure out if gain is an object or just a value.
         // If it's an object, then this uses the modifier line, if just a value then this uses a global gain
         if (modifierObj["gain"].isObject())
         {
            QJsonObject gainObj = modifierObj["gain"].toObject();
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
         if (modifierObj["gain"].isDouble())
         {
            double gainValue = modifierObj["gain"].toDouble();
            mUIPtr->plotManagerWidget->SetGlobalMultiplier(gainValue);
            mUIPtr->mGlobalMultiplierLineEdit->setText(QString::number(gainValue));
         }
      }
      else if (modifierObj.contains("offset"))
      {
         // Finally, figure out if gain is an object or just a value.
         // If it's an object, then this uses the modifier line, if just a value then this uses a global gain
         if (modifierObj["offset"].isObject())
         {
            QJsonObject gainObj = modifierObj["offset"].toObject();
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
            mUIPtr->plotManagerWidget->SetModifierLineType(PlotManagerWidget::ModifierType::cOFFSET);
            mUIPtr->mShowModifierCheckbox->setChecked(true);
            ToggleModifierLine();
            mUIPtr->plotManagerWidget->SetModifierLine(modifierLine);
         }
         if (modifierObj["offset"].isDouble())
         {
            double offsetValue = modifierObj["offset"].toDouble();
            mUIPtr->plotManagerWidget->SetGlobalOffset(offsetValue);
            mUIPtr->mGlobalOffsetLineEdit->setText(QString::number(offsetValue));
         }
      }
   }
}

void EngineDesignerWidget::HandleGlobalOffsetClicked()
{
   mUIPtr->plotManagerWidget->SetGlobalOffset(mUIPtr->mGlobalOffsetLineEdit->text().toDouble());
   SaveModel();
}

void EngineDesignerWidget::HandleGlobalMultiplierClicked()
{
   mUIPtr->plotManagerWidget->SetGlobalMultiplier(mUIPtr->mGlobalMultiplierLineEdit->text().toDouble());
   SaveModel();
}

} // namespace Designer
