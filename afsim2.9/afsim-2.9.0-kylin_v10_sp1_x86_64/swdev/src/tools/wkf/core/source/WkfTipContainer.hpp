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

#ifndef WKFTIPCONTAINER_HPP
#define WKFTIPCONTAINER_HPP

#include "wkf_export.h"

#include <QObject>
#include <QString>
#include <QStringList>

namespace wkf
{
// This class handled to gathering of tips within WKF applications.
// It assume the collections of tips is static after initialization and tips are not modified during the
//  lifetime of the application. (ie: no tips are added, removed, or modified)
class WKF_EXPORT TipContainer : public QObject
{
   Q_OBJECT

public:
   TipContainer(QObject* aParentPtr);

   // Returns a list of tips that is a collections of tips provided by the tips.txt file located in the
   //  qt resource file and tips provided by wkf::Plugins via the GetTips() virtual method.
   const QStringList& GetTips() const { return mTips; }

private:
   void ReadTipsFromFile();
   void QueryPluginsForTips();
   void UpdateDocLinks();

   QStringList mTips{};
};
} // namespace wkf

#endif
