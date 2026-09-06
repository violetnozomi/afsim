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

#include "WkfBM_PlatformStatusModel.hpp"

#include <string>

#include <QMenu>

#include "WkfBM_PlatformDatabase.hpp"
#include "WkfEnvironment.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfVtkEnvironment.hpp"

wkf::bm::PlatformStatusModel::PlatformStatusModel(const PlatformPlotDatabase* aDatabase)
   : ModelBase({"Side", "Type", "Platform"}, true)
   , mDatabase(*aDatabase)
{
   auto* teamVisibility = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();

   connect(teamVisibility, &wkf::TeamVisibilityPrefObject::TeamColorChanged, this, &PlatformStatusModel::UpdateTeamVisibility);
   connect(teamVisibility,
           &wkf::TeamVisibilityPrefObject::TeamVisibilityChanged,
           this,
           &PlatformStatusModel::UpdateTeamVisibility);
}

wkf::bm::LeafProxy wkf::bm::PlatformStatusModel::Platform(const QString& aSide, const QString& aType, const QString& aName)
{
   return GetOrAddLeaf({aSide, aType, aName});
}

wkf::bm::LeafProxy wkf::bm::PlatformStatusModel::Platform(const wkf::Platform* aPlatform)
{
   if (aPlatform)
   {
      auto side = QString::fromStdString(aPlatform->GetSide());
      auto type = QString::fromStdString(aPlatform->GetTypeAFSIM());
      auto name = QString::fromStdString(aPlatform->GetName());

      return GetOrAddLeaf({side, type, name});
   }

   return LeafProxy{};
}

void wkf::bm::PlatformStatusModel::UpdatePlatformColors(double aSimTime)
{
   for (const auto& platform : mDatabase)
   {
      auto proxy = GetNode({platform.second.mSide, platform.second.mTypes.first(), platform.second.mName});
      if (platform.second.mStartTime > aSimTime)
      {
         proxy.SetColor(QColor{0, 0, 128});
      }
      else if (platform.second.mEndTime < aSimTime)
      {
         proxy.SetColor(QColor{128, 0, 0});
      }
      else
      {
         proxy.SetColor(ut::nullopt);
      }
   }
}

void wkf::bm::PlatformStatusModel::InitializeNode(NodeProxy aNode)
{
   if (aNode.GetColumn() == 0)
   {
      // Platform Side "Column"
      auto* prefs = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();
      if (prefs)
      {
         aNode.SetHiddenByProgram(!prefs->IsTeamVisible(aNode.GetTitle()));
         aNode.SetColor(prefs->GetTeamColor(aNode.GetTitle()));
      }
   }
   else if (aNode.GetColumn() == 1)
   {
      // Platform Types Column
      aNode.SetColor(QColor(0, 128, 128));
   }
}

void wkf::bm::PlatformStatusModel::UpdateTeamVisibility()
{
   auto* teamVisibility = wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>();

   for (const auto& team : teamVisibility->GetTeamVisibilityMap())
   {
      auto np = GetNode({team.first});
      if (np)
      {
         np.SetHiddenByProgram(!team.second);
         np.SetColor(teamVisibility->GetTeamColor(team.first));
      }
   }

   UpdateCache();
}
