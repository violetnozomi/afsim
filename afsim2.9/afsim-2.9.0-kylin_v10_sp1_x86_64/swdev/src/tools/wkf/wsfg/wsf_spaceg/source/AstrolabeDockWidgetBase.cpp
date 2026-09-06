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

#include "AstrolabeDockWidgetBase.hpp"

#include <QMessageBox>

#include "AstrolabeConfig.hpp"
#include "OrbitalModelToSequence.hpp"
#include "UtLogSubscriber.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfScenario.hpp"

namespace wsfg
{

namespace spaceg
{

AstrolabeDockWidgetBase::AstrolabeDockWidgetBase(const AstrolabeConfig& aConfig, QWidget* aParentPtr, Qt::WindowFlags aFlags)
   : QDockWidget(aParentPtr, aFlags)
   , mCurrentModelPtr{nullptr}
   , mPreviewManager{}
{
   mUI.setupUi(this);

   if (aConfig.IsValid())
   {
      CreateConnections();
      mUI.mMissionView->Configure(aConfig);
      mUI.mDetailsPtr->ConfigureEventTypes(aConfig);
      UpdateActionButtons();
   }
   else
   {
      mUI.mMissionView->setVisible(false);
      mUI.mDetailsPtr->setVisible(false);
      mUI.mPlatformSelector->setVisible(false);
      mUI.mAcceptButton->setVisible(false);
      mUI.mVerifyButton->setVisible(false);
   }
}

//! Clear the status message.
void AstrolabeDockWidgetBase::ClearStatusMessage()
{
   mUI.mStatusLabel->setText(QString{});
}

//! Set an informational status message.
//!
//! Display a status message giving the user information. The provided
//! @p aMessage will be appended to a styled tag indicating that this is
//! an informational message.
//!
//! \param aMessage - The text of the informative message.
void AstrolabeDockWidgetBase::SetStatusMessage(const QString& aMessage)
{
   QString decoratedMessage = QString{"<span style = \"color: green\">NOTE:</span> "} + aMessage;
   mUI.mStatusLabel->setText(decoratedMessage);
}

//! Set a warning status message.
//!
//! Display a status message warning the user. The provided @p aMessage
//! will be appended to a styled tag indicating that this is a
//! warning message.
//!
//! \param aMessage - The text of the warning message.
void AstrolabeDockWidgetBase::SetWarningMessage(const QString& aMessage)
{
   QString decoratedMessage = QString{"<span style = \"color: yellow\">WARNING:</span> "} + aMessage;
   mUI.mStatusLabel->setText(decoratedMessage);
}

//! Set an error status message.
//!
//! Display a status message indicating an error. The provided @p aMessage
//! will be appended to a styled tag indicating that this is an error.
//!
//! \param aMessage - The text of the error message.
void AstrolabeDockWidgetBase::SetErrorMessage(const QString& aMessage)
{
   QString decoratedMessage = QString{"<span style = \"color: red\">ERROR:</span> "} + aMessage;
   mUI.mStatusLabel->setText(decoratedMessage);
}

//! Return the name of the selected platform.
//!
//! This is the platform for which Astrolabe is displaying and editing a
//! mission sequence.
//!
//! \returns The name of the selected platform.
QString AstrolabeDockWidgetBase::GetSelectedPlatform() const
{
   return mUI.mPlatformSelector->GetSelectedPlatform();
}

//! Return the name of the platform requested by the user.
//!
//! This is the platform that the user has selected with the platform
//! selector. The distinction arises because between when a user
//! selects a new platform to display with Astrolabe, there may be
//! some work that must occur, such as verifying that the user
//! wants to discard any current changes for the selected platform.
//!
//! \returns The name of the requested platform.
QString AstrolabeDockWidgetBase::GetRequestedPlatform() const
{
   return mUI.mPlatformSelector->GetRequestedPlatform();
}

//! Set the results of mission sequence verification.
//!
//! This will save the results of verification of the mission sequence and
//! display those results in the mission sequence view.
//!
//! \param aResults - The results of the mission sequence verification.
void AstrolabeDockWidgetBase::SetMissionVerificationResults(const MissionVerificationResults& aResults)
{
   mCurrentModelPtr->SetMissionVerificationResults(aResults);
   SetStatusMessage(QString{"Mission sequence verification completed."});
   mPreviewManager.RecreateOrbitPreviews(*mCurrentModelPtr);
   mPreviewManager.UpdateOrbitPreviews(*mCurrentModelPtr, mUI.mMissionView->CurrentIndex());
}

//! Do a pre-check of the mission event parameters before running the full verification.
//!
//! This will return true if all of the mission events have parameters that are in
//! their allowed ranges, or false otherwise. If any event does not pass this pre-check,
//! then the errors will be presented to the user and the offending event will be raised
//! in the UI.
//!
//! \param aSequencePtr - the sequence with event parameters that should be checked.
//! \returns true if all events pass initial verification; false otherwise.
bool AstrolabeDockWidgetBase::CheckEventParameters(std::unique_ptr<WsfOrbitalMissionSequence> aSequencePtr)
{
   if (!aSequencePtr)
   {
      return false;
   }

   // 1) Run the verification step using the sequence's pre-order traversal
   QModelIndex errorIndex{};
   QString     errorText{};
   std::string executingPlatform = mUI.mPlatformSelector->GetSelectedPlatform().toStdString();
   int         orderedIndex{0};
   auto operation = [&errorIndex, &errorText, &executingPlatform, &orderedIndex, this](const WsfOrbitalEvent* aEventPtr) -> bool
   {
      ut::log::Subscriber consoleSub{"raw"};
      bool                retval = aEventPtr->ValidateParameterRanges(executingPlatform);
      if (!retval)
      {
         errorIndex = mCurrentModelPtr->GetIndexFromOrderedIndex(orderedIndex);
         errorText.clear();
         auto msgs = consoleSub.TakeQueue();
         while (!msgs.empty())
         {
            // Because consoleSub is only subscribed to "raw", there should be no notes.
            errorText += QString::fromStdString(msgs.front().mData);
            msgs.pop();
         }
      }
      ++orderedIndex;
      return retval;
   };

   bool retval = aSequencePtr->PreOrderTraversal(operation);

   // 2) If the pre-order traversal does not complete, raise the offending event and display the error.
   if (!retval)
   {
      SetErrorMessage(QString{"Event parameters failed initial verification."});
      mUI.mMissionView->SetCurrentIndex(errorIndex);
      QMessageBox::warning(nullptr, QString("Event Parameter Error"), errorText, QMessageBox::Ok);
   }

   return retval;
}

//! Rearrange the contents of the dock when it is docked and undocked.
void AstrolabeDockWidgetBase::HandleTopLevelChanged(bool aTopLevel)
{
   if (aTopLevel)
   {
      QSize currentSize = size();
      QSize nextSize{2 * currentSize.width(), currentSize.height() / 2};
      mUI.mOverallLayout->setDirection(QBoxLayout::LeftToRight);
      mUI.mOverallLayout->setStretch(0, 4);
      mUI.mOverallLayout->setStretch(1, 5);
      resize(nextSize);
   }
   else
   {
      mUI.mOverallLayout->setDirection(QBoxLayout::TopToBottom);
      mUI.mOverallLayout->setStretch(0, 1);
      mUI.mOverallLayout->setStretch(1, 1);
   }
}

//! Hide or recreate orbit previews when the dock's visibility is changed.
void AstrolabeDockWidgetBase::HandleVisibilityChanged(bool aIsVisible)
{
   if (mCurrentModelPtr)
   {
      if (aIsVisible)
      {
         mPreviewManager.RecreateOrbitPreviews(*mCurrentModelPtr);
         mPreviewManager.UpdateOrbitPreviews(*mCurrentModelPtr, mUI.mMissionView->CurrentIndex());
      }
      else
      {
         mPreviewManager.ClearOrbitPreviews();
      }
   }
}

//! Respond to a change in the selected event.
//!
//! This will change the UI to display the details of the event at the selected @p aCurrentIndex
//! and will save any current modification of that event if @p aSaveModifications is true.
//!
//! \param aCurrentIndex      - The model index of the newly selected event.
//! \param aSaveModifications - true if any modifications to the currently selected event should be saved.
void AstrolabeDockWidgetBase::HandleCurrentChanged(const QModelIndex& aCurrentIndex, bool aSaveModifications)
{
   mUI.mDetailsPtr->SetCurrentItem(aCurrentIndex, aSaveModifications);
   mPreviewManager.UpdateOrbitPreviews(*mCurrentModelPtr, aCurrentIndex);
}

void AstrolabeDockWidgetBase::CreateConnections()
{
   connect(mUI.mVerifyButton, &QPushButton::clicked, this, &AstrolabeDockWidgetBase::OnVerify);
   connect(mUI.mAcceptButton, &QPushButton::clicked, this, &AstrolabeDockWidgetBase::OnApply);
   connect(mUI.mPlatformSelector, &PlatformSelector::PlatformRequested, this, &AstrolabeDockWidgetBase::PlatformSelected);

   connect(mUI.mMissionView,
           &wsfg::spaceg::AstrolabeMissionView::CurrentChanged,
           this,
           &AstrolabeDockWidgetBase::HandleCurrentChanged);
   connect(this, &QDockWidget::topLevelChanged, this, &AstrolabeDockWidgetBase::HandleTopLevelChanged);
   connect(this, &QDockWidget::visibilityChanged, this, &AstrolabeDockWidgetBase::HandleVisibilityChanged);
}

void AstrolabeDockWidgetBase::UpdateActionButtons()
{
   bool enabled = mCurrentModelPtr != nullptr;
   mUI.mAcceptButton->setEnabled(enabled);
   mUI.mVerifyButton->setEnabled(enabled);
}

//! Respond to the user clicking the Verify button.
//!
//! This is a template method that handles the common functionality of verification. The specific
//! actions to be taken by subclasses are handled in the InitialVerifyAction method.
void AstrolabeDockWidgetBase::OnVerify()
{
   ClearStatusMessage();
   mUI.mDetailsPtr->SaveCurrentModifications();
   InitialVerifyAction();
}

//! Finish the verification process.
//!
//! This is a template method that handles the common functionality of verification. The specific
//! actions to be taken by subclasses in the VerifyAction method.
//!
//! \param aSequencePtr - the mission sequence that is to be verified.
void AstrolabeDockWidgetBase::FinishVerify(std::unique_ptr<WsfOrbitalMissionSequence> aSequencePtr)
{
   if (CheckEventParameters(std::move(aSequencePtr)))
   {
      VerifyAction();
   }
}

//! Respond to the user clicking the Apply button.
//!
//! This is a template method that handles the common functionality of accepting mission sequences.
//! The specific actions to be taken by subclasses are handled in the InitialApplyAction method.
void AstrolabeDockWidgetBase::OnApply()
{
   ClearStatusMessage();
   mUI.mDetailsPtr->SaveCurrentModifications();
   InitialApplyAction();
}

//! Finish the apply action.
//!
//! This is a template method that handles the common functionality of accepting mission sequences.
//! The specific actions to be taken by subclasses are handles in the ApplyAction method.
//!
//! \param aSequencePtr - the mission sequence that is to be applied.
void AstrolabeDockWidgetBase::FinishApply(std::unique_ptr<WsfOrbitalMissionSequence> aSequencePtr)
{
   if (CheckEventParameters(std::move(aSequencePtr)))
   {
      ApplyAction();
   }
}

} // namespace spaceg

} // namespace wsfg
