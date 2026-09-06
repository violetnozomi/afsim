// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "aces/WkfAcesDisplayPluginBase.hpp"

#include <cstdlib>

#include <QMenu>
#include <QString>

#include "UtQtMemory.hpp"
#include "WkfAction.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"
#include "WkfPlatform.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "aces/WkfAcesDisplayDockWidget.hpp"

namespace
{
constexpr const char* cACES_DISPLAY = "ACES_Display";
}

wkf::AcesDisplay::PluginBase::PluginBase(const QString& aPluginName, const size_t aUniqueID, const QFlags<Roles>& aRoles)
   : wkf::Plugin(aPluginName, aUniqueID, aRoles)
{
   mCallbacks.Add(wkf::Observer::PlatformPendingDeletion.Connect(&PluginBase::PlatformToBeDeletedCB, this));
}

wkf::AcesDisplay::PluginBase::~PluginBase() {}

void wkf::AcesDisplay::PluginBase::BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr)
{
   if (aEntityPtr->IsA_TypeOf<wkf::Platform>())
   {
      wkf::Platform* platPtr = dynamic_cast<wkf::Platform*>(aEntityPtr);
      if ((HasSituationAwarenessProcessor(platPtr->GetIndex())) && mDisplays.find(platPtr->GetIndex()) == mDisplays.end())
      {
         // That big nested-if monstrosity means that this will only run if:
         //   - aEntityPtr is a pointer to a wkf::Platform
         //   - AND the platform has a SituationAwarenessProcessor,
         //   - AND there is no dock widget for this platform already
         auto airCombatMenu = aMenu->findChild<QMenu*>("Air Combat");
         if (!airCombatMenu)
         {
            airCombatMenu = ut::qt::make_qt_ptr<QMenu>(QString("Air Combat"), aMenu);
            aMenu->addMenu(airCombatMenu);
            airCombatMenu->setObjectName("Air Combat");
         }

         QAction* acesViewAction = new QAction(QIcon::fromTheme("tether"), QString("ACES View"), airCombatMenu);
         airCombatMenu->addAction(acesViewAction);
         connect(acesViewAction,
                 &QAction::triggered,
                 this,
                 [=]()
                 {
                    auto pd = ConnectToPlatform(platPtr);
                    if (pd != nullptr)
                    {
                       pd->show();
                       NewDisplay(platPtr);
                       SetDisplayFocus(platPtr->GetIndex());
                       connect(pd, &DockWidget::DisplayGainedFocus, this, &PluginBase::SetDisplayFocus);
                       connect(this, &PluginBase::SetTargetPlatforms, pd, &DockWidget::SetTargetPlatforms);
                    }
                 });
      }
   }
}

void wkf::AcesDisplay::PluginBase::PlatformToBeDeletedCB(wkf::Platform* aPlatformPtr)
{
   size_t index = aPlatformPtr->GetIndex();
   // If the deleted platform was in the list of displays...
   if (mDisplays.find(index) != mDisplays.end())
   {
      //... then remove it from the interface's list of platforms of interest.
      RemoveDisplay(index);
   }
}

void wkf::AcesDisplay::PluginBase::GuiUpdate()
{
   // If the simulation was reset, reset our container and reset displays.
   if (mDataContainer.GetSimulationInitialized())
   {
      Reset();
      mDataContainer.SetSimulationInitialized(false);
   }

   // Check if we have displays to update first
   if (mDisplays.size() > 0)
   {
      // Loop through each ACES Display finding and updating its data.
      for (auto& display : mDisplays)
      {
         if (display.second->ConnectionEstablished())
         {
            // If we have data in the data container for this ACES display
            if (mDataContainer.GetAcesData().find(display.first) != mDataContainer.GetAcesData().end())
            {
               const wkf::AcesDisplay::AcesData&           data  = mDataContainer.GetAcesData().at(display.first);
               std::vector<wkf::AcesDisplay::EntityTruth>& truth = mDataContainer.GetAcesTruthPlatforms();
               UpdateEntityTruthData(truth);
               mDisplays[display.first]->SetAcesData(data);
               mDisplays[display.first]->SetAcesTruthPlatforms(truth);
               auto& platformComment =
                  GetPlatformComment(display.first, mDataContainer.GetPlatformComments(), data.platformData.simTime);
               if (!platformComment.empty())
               {
                  mDisplays[display.first]->SetAcesPlatformComment(platformComment);
               }
            }
         }
      }
   }
}

void wkf::AcesDisplay::PluginBase::SetDisplayFocus(const size_t aPlatformIndex)
{
   mDisplayInFocus = aPlatformIndex;

   // Turn off focus for all other displays
   for (auto& display : mDisplays)
   {
      if (display.second->GetConnectedPlatformIndex() != mDisplayInFocus && display.second->GetHasFocus())
      {
         display.second->SetHasFocus(false);
      }
   }
}

