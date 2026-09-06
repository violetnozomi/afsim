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
#include "WkfEventMarkerPrefWidget.hpp"

#include <QFormLayout>
#include <QRadioButton>
#include <QTreeWidget>

#include "UtQtColorButton.hpp"
#include "WkfEnvironment.hpp"
#include "WkfEventMarkerPrefObject.hpp"

namespace
{
const QString cDOTMARKER  = "Dot Marker";
const QString cXMARKER    = "X Marker";
const QString cCUSTOMICON = "Custom Icon";
const QString cIMAGE      = "Image";
} // namespace

wkf::EventMarkerPrefWidget::EventMarkerPrefWidget()
   : wkf::PrefWidgetT<wkf::EventMarkerPrefObject>()
{
   setWindowTitle("Event Markers");

   QFormLayout* form = new QFormLayout(this);
   setLayout(form);
   auto* label     = new QLabel("Preferences on this page are shared among all AFSIM applications", this);
   QFont labelFont = label->font();
   labelFont.setBold(true);
   label->setFont(labelFont);
   form->addRow(label);

   mTimeout = new QSpinBox(this);
   mTimeout->setRange(1, 99999);
   form->addRow("Timeout (sec):", mTimeout);

   mMarkerScale = new QSlider(Qt::Horizontal, this);
   mMarkerScale->setMinimum(30);
   mMarkerScale->setMaximum(300);
   mMarkerScale->setValue(100);
   form->addRow("Marker Scale: ", mMarkerScale);

   mTreeWidget = new QTreeWidget(this);
   mTreeWidget->setHeaderHidden(true);
   mTreeWidget->setColumnCount(2);
   QSizePolicy policy = mTreeWidget->sizePolicy();
   policy.setVerticalStretch(1);
   mTreeWidget->setSizePolicy(policy);
   form->addRow(mTreeWidget);

   mItemMap["REMOVED"] = new QTreeWidgetItem(mTreeWidget->invisibleRootItem(), QStringList("Platform Removed"));
   PopulateOptions(mItemMap["REMOVED"]);

   mItemMap["DAMAGED"] = new QTreeWidgetItem(mTreeWidget->invisibleRootItem(), QStringList("Platform Damaged"));
   PopulateOptions(mItemMap["DAMAGED"]);

   mItemMap["WEAPON_HIT"] = new QTreeWidgetItem(mTreeWidget->invisibleRootItem(), QStringList("Weapon Hit"));
   PopulateOptions(mItemMap["WEAPON_HIT"]);

   mItemMap["WEAPON_MISSED"] = new QTreeWidgetItem(mTreeWidget->invisibleRootItem(), QStringList("Weapon Missed"));
   PopulateOptions(mItemMap["WEAPON_MISSED"]);

   connect(mPrefObjectPtr.get(),
           &EventMarkerPrefObject::NewEventTypeRegistered,
           [this](const QString& aName, const QColor& aColor)
           {
              mItemMap[aName] = new QTreeWidgetItem(mTreeWidget->invisibleRootItem(), QStringList(aName));
              PopulateOptions(mItemMap[aName]);
           });

   mTreeWidget->resizeColumnToContents(0);
}

void wkf::EventMarkerPrefWidget::PopulateOptions(QTreeWidgetItem* aItem)
{
   SetColor(aItem, Qt::red);
   aItem->setCheckState(0, Qt::Unchecked);

   QTreeWidgetItem* radioItem  = new QTreeWidgetItem();
   QFrame*          groupFrame = new QFrame(mTreeWidget);
   groupFrame->setObjectName(aItem->text(0));

   QRadioButton* radioDot = new QRadioButton(cDOTMARKER);
   radioDot->setObjectName(aItem->text(0) + cDOTMARKER);
   QRadioButton* radioX = new QRadioButton(cXMARKER);
   radioX->setObjectName(aItem->text(0) + cXMARKER);
   QRadioButton* radioCustom = new QRadioButton(cCUSTOMICON);
   radioCustom->setObjectName(aItem->text(0) + cCUSTOMICON);
   ImageSelectionWidget* imageSelector = new ImageSelectionWidget();
   imageSelector->setObjectName(aItem->text(0) + cIMAGE);

   imageSelector->setEnabled(false);
   connect(radioCustom, &QRadioButton::toggled, imageSelector, &ImageSelectionWidget::SetEnabledUpdate);

   QVBoxLayout* verticalBox = new QVBoxLayout;
   verticalBox->setContentsMargins(0, 0, 0, 0);
   verticalBox->addWidget(radioDot);
   verticalBox->addWidget(radioX);
   verticalBox->addWidget(radioCustom);
   verticalBox->addWidget(imageSelector);

   groupFrame->setLayout(verticalBox);
   aItem->addChild(radioItem);
   mTreeWidget->setItemWidget(radioItem, 0, groupFrame);
   mTreeWidget->setFirstItemColumnSpanned(radioItem, true);
}

