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

#include "MysticPluginZoneDockWidget.hpp"

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "VaEnvironment.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"
#include "zone/WkfAttachmentZone.hpp"

MysticZoneBrowser::DockWidget::DockWidget(wkf::ZoneBrowserDataContainer& aDataContainer, QWidget* aParentPtr)
   : ZoneBrowserDockWidget(aDataContainer, aParentPtr)
{
}

void MysticZoneBrowser::DockWidget::RemovePlatform(const std::string& aPlatformName)
{
   wkf::Platform* plat = vaEnv.GetStandardScenario()->FindPlatformByName(aPlatformName);

   auto removalLambda = [&, aPlatformName](wkf::ZoneIdentifier& aZoneData)
   {
      bool retVal = (plat && aZoneData.mPlatformName == aPlatformName);
      if (retVal)
      {
         for (auto& it : aZoneData.mZones)
         {
            if (it.mZonePtr)
            {
               it.mZonePtr->GetParent().RemoveAttachment(it.mZonePtr->GetUniqueId());
            }
         }
         aZoneData.mZones.clear();
      }
      // Account for zones associated with other platforms (or global) that are drawn on platform references. Unlike Wk,
      // these should not be removed because the platform can be re-added
      else if (aZoneData.mReferenceNames.count(aPlatformName) > 0)
      {
         for (auto& attIt : aZoneData.mZones)
         {
            if (attIt.mReferenceName == aPlatformName)
            {
               if (attIt.mZonePtr)
               {
                  attIt.mZonePtr->GetParent().RemoveAttachment(attIt.mZonePtr->GetUniqueId());
               }
               // Add to the delayed map to allow for rewinds to times prior to the deletion to automatically redraw the zone
               mDelayedZones[attIt.mReferenceName].emplace(aZoneData.mPlatformName, aZoneData.mZoneName);
            }
         }
      }
      return retVal;
   };

   mDrawZones.erase(std::remove_if(mDrawZones.begin(), mDrawZones.end(), removalLambda), mDrawZones.end());

   // If the platform state is rewinded, it will be re-added to this map, so remove it for now
   if (plat && mAttachmentZoneMap.count(plat->GetUniqueId()) > 0)
   {
      mAttachmentZoneMap.erase(plat->GetUniqueId());
   }

   // in the browser, if the deleted platform had zones, get rid of the platform's entire subtree
   // Note: this does not delete child ZoneSelectionWidgets due to how the widget was constructed, so manually delete those
   for (int i = 0; i < mUI.mTreeWidget->topLevelItemCount(); ++i)
   {
      QTreeWidgetItem* itemPtr = mUI.mTreeWidget->topLevelItem(i);
      if (itemPtr->text(0).toStdString() == aPlatformName)
      {
         delete itemPtr;
         i--;
         continue;
      }
      if (itemPtr->isExpanded())
      {
         // this is essentially a redraw of the dropdown menu: complications with how the dropdown is implemented make
         // it difficult to do otherwise
         itemPtr->setExpanded(false);
         itemPtr->setExpanded(true);
      }
   }
}

vespa::VaEntity* MysticZoneBrowser::DockWidget::GetActiveAnchor(const std::string& aReferenceName)
{
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      auto rvData = rvEnv.GetData();
      if (rvData)
      {
         rv::ResultPlatform* rvAnchor = rvData->FindPlatform(rvData->FindPlatformIndex(aReferenceName));
         if (rvAnchor && rvAnchor->IsActive(rvData->GetSimTime()))
         {
            return scenarioPtr->FindPlatformByName(aReferenceName);
            ;
         }
      }
   }
   return nullptr;
}
