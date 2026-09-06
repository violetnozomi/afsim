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

#include "WkfCommentForwardingService.hpp"

#include <QApplication>

wkf::CommentForwardingService* wkf::CommentForwardingService::mInstancePtr = nullptr;
int                            wkf::CommentForwardingService::mUsageCount  = 0;

wkf::CommentForwardingService* wkf::CommentForwardingService::Request()
{
   mUsageCount++;
   if (!mInstancePtr)
   {
      mInstancePtr = new CommentForwardingService(QApplication::instance());
   }
   return mInstancePtr;
}

void wkf::CommentForwardingService::Release()
{
   mUsageCount--;
   if (mUsageCount == 0)
   {
      delete mInstancePtr;
      mInstancePtr = nullptr;
   }
}

wkf::CommentForwardingService::CommentForwardingService(QObject* aParent)
   : QObject(aParent)
{
}

void wkf::CommentForwardingService::CommentReceived(double aTime, const QString& aPlatformName, const QString& aMessage)
{
   if (mInstancePtr)
   {
      emit mInstancePtr->CommentReady(aTime, aPlatformName, aMessage);
   }
}

void wkf::CommentForwardingService::CommentClear()
{
   if (mInstancePtr)
   {
      emit mInstancePtr->ClearComments();
   }
}
