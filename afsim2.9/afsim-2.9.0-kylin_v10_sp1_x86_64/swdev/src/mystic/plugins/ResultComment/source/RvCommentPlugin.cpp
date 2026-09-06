// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvCommentPlugin.hpp"

#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtTime.hpp"
#include "VaTimeManager.hpp"
#include "VaUtils.hpp"
#include "WkfMainWindow.hpp"
#include "WkfPrefWidget.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfUnitsObject.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "annotation/WkfAttachmentDecorator.hpp"
#include "comment/WkfCommentForwardingService.hpp"
#include "comment/WkfCommentPrefObject.hpp"
#include "comment/WkfCommentPrefWidget.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvComment::Plugin,
                          "Comment Bubbles",
                          "Processes comment events from AFSIM and displays comment bubbles on the map display",
                          "mystic")

void RvComment::Plugin::CommentRead(rv::ResultDb& aData, const rv::MsgComment& aDrawCommand)
{
   rv::ResultPlatform* plat = aData.FindPlatform(aDrawCommand.platformIndex());
   if (plat)
   {
      wkf::CommentForwardingService::CommentReceived(aDrawCommand.simTime(),
                                                     plat->GetName().c_str(),
                                                     aDrawCommand.text().c_str());

      CommentData::iterator comments = mComments.find(aDrawCommand.simTime());
      if (comments != mComments.end())
      {
         std::vector<std::pair<unsigned int, rv::MsgComment>>::iterator c =
            std::find_if(comments->second.begin(), comments->second.end(), FindCommentByIndex(aDrawCommand));
         if (c != comments->second.end())
         {
            c->second = aDrawCommand;
         }
         else
         {
            comments->second.emplace_back(0, aDrawCommand);
         }
      }
      else
      {
         std::vector<std::pair<unsigned int, rv::MsgComment>> vectorComments;
         vectorComments.emplace_back(0, aDrawCommand);
         mComments.emplace(aDrawCommand.simTime(), vectorComments);
      }
   }
}

void RvComment::Plugin::ClearComments(bool aFullReset)
{
   wkf::CommentForwardingService::CommentClear();
   mComments.clear();
}

RvComment::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mPrefWidgetPtr(new wkf::CommentPrefWidget)
   , mDockWidgetPtr(new wkf::CommentDockWidget(wkfEnv.GetMainWindow()))
{
   wkfEnv.GetMainWindow()->addDockWidget(Qt::BottomDockWidgetArea, mDockWidgetPtr);

   // If they disabled comments in the preferences, remove all comments.
   connect(mPrefWidgetPtr->GetPreferenceObject(), &wkf::CommentPrefObject::EnabledChanged, this, &Plugin::PreferencesChanged);
   connect(mPrefWidgetPtr->GetPreferenceObject(),
           &wkf::CommentPrefObject::ShowTimestampChanged,
           this,
           &Plugin::PreferencesChanged);
   connect(mPrefWidgetPtr->GetPreferenceObject(), &wkf::CommentPrefObject::TimeoutChanged, this, &Plugin::PreferencesChanged);
   connect(&rvEnv, &rv::Environment::ClearScenario, this, &Plugin::ClearComments);
}

QList<wkf::PrefWidget*> RvComment::Plugin::GetPreferencesWidgets() const
{
   QList<wkf::PrefWidget*> prefs;
   prefs << mPrefWidgetPtr;
   return prefs;
}


void RvComment::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   Redraw();
}

void RvComment::Plugin::Redraw()
{
   if (wkfEnv.GetPreferenceObject<wkf::CommentPrefObject>()->GetEnabled())
   {
      vespa::VaViewer* viewer   = vaEnv.GetStandardViewer();
      wkf::Scenario*   scenario = vaEnv.GetStandardScenario();
      if (viewer != nullptr && scenario != nullptr)
      {
         for (auto& comments : mComments)
         {
            for (auto& c : comments.second)
            {
               wkf::Platform* platform = scenario->FindPlatformByIndex(c.second.platformIndex());
               if (platform != nullptr)
               {
                  double creationTime = comments.first;
                  int    timeout      = wkfEnv.GetPreferenceObject<wkf::CommentPrefObject>()->GetTimeoutSec();
                  double currentTime  = vaEnv.GetTime()->GetCurrentTime();

                  // Check if a comment already exists for the platform
                  vespa::VaAttachment* attachment = platform->FindAttachment("decorator");
                  if (attachment != nullptr)
                  {
                     wkf::AttachmentDecorator* decorator = dynamic_cast<wkf::AttachmentDecorator*>(attachment);
                     if (decorator != nullptr)
                     {
                        if (currentTime > creationTime && currentTime < creationTime + timeout)
                        {
                           // If a comment currently exists for the platform,
                           // but another comment was made for the currentTime, replace the comment text
                           decorator->SetText(FormatMessage(c.second.text(), comments.first));
                        }
                        else
                        {
                           // If a comment currently exists for the platform,
                           // and the comment does not fall within the timeout window, remove the comment
                           // if the attachment id is the same for the existing attachment.
                           if (decorator->GetUniqueId() == c.first)
                           {
                              platform->RemoveAttachment(decorator->GetUniqueId());
                           }
                        }
                     }
                  }
                  else // If a comment does not exist for the platform
                  {
                     if (currentTime > creationTime && currentTime < creationTime + timeout)
                     {
                        // If a comment does not exist for the platform
                        // and the comment falls within the timeout window,
                        // create a new comment
                        QString team  = QString::fromStdString(platform->GetSide());
                        QColor  color = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamColor(team);

                        auto commentAttachment =
                           vespa::make_attachment<wkf::AttachmentDecorator>(*platform,
                                                                            viewer,
                                                                            FormatMessage(c.second.text(), comments.first));
                        commentAttachment->SetColor(color.redF(), color.greenF(), color.blueF(), color.alphaF());
                        vespa::VaAttachment::LoadAttachment(*commentAttachment);

                        c.first = commentAttachment->GetUniqueId();
                     }
                  }
               }
            }
         }
      }
   }
}

void RvComment::Plugin::PreferencesChanged()
{
   vespa::VaViewer* viewer   = vaEnv.GetStandardViewer();
   wkf::Scenario*   scenario = vaEnv.GetStandardScenario();
   if (viewer != nullptr && scenario != nullptr)
   {
      for (auto& comments : mComments)
      {
         for (auto& c : comments.second)
         {
            wkf::Platform* platform = scenario->FindPlatformByIndex(c.second.platformIndex());
            if (platform != nullptr)
            {
               // Check if a comment already exists for the platform
               vespa::VaAttachment* attachment = platform->FindAttachment("decorator");
               if (attachment != nullptr)
               {
                  platform->RemoveAttachment(attachment->GetUniqueId());
               }
            }
         }
      }
   }

   Redraw();
}

QString RvComment::Plugin::FormatMessage(const std::string& aCommentMessage, double aSimTime) const
{
   std::string message = aCommentMessage;

   // Prepend the timestamp to the message if required by the preferences
   if (wkfEnv.GetPreferenceObject<wkf::CommentPrefObject>()->GetShowTimestamp())
   {
      UtTime timestamp;
      message = timestamp.ToString(aSimTime, wkfEnv.GetPreferenceObject<wkf::UnitsObject>()->GetTimeFormatting()) +
                " " + message;
   }

   return QString::fromStdString(message);
}
