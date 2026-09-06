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

#ifndef RV_BOOKMARKBROWSER
#define RV_BOOKMARKBROWSER

#include <QDockWidget>

#include "RvEnvironment.hpp"
#include "RvEventPipeClasses.hpp"
#include "ui_BookmarkBrowser.h"

namespace RvTimeController
{
class BookmarkBrowser : public QDockWidget
{
   Q_OBJECT
public:
   BookmarkBrowser(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   ~BookmarkBrowser() override = default;
   void AddBookmark(const rv::MsgBookmark& aBookmark);

signals:
   void BookmarkSelected(const rv::MsgBookmark& aBookmark);

private:
   void ResetBookmarks();
   void SendBookmark();
   void SetBrowserItems();

   Ui::BookmarkBrowser mUi;
   struct TimeCompare
   {
      bool operator()(const rv::MsgBookmark& aTime1, const rv::MsgBookmark& aTime2) const
      {
         return aTime1.time() < aTime2.time();
      }
   };
   std::multiset<rv::MsgBookmark, TimeCompare> mBookmarks;
};
} // namespace RvTimeController

#endif
