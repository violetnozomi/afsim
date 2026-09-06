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

#ifndef WKFTEAMVISIBILITYWIDGET_HPP
#define WKFTEAMVISIBILITYWIDGET_HPP

#include "wkf_export.h"

#include <map>
#include <mutex>
#include <set>

#include <QListWidgetItem>
#include <QWidget>

#include "WkfEnvironment.hpp"
#include "WkfPlatform.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "ui_WkfTeamVisibilityWidget.h"

namespace wkf
{
class WKF_EXPORT TeamVisibilityWidget : public QWidget
{
public:
   TeamVisibilityWidget(QWidget* aParent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
   void Update();

private:
   void VisibilityChanged(QListWidgetItem* item);
   void Populate();
   void PreferencesChanged();

   Ui::WkfTeamVisibilityWidget mUI;
   std::set<QString>           mPresentSides;
   std::map<QString, QString>  mPlatformSide;
   bool                        mModified{false};
   bool                        mCleanupRequested{false};

   void StandardScenarioRemovedCB(wkf::Scenario* aScenarioPtr);
   void PlatformAddedCB(vespa::VaEntity* aEntityPtr);
   void PlatformDeletedCB(vespa::VaEntity* aEntityPtr);
   void PlatformChangedSidesCB(wkf::Platform* aPlatformPtr);
   void RequestTeamCleanup();
   void CleanupTeams();

   vespa::VaCallbackHolder mCallbacks;
};
} // namespace wkf
#endif // !TEAMVISIBILITYWIDGET_HPP
