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
#include "WkfSensorVolumesPrefWidget.hpp"

#include <assert.h>
#include <iostream>

#include <QCheckBox>
#include <QColorDialog>
#include <QInputDialog>

#include "UtQtColorButton.hpp"
#include "WkfSensorVolumesPrefObject.hpp"

namespace
{
class ColorAndEnable : public QWidget
{
public:
   ColorAndEnable(QWidget* aParent)
      : QWidget(aParent)
   {
      mCheckBox    = new QCheckBox(this);
      mColorButton = new UtQtColorButton(this);
      mColorButton->ShowAlpha(true);
      QHBoxLayout* layout = new QHBoxLayout(this);
      layout->addSpacing(10);
      layout->addWidget(mCheckBox);
      layout->addWidget(mColorButton);
      layout->setMargin(0);
   }
   void   setCheckState(bool aState) { mCheckBox->setChecked(aState); }
   void   setColor(const QColor& aColor) { mColorButton->SetColor(aColor); }
   bool   isChecked() const { return mCheckBox->isChecked(); }
   QColor getColor() const { return mColorButton->GetColor(); }

private:
   QCheckBox*       mCheckBox;
   UtQtColorButton* mColorButton;
};

void BuildRow(QTableWidget*  aWidget,
              const QString& aName,
              bool           aShowFaces,
              const QColor&  aFaceColor,
              bool           aShowEdges,
              const QColor&  aEdgeColor,
              bool           aShowProjections,
              const QColor&  aProjColor)
{
   int row = aWidget->rowCount();
   aWidget->insertRow(row);
   aWidget->setCellWidget(row, 0, new QLabel(aName, aWidget));

   auto* faces = new ColorAndEnable(aWidget);
   faces->setCheckState(aShowFaces);
   faces->setColor(aFaceColor);
   aWidget->setCellWidget(row, 1, faces);

   auto* edges = new ColorAndEnable(aWidget);
   edges->setCheckState(aShowEdges);
   edges->setColor(aEdgeColor);
   aWidget->setCellWidget(row, 2, edges);

   auto* projections = new ColorAndEnable(aWidget);
   projections->setCheckState(aShowProjections);
   projections->setColor(aProjColor);
   aWidget->setCellWidget(row, 3, projections);

   aWidget->resizeRowToContents(row);
}

void AddMode(QTableWidget* aWidget, const std::set<std::string>& aNames, wkf::SensorVolumesPrefWidget::EntryMethod aEntryMethod)
{
   QStringList undefinedModes;

   for (const auto& it : aNames)
   {
      bool found = false;
      for (int i = 0; i < aWidget->rowCount(); ++i)
      {
         auto label = dynamic_cast<QLabel*>(aWidget->cellWidget(i, 0));
         if (label)
         {
            if (label->text().toStdString() == it)
            {
               found = true;
               break;
            }
         }
      }
      if (!found)
      {
         undefinedModes.push_back(QString::fromStdString(it));
      }
   }

   undefinedModes.push_back("");

   bool    ok = true;
   QString mode;
   if (aEntryMethod == wkf::SensorVolumesPrefWidget::EntryMethod::cCOMBOBOX)
   {
      mode = QInputDialog::getItem(aWidget, "Select a mode", "Mode:", undefinedModes, 0, true, &ok);
   }
   else if (aEntryMethod == wkf::SensorVolumesPrefWidget::EntryMethod::cLINEEDIT)
   {
      mode = QInputDialog::getText(aWidget, "Enter a mode", "Mode:", QLineEdit::Normal, "", &ok);
   }
   if ((ok) && !mode.isEmpty())
   {
      int defaultMode = -1;
      for (int i = 0; i < aWidget->rowCount(); ++i)
      {
         auto label = dynamic_cast<QLabel*>(aWidget->cellWidget(i, 0));
         if (label)
         {
            if (label->text().toStdString() == "default")
            {
               defaultMode = i;
               break;
            }
         }
      }
      if (defaultMode >= 0)
      {
         auto faceW = dynamic_cast<ColorAndEnable*>(aWidget->cellWidget(defaultMode, 1));
         auto edgeW = dynamic_cast<ColorAndEnable*>(aWidget->cellWidget(defaultMode, 2));
         auto projW = dynamic_cast<ColorAndEnable*>(aWidget->cellWidget(defaultMode, 3));
         if (faceW && edgeW && projW)
         {
            BuildRow(aWidget,
                     mode,
                     faceW->isChecked(),
                     faceW->getColor(),
                     edgeW->isChecked(),
                     edgeW->getColor(),
                     projW->isChecked(),
                     projW->getColor());
         }
      }
   }
}

QString GetRowName(QTableWidget* aWidget, int aRow)
{
   auto label = dynamic_cast<QLabel*>(aWidget->cellWidget(aRow, 0));
   if (label)
   {
      return label->text();
   }
   return "";
}
} // namespace