bool wkf::AcesDisplay::PluginBase::HasSituationAwarenessProcessor(const size_t aPlatformIndex)
{
   if (mDataContainer.GetPlatformsWithSAP().find(aPlatformIndex) != mDataContainer.GetPlatformsWithSAP().end())
   {
      return true;
   }
   return false;
}

void wkf::AcesDisplay::PluginBase::NewDisplay(wkf::Platform* aPlatform)
{
   mPlatformsOfInterest.insert(aPlatform->GetIndex());
}

void wkf::AcesDisplay::PluginBase::RemoveDisplay(size_t aIndex)
{
   mPlatformsOfInterest.erase(aIndex);

   if (mTargetPlatforms.find(aIndex) != mTargetPlatforms.end())
   {
      mTargetPlatforms.erase(aIndex);
   }
}

void wkf::AcesDisplay::PluginBase::Reset()
{
   for (auto& display : mDisplays)
   {
      delete display.second.data();
   }
   mDisplays.clear();
}

void wkf::AcesDisplay::PluginBase::UpdateEntityTruthData(std::vector<EntityTruth>& aTruthPlatforms)
{
   auto scenario = vaEnv.GetStandardScenario();
   if (scenario != nullptr)
   {
      for (auto& entity : aTruthPlatforms)
      {
         wkf::Platform* platPtr = scenario->FindPlatformByIndex(entity.index);
         if (platPtr != nullptr && EntityTruthValid(entity))
         {
            const auto& pos = platPtr->GetPosition();
            // Fill in the general data needed to draw here.
            entity.lat_deg = pos.GetLat();
            entity.lon_deg = pos.GetLon();

            double heading_rad, dummypitch, dummyroll;
            platPtr->GetOrientation(heading_rad, dummypitch, dummyroll);
            entity.heading_deg = heading_rad * UtMath::cDEG_PER_RAD;
            entity.altitude_ft = pos.GetAlt() * UtMath::cFT_PER_M;
            entity.speed_kts   = platPtr->GetVelocityWCS().Magnitude() * UtMath::cNMPH_PER_MPS;

            entity.altitudeValid = true;
            entity.speedValid    = true;
            entity.headingValid  = true;
         }
      }
   }
}

const std::string
wkf::AcesDisplay::PluginBase::GetPlatformComment(const size_t                                           aPlatformIndex,
                                                 const std::map<size_t, std::map<double, std::string>>& aPlatformComments,
                                                 const double                                           aSimTime) const
{
   // Loop through our platform comments
   if (aPlatformComments.find(aPlatformIndex) != aPlatformComments.end())
   {
      auto& comments = aPlatformComments.at(aPlatformIndex);
      // Find the first plaform comment before this simTime
      double commentTime = -1.0;
      for (auto& comment : comments)
      {
         if (comment.first > commentTime && comment.first <= aSimTime)
         {
            commentTime = comment.first;
         }
      }

      // Once we have the newest comment, return it
      if (commentTime >= 0.0)
      {
         return comments.at(commentTime);
      }
   }
   // Otherwise, return an empty string
   return "";
}

wkf::AcesDisplay::DockWidget* wkf::AcesDisplay::PluginBase::ConnectToPlatform(wkf::Platform* aPlatformPtr)
{
   // Store pointer to this dockWidget,
   // since name is not unique due to multiple views can point to same entity, use a key value
   if (mDisplays.find(aPlatformPtr->GetIndex()) == mDisplays.end())
   {
      auto it = mDisplays
                   .emplace(aPlatformPtr->GetIndex(),
                            new DockWidget(QString::fromStdString(cACES_DISPLAY + aPlatformPtr->GetName()),
                                           aPlatformPtr,
                                           wkfEnv.GetMainWindow()->centralWidget()))
                   .first;
      int  key        = it->first;
      auto dockWidget = it->second;
      // Connect to destroyed signal so we can remove the dock widget from our list of widgets
      connect(dockWidget.data(),
              &QObject::destroyed,
              [=](QObject* obj)
              {
                 RemoveDisplay(key);
                 mDisplays.erase(key);
              });

      // Set Window title
      dockWidget->setWindowTitle(QString("ACES: %1").arg(QString::fromStdString(aPlatformPtr->GetName())));

      // Set current prefs
      return dockWidget;
   }
   else
   {
      return nullptr;
   }
}

void wkf::AcesDisplay::PluginBase::Tether()
{
   wkf::Platform* platPtr = wkfEnv.GetPlatformOfInterest();
   if (platPtr)
   {
      auto pd = ConnectToPlatform(platPtr);
      if (pd != nullptr)
      {
         NewDisplay(platPtr);
         SetDisplayFocus(platPtr->GetIndex());
         connect(pd, &DockWidget::DisplayGainedFocus, this, &PluginBase::SetDisplayFocus);
         connect(this, &PluginBase::SetTargetPlatforms, pd, &DockWidget::SetTargetPlatforms);
         pd->show();
      }
   }
}
