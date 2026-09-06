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

#include "AstrolabeEventDetails.hpp"

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStackedLayout>
#include <QToolButton>
#include <QVBoxLayout>

#include "AstrolabeConfig.hpp"
#include "ConfigWidget.hpp"
#include "OrbitalMissionModel.hpp"

namespace wsfg
{

namespace spaceg
{

AstrolabeEventDetails::AstrolabeEventDetails(QWidget* aParentPtr)
   : mDetailPanels{}
   , mIndexToName{}
   , mLayoutPtr{new QStackedLayout{}}
   , mEventLabelPtr{new QLabel{}}
   , mRevertButtonPtr{new QToolButton{}}
   , mBlankWidgetPtr{new QWidget{}}
   , mModelPtr{nullptr}
   , mCurrentItem{}
{
   QAction* revertActionPtr = new QAction{QIcon::fromTheme("undo"), QString("Revert Event Modifications"), this};
   connect(revertActionPtr, &QAction::triggered, this, &AstrolabeEventDetails::Revert);
   mRevertButtonPtr->setDefaultAction(revertActionPtr);
   mRevertButtonPtr->setVisible(false);

   QHBoxLayout* topLayoutPtr = new QHBoxLayout{};
   topLayoutPtr->addWidget(mEventLabelPtr, 1);
   topLayoutPtr->addWidget(mRevertButtonPtr, 0);

   QVBoxLayout* overallPtr = new QVBoxLayout{};
   overallPtr->addLayout(topLayoutPtr, 0);
   overallPtr->addLayout(mLayoutPtr, 1);
   overallPtr->setContentsMargins(QMargins{0, 0, 0, 0});

   mLayoutPtr->addWidget(mBlankWidgetPtr);

   setLayout(overallPtr);
}

//! Configure the event types for use with this widget.
//!
//! This method will use the provided @p aConfiguration to create the specific ConfigWidget
//! subclasses that will serve as display and editors for the event types supported by
//! Astrolabe.
//!
//! \param aConfiguration - the configuration of Astrolabe.
void AstrolabeEventDetails::ConfigureEventTypes(const AstrolabeConfig& aConfiguration)
{
   for (int i = 0; i < aConfiguration.NumEventTypes(); ++i)
   {
      ConfigWidget* widgetPtr = aConfiguration.CreateEventConfigWidget(i);
      QScrollArea*  scrollPtr = new QScrollArea{};
      scrollPtr->setWidgetResizable(true);
      scrollPtr->setWidget(widgetPtr);
      mDetailPanels[aConfiguration.EventName(i)] = PanelInfo{i + 1, widgetPtr, aConfiguration.EventDisplay(i)};
      mIndexToName[i + 1]                        = aConfiguration.EventName(i);
      mLayoutPtr->addWidget(scrollPtr);
   }
}

//! Set the OrbitalMissionModel currently being modified by Astrolabe.
void AstrolabeEventDetails::SetModel(OrbitalMissionModel* aModelPtr)
{
   mModelPtr = aModelPtr;
   SetCurrentItem(QModelIndex{});
}

//! Set the currently displayed item.
//!
//! This object can only display a single event at a time. When a different event is loaded,
//! if the currently displayed event has been modified, the modifications will be saved to the
//! model if @p aSaveModifications is true.
//!
//! \param aIndex            - the model index of the item to display.
//! \param aSaveModification - true if any modifications to the currently displayed event should be retained.
void AstrolabeEventDetails::SetCurrentItem(const QModelIndex& aIndex, bool aSaveModification)
{
   if (mModelPtr != nullptr)
   {
      if (aSaveModification)
      {
         SaveCurrentModifications();
      }

      if (aIndex.isValid())
      {
         QString eventName = SetPanelValues(aIndex);
         if (!eventName.isEmpty())
         {
            SetEventName(eventName);
            mCurrentItem = aIndex;
         }
      }
      else
      {
         SetEventName(QString{});
      }
   }
}

//! Revert any changes to the current item since it was loaded.
void AstrolabeEventDetails::Revert()
{
   if (mModelPtr != nullptr)
   {
      mModelPtr->RevertItem(mCurrentItem);
      SetPanelValues(mCurrentItem);
   }
}

//! Save any changes to the current item since it was loaded.
void AstrolabeEventDetails::SaveCurrentModifications()
{
   if (mCurrentItem.isValid())
   {
      int curr = mLayoutPtr->currentIndex();
      if (curr != 0)
      {
         auto nameIter = mIndexToName.find(curr);
         if (nameIter != mIndexToName.end())
         {
            auto panelIter = mDetailPanels.find(nameIter->second);
            if (panelIter != mDetailPanels.end())
            {
               QJsonObject values        = panelIter->second.mConfigWidgetPtr->GetValues();
               values[QString{"option"}] = panelIter->first.toLower();
               mModelPtr->SetModifiedData(mCurrentItem, values);
            }
         }
      }
   }
}

//! Update the displayed event name.
void AstrolabeEventDetails::SetEventName(const QString& aName)
{
   auto iter = mDetailPanels.find(aName);
   if (iter != mDetailPanels.end())
   {
      mLayoutPtr->setCurrentIndex(iter->second.mPanelIndex);
      mEventLabelPtr->setText(iter->second.mDisplayName);
      mRevertButtonPtr->setVisible(true);
   }
   else
   {
      mLayoutPtr->setCurrentWidget(mBlankWidgetPtr);
      mEventLabelPtr->setText(QString{});
      mRevertButtonPtr->setVisible(false);
   }
}

//! Set the values displayed from the event with the given index.
//!
//! Set the panel values displayed in the current widget to those for the model item
//! with the given @p aIndex.
//!
//! \param aIndex - The index to the item that should have it's values displayed.
//! \returns      - The name of the event type for the model item in question.
QString AstrolabeEventDetails::SetPanelValues(const QModelIndex& aIndex)
{
   QString retval{};
   QString eventType = mModelPtr->GetItemEventType(aIndex);
   auto    panelIter = mDetailPanels.find(eventType);
   if (panelIter != mDetailPanels.end())
   {
      panelIter->second.mConfigWidgetPtr->ClearValues();
      panelIter->second.mConfigWidgetPtr->SetValues(mModelPtr->GetItemData(aIndex));
      retval = eventType;
   }
   return retval;
}

} // namespace spaceg

} // namespace wsfg
