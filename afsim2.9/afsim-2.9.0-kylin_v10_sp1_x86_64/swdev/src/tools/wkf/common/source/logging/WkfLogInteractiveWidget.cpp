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

#include "WkfLogInteractiveWidget.hpp"

#include <QCheckBox>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QToolButton>
#include <QTreeView>
#include <QWidget>

wkf::log::InteractiveWidget::InteractiveWidget(MasterModel& aModel, const ut::log::TypeList& aDefaults)
   : QWidget(nullptr)
   , mModel(aModel)
{
   using ut::log::Message;

   mUi.setupUi(this);
   mUi.treeView->setModel(&mModel);
   mUi.treeView->setHeaderHidden(true);

   // Set defaults
   for (const auto& type : aDefaults)
   {
      mModel.AddType(QString::fromStdString(type));

      if (type == Message::Fatal())
      {
         mUi.fatalCheckBox->setChecked(true);
      }
      else if (type == Message::Error())
      {
         mUi.errorCheckBox->setChecked(true);
      }
      else if (type == Message::Warning())
      {
         mUi.warningCheckBox->setChecked(true);
      }
      else if (type == Message::Info())
      {
         mUi.infoCheckBox->setChecked(true);
      }
      else if (type == Message::Debug())
      {
         mUi.debugCheckBox->setChecked(true);
      }
      else if (type == Message::Developer())
      {
         mUi.developerCheckBox->setChecked(true);
      }
   }

   // Setup error and warning counters
   connect(&aModel, &MasterModel::ErrorCountChanged, this, &InteractiveWidget::UpdateErrorCount);
   connect(&aModel, &MasterModel::WarningCountChanged, this, &InteractiveWidget::UpdateWarningCount);

   // Setup expand & collapse
   connect(mUi.expandButton,
           &QToolButton::clicked,
           [this]()
           {
              mUi.treeView->expandAll();
              mUi.treeView->SetExpandingByDefault(true);
           });
   connect(mUi.collapseButton,
           &QToolButton::clicked,
           [this]()
           {
              mUi.treeView->collapseAll();
              mUi.treeView->SetExpandingByDefault(false);
           });

   // Setup options button & widget
   mUi.optionsButton->setChecked(false);
   mUi.optionsWidget->hide();

   connect(mUi.optionsButton, &QPushButton::clicked, mUi.optionsWidget, &QWidget::setVisible);

   // Setup search bar
   connect(mUi.searchBar, &QLineEdit::textChanged, &mModel, &FilterModel::Find);

   // Connect other buttons
   ConnectFilterOptions();
   ConnectSortingOptions();
}

void wkf::log::InteractiveWidget::UpdateErrorCount(int aErrors)
{
   if (aErrors == 1)
   {
      mUi.errorCount->setText("1 Error");
   }
   else
   {
      mUi.errorCount->setText(QString("%1 Errors").arg(aErrors));
   }
}

void wkf::log::InteractiveWidget::UpdateWarningCount(int aWarnings)
{
   if (aWarnings == 1)
   {
      mUi.warningCount->setText("1 Warning");
   }
   else
   {
      mUi.warningCount->setText(QString("%1 Warnings").arg(aWarnings));
   }
}

void wkf::log::InteractiveWidget::ConnectFilterOptions() noexcept
{
   using ut::log::Message;

   auto applyFilter = [this](bool aChecked, const std::string& aType)
   {
      if (aChecked)
      {
         mModel.AddType(QString::fromStdString(aType));
      }
      else
      {
         mModel.RemoveType(QString::fromStdString(aType));
      }
   };

   connect(mUi.fatalCheckBox,
           &QCheckBox::clicked,
           [applyFilter](bool aChecked) { applyFilter(aChecked, Message::Fatal()); });

   connect(mUi.errorCheckBox,
           &QCheckBox::clicked,
           [applyFilter](bool aChecked) { applyFilter(aChecked, Message::Error()); });

   connect(mUi.warningCheckBox,
           &QCheckBox::clicked,
           [applyFilter](bool aChecked) { applyFilter(aChecked, Message::Warning()); });

   connect(mUi.infoCheckBox,
           &QCheckBox::clicked,
           [applyFilter](bool aChecked) { applyFilter(aChecked, Message::Info()); });

   connect(mUi.debugCheckBox,
           &QCheckBox::clicked,
           [applyFilter](bool aChecked) { applyFilter(aChecked, Message::Debug()); });

   connect(mUi.developerCheckBox,
           &QCheckBox::clicked,
           [applyFilter](bool aChecked) { applyFilter(aChecked, Message::Developer()); });
}

void wkf::log::InteractiveWidget::ConnectSortingOptions() noexcept
{
   connect(mUi.chronologicalButton,
           &QRadioButton::clicked,
           [this]() { mModel.SetSortingMethod(FilterSettings::Sorting::Chronological); });

   connect(mUi.typeButton, &QRadioButton::clicked, [this]() { mModel.SetSortingMethod(FilterSettings::Sorting::Type); });

   connect(mUi.alphabeticalButton,
           &QRadioButton::clicked,
           [this]() { mModel.SetSortingMethod(FilterSettings::Sorting::Alphabetical); });

   connect(mUi.reverseCheckBox, &QCheckBox::clicked, &mModel, &FilterModel::SetSortingReversed);
}
