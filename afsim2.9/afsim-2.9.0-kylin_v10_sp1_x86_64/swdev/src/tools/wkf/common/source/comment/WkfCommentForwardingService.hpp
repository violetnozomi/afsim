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
#ifndef WKFCOMMENTFORWARDINGSERVICE_HPP
#define WKFCOMMENTFORWARDINGSERVICE_HPP

#include "wkf_common_export.h"

#include <QObject>

namespace wkf
{
class WKF_COMMON_EXPORT CommentForwardingService : public QObject
{
   Q_OBJECT
public:
   static CommentForwardingService* Request();
   static void                      Release();
   static void CommentReceived(double aTime, const QString& aPlatformName, const QString& aMessage);
   static void CommentClear();

signals:
   void CommentReady(double aTime, const QString& aPlatformName, const QString& aMessage);
   void ClearComments();

private:
   CommentForwardingService(QObject* aParent);
   ~CommentForwardingService() override = default;

   static int                       mUsageCount;
   static CommentForwardingService* mInstancePtr;
};
} // namespace wkf

#endif