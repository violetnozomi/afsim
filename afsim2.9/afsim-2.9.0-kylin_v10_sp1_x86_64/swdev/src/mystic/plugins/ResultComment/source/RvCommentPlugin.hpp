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
#ifndef RVCOMMENTPLUGIN_HPP
#define RVCOMMENTPLUGIN_HPP

#include "RvEventPipeClasses.hpp"
#include "RvEventPipeMessages.hpp"
#include "RvPlugin.hpp"
#include "comment/WkfCommentDockWidget.hpp"
#include "comment/WkfCommentPrefWidget.hpp"

namespace wkf
{
class Platform;
}

namespace RvComment
{
// sim_time -> vector of pairs of attachment ids and comments
using CommentData = std::map<double, std::vector<std::pair<unsigned int, rv::MsgComment>>>;

// Define Unary Operator for find_if
struct FindCommentByIndex
{
public:
   explicit FindCommentByIndex(const rv::MsgComment& aComment)
      : mComment(std::make_pair(0, aComment))
   {
   }
   bool operator()(const std::pair<unsigned int, rv::MsgComment>& aC1) const
   {
      return (aC1.second.platformIndex() == mComment.second.platformIndex() && aC1.first == mComment.first);
   }

private:
   std::pair<unsigned int, rv::MsgComment> mComment;
};

class Plugin : public rv::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   void CommentRead(rv::ResultDb& aData, const rv::MsgComment& aDrawCommand) override;
public slots:
   void ClearComments(bool aFullReset);

protected:
   void AdvanceTimeRead(const rv::ResultData& aData) override;

private:
   void    Redraw();
   void    PreferencesChanged();
   QString FormatMessage(const std::string& aCommentMessage, double aSimTime) const;

   CommentData mComments;

   PluginUiPointer<wkf::CommentPrefWidget> mPrefWidgetPtr;
   PluginUiPointer<wkf::CommentDockWidget> mDockWidgetPtr;
};
} // namespace RvComment
#endif
