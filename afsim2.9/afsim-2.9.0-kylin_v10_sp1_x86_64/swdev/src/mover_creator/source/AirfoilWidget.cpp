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

#include "AirfoilWidget.hpp"

#include <QDir>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

#include "AeroData.hpp"
#include "GeometrySurface.hpp"
#include "MoverCreatorMainWindow.hpp"
#include "ui_AirfoilWidget.h"

namespace Designer
{

AirfoilWidget::AirfoilWidget(GeometrySurface* aSurfacePtr,
                             QWidget*         aParent,
                             Qt::WindowFlags  aFlags)
   : QDialog(aParent)
   , mUIPtr(new Ui::AirfoilWidget)
   , mAeroData(nullptr)
   , mGeometrySurfacePtr(aSurfacePtr)
{
   mUIPtr->setupUi(this);

   PopulateAirfoilList();

   connect(mUIPtr->mAirfoilComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
           this, &AirfoilWidget::HandleAirfoilChanged);
   connect(mUIPtr->mOKButton, &QPushButton::clicked, this, &AirfoilWidget::HandleSelectClicked);
   connect(mUIPtr->mCancelButton, &QPushButton::clicked, this, &QDialog::close);
   connect(mUIPtr->mPlotTypeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
           this, &AirfoilWidget::HandlePlotTypeChanged);

   HandleAirfoilChanged(); //initialize the spline for the first airfoil
}

AirfoilWidget::~AirfoilWidget()
{
   delete mUIPtr;
}

void AirfoilWidget::PopulateAirfoilList()
{
   QString dirPath = MoverCreatorMainWindow::GetDataPath() + "/Airfoils/";
   QDir listDir(dirPath);
   if (listDir.exists())
   {
      for (auto& curItem : listDir.entryInfoList())
      {
         if (curItem.isFile())
         {
            QString fileName = curItem.baseName();
            if (fileName != "." && fileName != "..")
            {
               mUIPtr->mAirfoilComboBox->addItem(fileName);
            }
         }
      }
   }

   if (mGeometrySurfacePtr != nullptr)
   {
      mUIPtr->mAirfoilComboBox->setCurrentText(mGeometrySurfacePtr->GetAirfoil());
   }
}

void AirfoilWidget::HandleAirfoilChanged()
{
   if (mAeroData == nullptr)
   {
      mAeroData = std::make_shared<AeroData>();
   }
   else
   {
      mAeroData.reset();
      mAeroData = std::make_shared<AeroData>();
   }
   mAeroData->LoadFromFile(mUIPtr->mAirfoilComboBox->currentText());
   HandlePlotTypeChanged();
}

void AirfoilWidget::UpdatePlot()
{
   mUIPtr->plotManagerWidget->ClearPlots();

   // Pair of two maps - one for the negative half and one for the positive half
   std::pair<std::map<double, double>, std::map<double, double>> rawData;

   if (mUIPtr->mPlotTypeComboBox->currentIndex() == cLIFT_VS_ALPHA)
   {
      rawData = mAeroData->GetRawLift();
   }
   else if (mUIPtr->mPlotTypeComboBox->currentIndex() == cDRAG_VS_ALPHA)
   {
      rawData = mAeroData->GetRawDrag();
   }
   else if (mUIPtr->mPlotTypeComboBox->currentIndex() == cMOMENT_VS_ALPHA)
   {
      rawData = mAeroData->GetRawMoment();
   }

   // Need to copy the data into a structure that the plot widget can use.
   // Concatenate the two maps together in a vector of pairs
   std::vector<std::pair<double, double>> plotData;
   std::copy(rawData.first.begin(), rawData.first.end(), std::back_inserter(plotData));
   std::copy(rawData.second.begin(), rawData.second.end(), std::back_inserter(plotData));

   PlotDataSet* dataSet = new PlotDataSet;
   dataSet->SetPlotData(std::move(plotData));

   mUIPtr->plotManagerWidget->AddPlot(dataSet);
}

void AirfoilWidget::HandleSelectClicked()
{
   if (mGeometrySurfacePtr != nullptr)
   {
      mGeometrySurfacePtr->SetAirfoil(mUIPtr->mAirfoilComboBox->currentText());
   }
   close();
}


void AirfoilWidget::HandlePlotTypeChanged()
{
   UpdatePlot();
}

} // namespace Designer