wkf::MarkerType wkf::EventMarkerPrefWidget::GetRadioButtons(const QString& aParentName)
{
   QRadioButton* button = mTreeWidget->findChild<QRadioButton*>(aParentName + cDOTMARKER);
   if (button && button->isChecked())
   {
      return wkf::MarkerType::DOT;
   }
   button = mTreeWidget->findChild<QRadioButton*>(aParentName + cXMARKER);
   if (button && button->isChecked())
   {
      return wkf::MarkerType::X;
   }
   button = mTreeWidget->findChild<QRadioButton*>(aParentName + cCUSTOMICON);
   if (button && button->isChecked())
   {
      return wkf::MarkerType::CUSTOM;
   }
   // shouldn't reach here, since it should default to DOT
   return wkf::MarkerType::DOT;
}

void wkf::EventMarkerPrefWidget::SetRadioButton(const QString& aParentName, const MarkerType& aMarkerType)
{
   QRadioButton* button;
   switch (aMarkerType)
   {
   case wkf::MarkerType::X:
      button = mTreeWidget->findChild<QRadioButton*>(aParentName + cXMARKER);
      break;
   case wkf::MarkerType::CUSTOM:
      button = mTreeWidget->findChild<QRadioButton*>(aParentName + cCUSTOMICON);
      break;
   // use dot markings by default
   default:
      button = mTreeWidget->findChild<QRadioButton*>(aParentName + cDOTMARKER);
      break;
   }
   button->setChecked(true);
}

QString wkf::EventMarkerPrefWidget::GetIconPath(const QString& aParentName, bool& aOk)
{
   aOk                                = false;
   ImageSelectionWidget* customWidget = mTreeWidget->findChild<ImageSelectionWidget*>(aParentName + cIMAGE);
   if (customWidget)
   {
      aOk = customWidget->IsValid();
      return customWidget->GetText();
   }
   return "";
}

void wkf::EventMarkerPrefWidget::SetIconPath(const QString& aParentName, const QString& aIconPath)
{
   ImageSelectionWidget* customWidget = mTreeWidget->findChild<ImageSelectionWidget*>(aParentName + cIMAGE);
   if (customWidget)
   {
      customWidget->SetText(aIconPath);
   }
}

void wkf::EventMarkerPrefWidget::WritePreferenceData(EventMarkerPrefData& aPrefData)
{
   aPrefData.mTimeout     = mTimeout->value();
   aPrefData.mMarkerScale = (static_cast<float>(mMarkerScale->value())) / 100;

   bool ok = false;
   for (auto&& it : aPrefData.mEventData)
   {
      QString     tag  = QString::fromStdString(it.first);
      const auto& item = mItemMap.find(tag);
      if (item != mItemMap.end())
      {
         it.second.mShow       = (item->second->checkState(0) == Qt::Checked);
         it.second.mColor      = GetColor(item->second);
         it.second.mMarkerType = GetRadioButtons(item->second->text(0));
         it.second.mIconPath   = GetIconPath(item->second->text(0), ok);

         // If the user select Custom marker type, but the Icon Path was invalid,
         //  switch to using the Dot marker instead.
         if (it.second.mMarkerType == CUSTOM && !ok)
         {
            SetRadioButton(item->second->text(0), DOT);
            it.second.mMarkerType = DOT;
         }
      }
   }
}

void wkf::EventMarkerPrefWidget::ReadPreferenceData(const EventMarkerPrefData& aPrefData)
{
   mTimeout->setValue(aPrefData.mTimeout);
   mMarkerScale->setValue(static_cast<int>(aPrefData.mMarkerScale * 100));


   for (auto&& it : aPrefData.mEventData)
   {
      QString     tag  = QString::fromStdString(it.first);
      const auto& item = mItemMap.find(tag);
      if (item != mItemMap.end())
      {
         item->second->setCheckState(0, it.second.mShow ? Qt::Checked : Qt::Unchecked);
         SetColor(item->second, it.second.mColor);
         SetRadioButton(item->second->text(0), it.second.mMarkerType);
         SetIconPath(item->second->text(0), it.second.mIconPath);
      }
   }
}

QColor wkf::EventMarkerPrefWidget::GetColor(QTreeWidgetItem* aItem) const
{
   UtQtColorButton* colorButton = dynamic_cast<UtQtColorButton*>(mTreeWidget->itemWidget(aItem, 1));
   if (nullptr != colorButton)
   {
      return colorButton->GetColor();
   }
   else
   {
      return Qt::red;
   }
}

void wkf::EventMarkerPrefWidget::SetColor(QTreeWidgetItem* aItem, const QColor& aColor)
{
   UtQtColorButton* colorButton = dynamic_cast<UtQtColorButton*>(mTreeWidget->itemWidget(aItem, 1));
   if (nullptr == colorButton)
   {
      colorButton = new UtQtColorButton(this);
      mTreeWidget->setItemWidget(aItem, 1, colorButton);
   }
   if (nullptr != colorButton)
   {
      colorButton->SetColor(aColor);
   }
}

void wkf::EventMarkerPrefWidget::RegisterEvent(const QString& aName, const QColor& aColor)
{
   mItemMap[aName] = new QTreeWidgetItem(mTreeWidget->invisibleRootItem(), QStringList(aName));
   PopulateOptions(mItemMap[aName]);
}
