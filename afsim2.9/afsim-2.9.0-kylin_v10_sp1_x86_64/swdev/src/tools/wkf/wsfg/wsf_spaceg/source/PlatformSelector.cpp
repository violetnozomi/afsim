// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "PlatformSelector.hpp"

#include <QAction>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QStringListModel>

#include "WkfPlatform.hpp"
#include "WkfSelectorWidgets.hpp"

namespace wsfg
{

namespace spaceg
{

PlatformSelector::PlatformSelector(QWidget* aParentPtr)
   : QWidget(aParentPtr)
   , mComboBoxPtr{new QComboBox{this}}
   , mModelPtr{new QStringListModel{this}}
   , mSelectorButtonPtr{new wkf::PlatformSelector{this}}
{
   mComboBoxPtr->setModel(mModelPtr);
   mComboBoxPtr->setCurrentIndex(-1);

   connect(mComboBoxPtr, QOverload<int>::of(&QComboBox::activated), this, &PlatformSelector::OnSelectionAccepted);
   connect(mSelectorButtonPtr, &wkf::PlatformSelector::Selected, this, &PlatformSelector::OnPlatformSelectionTool);

   QHBoxLayout* layoutPtr = new QHBoxLayout{};
   layoutPtr->setMargin(0);
   layoutPtr->addWidget(mComboBoxPtr, 1);
   layoutPtr->addWidget(mSelectorButtonPtr, 0);
   setLayout(layoutPtr);
}

//! Return the name of the platform requested by the user.
QString PlatformSelector::GetRequestedPlatform() const
{
   return mComboBoxPtr->currentText();
}

// Note that the intended operation is that this is called externally. The two phases of
// selection is because we need to be able to allow the owner of this widget to reject the
// requested change.

//! Select the given platform.
//!
//! Set the platform selection to the platform with the given @p aPlatformName.
//! This method is intended to be called externally. There is a two-phase platform
//! selection process. First the user requests a platform's sequence be loaded into
//! Astrolabe. Then, the platform is loaded. This is part of the second phase.
//! The two phase platform selection is to allow for Astrolabe to notice unsaved
//! changes to the current mission sequence, and allow the user to save them, ignore
//! those changes, or cancel the change of platform.
//!
//! \param aPlatformName - The name of the platform that was requested and accepted.
void PlatformSelector::SelectPlatform(const QString& aPlatformName)
{
   int index = mComboBoxPtr->findText(aPlatformName);
   if (index != -1)
   {
      mComboBoxPtr->setCurrentIndex(index);
      mSelectedPlatform = aPlatformName;
   }
   if (aPlatformName.isEmpty())
   {
      mSelectedPlatform = aPlatformName;
   }
}

//! Set the list of available space-domain platforms.
void PlatformSelector::SetPlatformList(const QStringList& aPlatformList)
{
   mModelPtr->setStringList(aPlatformList);
   int index = mComboBoxPtr->findText(mSelectedPlatform);
   if (index != -1)
   {
      mComboBoxPtr->setCurrentIndex(index);
   }
}

//! Add a space-domain platform to the list of available platforms that Astrolabe can select.
void PlatformSelector::PlatformAdded(const QString& aAddedPlatformName)
{
   QStringList modList = mModelPtr->stringList();
   if (!modList.contains(aAddedPlatformName))
   {
      modList << aAddedPlatformName;
      SetPlatformList(modList);
   }
}

//! Remove a platform from the list of available space-domain platforms.
void PlatformSelector::PlatformRemoved(const QString& aRemovedPlatformName)
{
   QStringList modList = mModelPtr->stringList();
   int         index   = modList.indexOf(aRemovedPlatformName);
   if (index != -1)
   {
      modList.removeAt(index);
      SetPlatformList(modList);
   }
}

void PlatformSelector::OnSelectionAccepted(int /*aIndex*/)
{
   QString selection = GetRequestedPlatform();
   if (selection != GetSelectedPlatform())
   {
      emit PlatformRequested(selection);
   }
}

void PlatformSelector::OnPlatformSelectionTool(const wkf::Platform* aPlatformPtr)
{
   if (aPlatformPtr)
   {
      QString pickedPlatform = QString::fromStdString(aPlatformPtr->GetName());
      int     index          = mComboBoxPtr->findText(pickedPlatform);
      if (index != -1)
      {
         mComboBoxPtr->setCurrentIndex(index);
         OnSelectionAccepted(index);
      }
   }
}

} // namespace spaceg

} // namespace wsfg