wkf::SensorVolumesPrefWidget::SensorVolumesPrefWidget(EntryMethod aEntryMethod)
   : PrefWidgetT<SensorVolumesPrefObject>()
   , mEntryMethod(aEntryMethod)
{
   mUi.setupUi(this);

   mUi.sensorTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   mUi.weaponTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

   connect(mUi.addSensorButton, &QPushButton::clicked, this, &wkf::SensorVolumesPrefWidget::AddSensorMode);
   connect(mUi.deleteSensorButton, &QPushButton::clicked, this, &wkf::SensorVolumesPrefWidget::DeleteSensorMode);
   connect(mUi.addWeaponButton, &QPushButton::clicked, this, &wkf::SensorVolumesPrefWidget::AddWeaponMode);
   connect(mUi.deleteWeaponButton, &QPushButton::clicked, this, &wkf::SensorVolumesPrefWidget::DeleteWeaponMode);
   connect(mUi.sensorTableWidget, &QTableWidget::currentCellChanged, this, &wkf::SensorVolumesPrefWidget::SensorCellChanged);
   connect(mUi.weaponTableWidget, &QTableWidget::currentCellChanged, this, &wkf::SensorVolumesPrefWidget::WeaponCellChanged);
}

void wkf::SensorVolumesPrefWidget::WritePreferenceData(SensorVolumesPrefData& aPrefData)
{
   aPrefData.mModeMap.clear();

   auto writePrefHelper = [&aPrefData](int aRow, bool weapon, QTableWidget* aWidget)
   {
      auto label = dynamic_cast<QLabel*>(aWidget->cellWidget(aRow, 0));
      auto faceW = dynamic_cast<ColorAndEnable*>(aWidget->cellWidget(aRow, 1));
      auto edgeW = dynamic_cast<ColorAndEnable*>(aWidget->cellWidget(aRow, 2));
      auto projW = dynamic_cast<ColorAndEnable*>(aWidget->cellWidget(aRow, 3));

      if (label && faceW && edgeW && projW)
      {
         std::string name           = label->text().toStdString();
         bool        showFaces      = faceW->isChecked();
         QColor      faceColor      = faceW->getColor();
         bool        showEdges      = edgeW->isChecked();
         QColor      edgeColor      = edgeW->getColor();
         bool        showProjection = projW->isChecked();
         QColor      projColor      = projW->getColor();

         aPrefData.mModeMap.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(name, weapon),
            std::forward_as_tuple(showFaces, faceColor, showEdges, edgeColor, showProjection, projColor));
      }
   };

   for (int i = 0; i < mUi.sensorTableWidget->rowCount(); ++i)
   {
      writePrefHelper(i, false, mUi.sensorTableWidget);
   }

   for (int i = 0; i < mUi.weaponTableWidget->rowCount(); ++i)
   {
      writePrefHelper(i, true, mUi.weaponTableWidget);
   }

   if (mUi.mVolumeType->currentText() == "Slew Limits")
   {
      aPrefData.mDrawMode = SensorVolumesPrefData::cSLEW_VOLUME;
   }
   else if (mUi.mVolumeType->currentText() == "Cue Limits")
   {
      aPrefData.mDrawMode = SensorVolumesPrefData::cCUE_VOLUME;
   }
   else if (mUi.mVolumeType->currentText() == "Scan Limits")
   {
      aPrefData.mDrawMode = SensorVolumesPrefData::cSCAN_VOLUME;
   }
   else if (mUi.mVolumeType->currentText() == "Field of View")
   {
      aPrefData.mDrawMode = SensorVolumesPrefData::cFIELD_OF_VIEW;
   }
   else if (mUi.mVolumeType->currentText() == "Beam Width")
   {
      aPrefData.mDrawMode = SensorVolumesPrefData::cBEAM_WIDTH;
   }
   else
   {
      aPrefData.mDrawMode = SensorVolumesPrefData::cCALCULATED;
   }
}

