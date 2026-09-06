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

#include "AstrolabeMissionView.hpp"

#include <QMenu>
#include <QToolButton>

#include "AstrolabeConfig.hpp"
#include "OrbitalMissionModel.hpp"
#include "OrbitalMissionView.hpp"

namespace wsfg
{

namespace spaceg
{

AstrolabeMissionView::AstrolabeMissionView(QWidget* aParentPtr)
   : QWidget{aParentPtr}
{
   mUi.setupUi(this);
}

//! Configure this view.
//!
//! This will configure this object according to the supplied @p aConfig. For
//! example, this will set up the toolbar buttons, which have entries that depend
//! on the details of the configuration.
//!
//! \param aConfig - The Astrolabe configuration.
void AstrolabeMissionView::Configure(const AstrolabeConfig& aConfig)
{
   CreateActions(aConfig);
   CreateConnections();
   mUi.mMissionView->Configure(mTransformEventActions, mMoveEventUpActionPtr, mMoveEventDownActionPtr, mRemoveEventActionPtr);
   ConfigureToolButtons(aConfig);

   UpdateToolbar(QModelIndex{});
}

//! Set the model this view is displaying.
void AstrolabeMissionView::SetModel(OrbitalMissionModel* aModelPtr)
{
   mCurrentModelPtr = aModelPtr;
   mUi.mMissionView->setModel(aModelPtr);
   mUi.mMissionView->expandAll();
   UpdateToolbar(QModelIndex{});
}

//! Return the current index selected in the view.
QModelIndex AstrolabeMissionView::CurrentIndex() const
{
   return mUi.mMissionView->currentIndex();
}

//! Set the currently selected index.
void AstrolabeMissionView::SetCurrentIndex(const QModelIndex& aIndex)
{
   mUi.mMissionView->setCurrentIndex(aIndex);
}

void AstrolabeMissionView::CreateActions(const AstrolabeConfig& aConfig)
{
   mMoveEventUpActionPtr   = new QAction{QIcon::fromTheme("up"), QString{"Move Event Up"}, this};
   mMoveEventDownActionPtr = new QAction{QIcon::fromTheme("down"), QString{"Move Event Down"}, this};
   mRemoveEventActionPtr   = new QAction{QIcon::fromTheme("delete"), QString{"Remove Event"}, this};

   for (int i = 0; i < aConfig.NumEventTypes(); ++i)
   {
      QString typeName = aConfig.EventName(i);
      if (aConfig.EventIsAddable(i))
      {
         QAction* addAction         = new QAction{aConfig.EventDisplay(i), this};
         mAddEventActions[typeName] = addAction;
      }
      if (aConfig.EventIsTransformationTarget(i))
      {
         QAction* transformAction         = new QAction{aConfig.EventDisplay(i), this};
         mTransformEventActions[typeName] = transformAction;
      }
   }
}

void AstrolabeMissionView::CreateConnections()
{
   connect(mMoveEventUpActionPtr, &QAction::triggered, this, &AstrolabeMissionView::MoveEventUp);
   connect(mMoveEventDownActionPtr, &QAction::triggered, this, &AstrolabeMissionView::MoveEventDown);
   connect(mRemoveEventActionPtr, &QAction::triggered, this, &AstrolabeMissionView::RemoveEvent);

   for (auto& iter : mAddEventActions)
   {
      QString eventType = iter.first;
      connect(iter.second, &QAction::triggered, [this, eventType]() { AddEvent(eventType); });
   }

   for (auto& iter : mTransformEventActions)
   {
      QString eventType = iter.first;
      connect(iter.second, &QAction::triggered, [this, eventType]() { TransformEvent(eventType); });
   }

   connect(mUi.mMissionView,
           &OrbitalMissionView::CurrentChanged,
           [this](const QModelIndex& aCurrent) { HandleCurrentChanged(aCurrent, true); });
}

void AstrolabeMissionView::ConfigureToolButtons(const AstrolabeConfig& aConfig)
{
   QMenu* addMenuPtr = new QMenu{mUi.mAddEventButton};
   for (auto& iter : mAddEventActions)
   {
      addMenuPtr->addAction(iter.second);
   }
   mUi.mAddEventButton->setMenu(addMenuPtr);
   mUi.mAddEventButton->setPopupMode(QToolButton::InstantPopup);
   mUi.mAddEventButton->setIcon(QIcon::fromTheme("add"));

   QMenu* transformMenuPtr = new QMenu{this};
   for (auto& iter : mTransformEventActions)
   {
      transformMenuPtr->addAction(iter.second);
   }
   mUi.mTransformEventButton->setMenu(transformMenuPtr);
   mUi.mTransformEventButton->setPopupMode(QToolButton::InstantPopup);
   mUi.mTransformEventButton->setIcon(QIcon::fromTheme("configure"));

   mUi.mMoveEventUpButton->setDefaultAction(mMoveEventUpActionPtr);
   mUi.mMoveEventDownButton->setDefaultAction(mMoveEventDownActionPtr);
   mUi.mRemoveEventButton->setDefaultAction(mRemoveEventActionPtr);
}

void AstrolabeMissionView::AddEvent(const QString& aType)
{
   if (mCurrentModelPtr != nullptr)
   {
      QModelIndex addedIndex = mCurrentModelPtr->AppendEvent(aType);
      if (aType == "COMPOUND")
      {
         mUi.mMissionView->expand(addedIndex);
      }
      mUi.mMissionView->setCurrentIndex(addedIndex);
   }
}

void AstrolabeMissionView::RemoveEvent()
{
   if (mCurrentModelPtr != nullptr)
   {
      QModelIndex current = mUi.mMissionView->currentIndex();
      if (mCurrentModelPtr->IndexIsRemovable(current))
      {
         mUi.mMissionView->setCurrentIndex(QModelIndex{});
         if (mCurrentModelPtr->RemoveEvent(current))
         {
            UpdateToolbar(mUi.mMissionView->currentIndex());
         }
      }
   }
}

void AstrolabeMissionView::MoveEventUp()
{
   if (mCurrentModelPtr != nullptr)
   {
      QModelIndex currentIndex = mUi.mMissionView->currentIndex();
      mUi.mMissionView->setCurrentIndex(QModelIndex{});
      currentIndex = mCurrentModelPtr->MoveEventUp(currentIndex);
      mUi.mMissionView->setCurrentIndex(currentIndex);
      mUi.mMissionView->expand(currentIndex);
      UpdateToolbar(currentIndex);
   }
}

void AstrolabeMissionView::MoveEventDown()
{
   if (mCurrentModelPtr != nullptr)
   {
      QModelIndex currentIndex = mUi.mMissionView->currentIndex();
      mUi.mMissionView->setCurrentIndex(QModelIndex{});
      currentIndex = mCurrentModelPtr->MoveEventDown(currentIndex);
      mUi.mMissionView->setCurrentIndex(currentIndex);
      mUi.mMissionView->expand(currentIndex);
      UpdateToolbar(currentIndex);
   }
}

void AstrolabeMissionView::TransformEvent(const QString& aType)
{
   if (mCurrentModelPtr != nullptr)
   {
      QModelIndex current = mUi.mMissionView->currentIndex();
      bool        ok      = mCurrentModelPtr->TransformEvent(current, aType);
      if (ok)
      {
         if (aType == "COMPOUND")
         {
            mUi.mMissionView->expand(current);
         }
      }
      HandleCurrentChanged(current, false);
   }
}

void AstrolabeMissionView::UpdateToolbar(const QModelIndex& aIndex)
{
   bool selected{false};
   bool canMoveUp{false};
   bool canMoveDown{false};
   bool canBeRemoved{false};
   bool canBeTransformed{false};

   if (mCurrentModelPtr)
   {
      selected         = aIndex.isValid();
      canMoveUp        = !mCurrentModelPtr->IndexIsFirstChild(aIndex);
      canMoveDown      = !mCurrentModelPtr->IndexIsLastChild(aIndex);
      canBeRemoved     = mCurrentModelPtr->IndexIsRemovable(aIndex);
      canBeTransformed = mCurrentModelPtr->IndexIsTransformable(aIndex);
   }

   mUi.mAddEventButton->setEnabled(mCurrentModelPtr);
   mUi.mTransformEventButton->setEnabled(selected && canBeTransformed);
   mMoveEventUpActionPtr->setEnabled(selected && canMoveUp);
   mMoveEventDownActionPtr->setEnabled(selected && canMoveDown);
   mRemoveEventActionPtr->setEnabled(selected && canBeRemoved);
}

void AstrolabeMissionView::HandleCurrentChanged(const QModelIndex& aCurrent, bool aSaveModifications)
{
   UpdateToolbar(aCurrent);
   emit CurrentChanged(aCurrent, aSaveModifications);
}

} // namespace spaceg

} // namespace wsfg