void wkf::SensorVolumesPrefWidget::ReadPreferenceData(const SensorVolumesPrefData& aPrefData)
{
   mUi.sensorTableWidget->clearContents();
   mUi.sensorTableWidget->setRowCount(0);
   mUi.weaponTableWidget->clearContents();
   mUi.weaponTableWidget->setRowCount(0);

   for (const auto& it : aPrefData.mModeMap)
   {
      if (it.first.second)
      {
         BuildRow(mUi.weaponTableWidget,
                  QString::fromStdString(it.first.first),
                  it.second.mShowFaces,
                  it.second.mFaceColor,
                  it.second.mShowEdges,
                  it.second.mEdgeColor,
                  it.second.mShowProjection,
                  it.second.mProjColor);
      }
      else
      {
         BuildRow(mUi.sensorTableWidget,
                  QString::fromStdString(it.first.first),
                  it.second.mShowFaces,
                  it.second.mFaceColor,
                  it.second.mShowEdges,
                  it.second.mEdgeColor,
                  it.second.mShowProjection,
                  it.second.mProjColor);
      }
   }
   mUi.sensorTableWidget->resizeRowsToContents();
   mUi.weaponTableWidget->resizeRowsToContents();
   switch (aPrefData.mDrawMode)
   {
   case (SensorVolumesPrefData::cSLEW_VOLUME):
      mUi.mVolumeType->setCurrentText("Slew Limits");
      break;
   case (SensorVolumesPrefData::cCUE_VOLUME):
      mUi.mVolumeType->setCurrentText("Cue Limits");
      break;
   case (SensorVolumesPrefData::cSCAN_VOLUME):
      mUi.mVolumeType->setCurrentText("Scan Limits");
      break;
   case (SensorVolumesPrefData::cFIELD_OF_VIEW):
      mUi.mVolumeType->setCurrentText("Field of View");
      break;
   case (SensorVolumesPrefData::cBEAM_WIDTH):
      mUi.mVolumeType->setCurrentText("Beam Width");
      break;
   case (SensorVolumesPrefData::cCALCULATED):
      mUi.mVolumeType->setCurrentText("Calculated");
      break;
   }
}

void wkf::SensorVolumesPrefWidget::AddSensorMode()
{
   AddMode(mUi.sensorTableWidget, mSensorModeNameSet, mEntryMethod);
}

void wkf::SensorVolumesPrefWidget::DeleteSensorMode()
{
   mUi.sensorTableWidget->removeRow(mUi.sensorTableWidget->currentRow());
   mUi.deleteSensorButton->setEnabled(false);
}

void wkf::SensorVolumesPrefWidget::AddWeaponMode()
{
   AddMode(mUi.weaponTableWidget, mWeaponModeNameSet, mEntryMethod);
}

void wkf::SensorVolumesPrefWidget::DeleteWeaponMode()
{
   mUi.weaponTableWidget->removeRow(mUi.weaponTableWidget->currentRow());
   mUi.deleteSensorButton->setEnabled(false);
}

void wkf::SensorVolumesPrefWidget::SensorCellChanged(int aRow, int aColumn)
{
   if (GetRowName(mUi.sensorTableWidget, aRow) != "default")
   {
      mUi.deleteSensorButton->setEnabled(true);
   }
   else
   {
      mUi.deleteSensorButton->setEnabled(false);
   }
}

void wkf::SensorVolumesPrefWidget::WeaponCellChanged(int aRow, int aColumn)
{
   if (GetRowName(mUi.weaponTableWidget, aRow) != "default")
   {
      mUi.deleteWeaponButton->setEnabled(true);
   }
   else
   {
      mUi.deleteWeaponButton->setEnabled(false);
   }
}

void wkf::SensorVolumesPrefWidget::AddSensorModeName(const std::string& aName)
{
   if (aName != "default")
   {
      mSensorModeNameSet.insert(aName);
   }
}

void wkf::SensorVolumesPrefWidget::AddWeaponModeName(const std::string& aName)
{
   if (aName != "default")
   {
      mWeaponModeNameSet.insert(aName);
   }
}
